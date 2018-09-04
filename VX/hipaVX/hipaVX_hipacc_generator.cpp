#include "hipaVX_hipacc_generator.hpp"

#include "config_reader.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <map>

using std::string;

static string hipaVX_folder = "VX/hipaVX";

static std::map<vx_df_image, string> VX_DF_IMAGE_to_hipacc = {
	{VX_DF_IMAGE_U8, "uchar"},
	{VX_DF_IMAGE_S16, "short"},
};

static string read_file(const string &filename)
{
	std::ifstream in(filename);

	// Fastet way to read a whole file https://stackoverflow.com/a/116220
	return static_cast<std::stringstream const&>(std::stringstream() << in.rdbuf()).str();
}

static string use_template(const string &template_string, const string &template_variable, const string &actual_variable)
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

static string use_template(const string &template_string, const string &template_variable, const int actual_variable)
{
	return use_template(template_string, template_variable, std::to_string(actual_variable));
}

static string use_template(const string &template_string, const string &template_variable, const unsigned int actual_variable)
{
	return use_template(template_string, template_variable, std::to_string(actual_variable));
}

static void write_file(const string &filename, const string &content)
{
	std::ofstream out(filename);
	out << content;
}

static string generate_image_name(vx_image image)
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

string kernel_builder(const std::vector<Kernel_Variable*>& variables, const std::vector<string>& kernel_code, const string& kernel_name)
{

	string member_variables;
	string constructor_parameters;
	string constructor_init_list;
	string add_accessor;

	string tabs = "\t";

	for(const auto& kv: variables)
	{
		string s = kv->generate_kerneldefinition();

		member_variables += tabs + s + ";\n";
		constructor_parameters += ", " + s;
		constructor_init_list += ", " + kv->name + "(" + kv->name + ")";
		if (kv->is_accessor())
		{
			add_accessor += tabs + '\t' + "add_accessor(&" + kv->name + ");\n";
		}
	}

	string kernel;
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

string kernelcall_builder(const std::vector<Kernelcall_Variable*>& call_variables,
						  const std::vector<std::vector<Kernelcall_Variable *>>& kernel_calls)
{
	string to_return;
	for(const auto& kv: call_variables)
	{
		auto strings = kv->generate_kernelcall();
		for (const auto& s: strings)
		{
			to_return += '\t' + s + '\n';
		}
		to_return += '\n';
	}

	to_return += '\n';

	return to_return;
}


string node_generator(HipaVX::WriteImageNode* n, Type t)
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
string node_generator(HipaVX::Sobel3x3Node* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/sobel.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->src->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->dst_x->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/sobel.call");
		string s = kernelcall_builder(cs.kcv, cs.kc);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->src->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->dst_x->col]);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->src));
		s = use_template(s, "IMAGE_OUT_X", generate_image_name(n->dst_x));
		s = use_template(s, "IMAGE_OUT_Y", generate_image_name(n->dst_y));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
string node_generator(HipaVX::ConvertDepthNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/convert.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->src->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->dst->col]);

		s = use_template(s, "SHIFT", n->shift->i32);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/convert.call");
		string s = kernelcall_builder(cs.kcv, cs.kc);


		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->src->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->dst->col]);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->src));
		s = use_template(s, "IMAGE_IN_WIDTH", n->src->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->src->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->dst));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
string node_generator(HipaVX::MagnitudeNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/magnitude.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->grad_x->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->mag->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/magnitude.call");
		string s = kernelcall_builder(cs.kcv, cs.kc);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->grad_x->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->mag->col]);

		s = use_template(s, "INPUT_IMAGE_GRAD_X", generate_image_name(n->grad_x));
		s = use_template(s, "INPUT_IMAGE_GRAD_Y", generate_image_name(n->grad_y));
		s = use_template(s, "IMAGE_IN_WIDTH", n->grad_x->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->grad_x->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->mag));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}

string node_generator(HipaVX::AndNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/and.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in_1->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/and.call");
		string s = kernelcall_builder(cs.kcv, cs.kc);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in_1->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "IMAGE_IN_1", generate_image_name(n->in_1));
		s = use_template(s, "IMAGE_IN_2", generate_image_name(n->in_2));
		s = use_template(s, "IMAGE_IN_WIDTH", n->in_1->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in_1->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
string node_generator(HipaVX::XorNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/xor.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in_1->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/xor.call");
		string s = kernelcall_builder(cs.kcv, cs.kc);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in_1->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "IMAGE_IN_1", generate_image_name(n->in_1));
		s = use_template(s, "IMAGE_IN_2", generate_image_name(n->in_2));
		s = use_template(s, "IMAGE_IN_WIDTH", n->in_1->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in_1->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
string node_generator(HipaVX::OrNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/or.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in_1->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/or.call");
		string s = kernelcall_builder(cs.kcv, cs.kc);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in_1->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "IMAGE_IN_1", generate_image_name(n->in_1));
		s = use_template(s, "IMAGE_IN_2", generate_image_name(n->in_2));
		s = use_template(s, "IMAGE_IN_WIDTH", n->in_1->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in_1->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
string node_generator(HipaVX::NotNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/and.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/and.call");
		string s = kernelcall_builder(cs.kcv, cs.kc);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "IMAGE_IN", generate_image_name(n->in));
		s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}


string node_generator(HipaVX::AbsDiffNode* n, Type t)
{}
string node_generator(HipaVX::AddNode* n, Type t)
{}
string node_generator(HipaVX::SubtractNode* n, Type t)
{}


}
