#include "hipaVX_hipacc_generator.hpp"


#include "config_reader.hpp"




std::string read_file(const std::string &filename)
{
	std::ifstream in(filename);

	// Fastet way to read a whole file https://stackoverflow.com/a/116220
	return static_cast<std::stringstream const&>(std::stringstream() << in.rdbuf()).str();
}

std::string use_template(const std::string &template_string, const std::string &template_variable, const std::string &actual_variable)
{
	const string temp = "@@@" + template_variable + "@@@";
	const auto temp_len = temp.length();
	string templated = template_string;

	size_t index = 0;
	while (true)
	{
		index = templated.find(temp, index);
		if (index == string::npos)
			break;

		auto first = templated.substr(0, index);
		auto last = templated.substr(index + temp_len);

		templated = first + actual_variable + last;
	}

	return templated;
}

std::string use_template(const std::string &template_string, const std::string &template_variable, const int actual_variable)
{
	return use_template(template_string, template_variable, std::to_string(actual_variable));
}

std::string use_template(const std::string &template_string, const std::string &template_variable, const unsigned int actual_variable)
{
	return use_template(template_string, template_variable, std::to_string(actual_variable));
}

void write_file(const std::string &filename, const std::string &content)
{
	std::ofstream out(filename);
	out << content;
}

std::string generate_image_name(vx_image image)
{
	return string("Image_") + std::to_string(image->my_id);
}

void process_graph(vx_graph graph)
{
	string main = read_file(hipaVX_folder + "/hipacc_main.template");

	string images;
	const string image_decl_template = "\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@);\n";

	for(const auto image: graph->used_images)
	{
		string image_name = generate_image_name(image);

		string temp = image_decl_template;
		// BAD HACK
		if(const HipaVX::FileinputImage* fim = dynamic_cast<const HipaVX::FileinputImage*>(image))
		{
			const string fim_decl_template = "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = load_data<@@@DATATYPE@@@>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, 1, \"@@@FILENAME@@@\");\n"
											 "\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
			temp = fim_decl_template;
			temp = use_template(temp, "FILENAME", fim->file);
		}

		temp = use_template(temp, "DATATYPE", VX_DF_IMAGE_to_hipacc[image->col]);
		temp = use_template(temp, "IMAGE_NAME", image_name);
		temp = use_template(temp, "IMAGE_WIDTH", image->w);
		temp = use_template(temp, "IMAGE_HEIGHT", image->h);

		images += temp;
	}

	main = use_template(main, "IMAGE_DEFINITIONS", images);

	string node_definitions;
	for(const auto node: graph->graph)
	{
		node_definitions += node->generateClassDefinition();
	}

	main = use_template(main, "KERNEL_DEFINITIONS", node_definitions);

	string node_calls;
	for(const auto node: graph->graph)
	{
		node_calls += node->generateNodeCall();
	}

	main = use_template(main, "KERNEL_CALLS", node_calls);



	write_file("main.hipaVX.cpp", main);
}



namespace generator
{

std::string node_generator(HipaVX::WriteImageNode* n, Type t)
{
	if (t == Type::Definition)
	{
		return "";
	}
	else if (t == Type::Call)
	{
		string s = "\t@@@IMAGE_DATATYPE@@@ *data_@@@IMAGE@@@ = @@@IMAGE@@@.data();\n";
		s       += "\tsave_data(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_CHANNELS@@@, data_@@@IMAGE@@@, \"@@@FILENAME@@@\");\n";
		s       += "\n";


		s = use_template(s, "IMAGE", generate_image_name(n->src));
		s = use_template(s, "IMAGE_DATATYPE", VX_DF_IMAGE_to_hipacc[n->src->col]);

		s = use_template(s, "IMAGE_WIDTH", n->src->w);
		s = use_template(s, "IMAGE_HEIGHT", n->src->h);
		s = use_template(s, "IMAGE_CHANNELS", 1); //TODO
		s = use_template(s, "FILENAME", n->dst_file);

		return s;
	}
	return "SOMETHING IS WRONG";
}

string kernel_builder(const std::vector<Kernel_Variable*>& variables, const std::vector<string>& kernel_code, const std::string& kernel_name)
{

	std::string member_variables;
	std::string constructor_parameters;
	std::string constructor_init_list;
	std::string add_accessor;

	string tabs = "\t\t";

	for(const auto& kv: variables)
	{
		std::string s = kv->generate_kerneldefinition_part();

		member_variables += tabs + s + ";\n";
		constructor_parameters += ", " + s;
		constructor_init_list += ", " + kv->name + "(" + kv->name + ")";
		if (kv->is_accessor())
		{
			add_accessor += tabs + '\t' + "add_accessor(&" + kv->name + ");\n";
		}
	}

	std::string kernel;
	for(const auto& line: kernel_code)
	{
		kernel += tabs + '\t' + line + '\n';
	}

	string s = read_file(hipaVX_folder + "/kernels/general_kernel_definition");
	s = use_template(s, "KERNEL_NAME", kernel_name);
	s = use_template(s, "VARIABLES", member_variables);
	s = use_template(s, "VARIABLES_CONSTRUCTOR_PARAMS", constructor_parameters);
	s = use_template(s, "VARIABLES_CONSTRUCTOR_INIT_LIST", constructor_init_list);
	s = use_template(s, "ADD_ACCESSOR", add_accessor);
	s = use_template(s, "MISC_CONSTRUCTOR", "");
	s = use_template(s, "KERNEL", kernel);

	return s;
}

string kernelcall_builder(const std::vector<Kernel_Variable*>& variables,
						  const std::vector<Kernelcall_Variable*>& call_variables,
						  const std::vector<std::vector<Kernelcall_Variable *>>& kernel_calls,
						  const std::string& kernel_name)
{
	std::string to_return;
	for(const auto& kv: call_variables)
	{
		auto strings = kv->generate_kernelcall_part();
		for (const auto& s: strings)
		{
			to_return += '\t' + s + '\n';
		}
		to_return += '\n';
	}

	to_return += '\n';

	for(size_t i = 0; i < kernel_calls.size(); i++)
	{
		const auto& call = kernel_calls[i];
		std::string instance_name = kernel_name + "_instance_" + std::to_string(i);
		to_return += '\t' + kernel_name + "_@@@ID@@@ " + instance_name + "(\n";
		for(size_t j = 0; j < call.size(); j++)
		{
			if(j != call.size()-1)
				to_return += "\t\t" + call[j]->get_name() + ",\n";
			else
				to_return += "\t\t" + call[j]->get_name() + ");\n";
		}

		to_return += '\t' + instance_name + ".execute();\n\n";
	}

	return to_return;
}

std::string node_generator(HipaVX::Sobel3x3Node* n, Type t)
{
	auto cs = read_config(hipaVX_folder + "/kernels/sobel.def");


	if (t == Type::Definition)
	{
		string s = kernel_builder(cs.kv, cs.k, "Sobel");

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->src->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->dst_x->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		string s = kernelcall_builder(cs.kv, cs.kcv, cs.kc, "Sobel");

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->src->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->dst_x->col]);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->src));
		s = use_template(s, "IMAGE_OUT_X", generate_image_name(n->dst_x));
		s = use_template(s, "IMAGE_OUT_Y", generate_image_name(n->dst_y));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}

	if (t == Type::Definition)
	{
		string s = read_file(hipaVX_folder + "/kernels/sobel.kernel_definition");

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->src->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->dst_x->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		string s = read_file(hipaVX_folder + "/kernels/sobel.kernel_call");


		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->src->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->dst_x->col]);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->src));
		s = use_template(s, "OUTPUT_IMAGE_X", generate_image_name(n->dst_x));
		s = use_template(s, "OUTPUT_IMAGE_Y", generate_image_name(n->dst_y));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
std::string node_generator(HipaVX::ConvertDepthNode* n, Type t)
{
	if (t == Type::Definition)
	{
		string s = read_file(hipaVX_folder + "/kernels/convert.kernel_definition");

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->src->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->dst->col]);

		s = use_template(s, "SHIFT", n->shift->i32);

		return s;
	}
	else if (t == Type::Call)
	{
		string s = read_file(hipaVX_folder + "/kernels/convert.kernel_call");


		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->src->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->dst->col]);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->src));
		s = use_template(s, "INPUT_IMAGE_WIDTH", n->src->w);
		s = use_template(s, "INPUT_IMAGE_HEIGHT", n->src->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->dst));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
std::string node_generator(HipaVX::MagnitudeNode* n, Type t)
{
	if (t == Type::Definition)
	{
		string s = read_file(hipaVX_folder + "/kernels/magnitude.kernel_definition");

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->grad_x->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->mag->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		string s = read_file(hipaVX_folder + "/kernels/magnitude.kernel_call");


		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->grad_x->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->mag->col]);

		s = use_template(s, "INPUT_IMAGE_GRAD_X", generate_image_name(n->grad_x));
		s = use_template(s, "INPUT_IMAGE_GRAD_Y", generate_image_name(n->grad_y));
		s = use_template(s, "INPUT_IMAGE_WIDTH", n->grad_x->w);
		s = use_template(s, "INPUT_IMAGE_HEIGHT", n->grad_x->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->mag));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
}
