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
	{VX_DF_IMAGE_S32, "int"},
	{VX_DF_IMAGE_U32, "uint"},
	{VX_DF_IMAGE_RGBX, "uchar4"},
	{VX_TYPE_FLOAT32, "float"}, //Not really a vx_df_image type
	{VX_TYPE_INT8, "uchar"}, //Not really a vx_df_image type
	{VX_TYPE_INT32, "int"} //Not really a vx_df_image type
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

		index += actual_variable.length();
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

static string use_template(const string &template_string, const string &template_variable, const double actual_variable)
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

std::vector<HipaVX::Image*> get_all_images(HipaVX::Graph *g)
{
	std::vector<HipaVX::Image*> images;
	std::vector<HipaVX::Node*> nodes = g->graph;

	while(!nodes.empty())
	{
		auto node = nodes[nodes.size() - 1];
		nodes.pop_back();

		for(auto ref: node->get_inputs())
		{
			if (ref->type == VX_TYPE_IMAGE)
				images.push_back((HipaVX::Image*) ref);
		}
		for(auto ref: node->get_outputs())
		{
			if (ref->type == VX_TYPE_IMAGE)
				images.push_back((HipaVX::Image*) ref);
		}

		auto subnodes = node->get_subnodes();
		nodes.insert(nodes.end(), subnodes.begin(), subnodes.end());
	}

	std::sort(images.begin(), images.end());
	images.erase(std::unique(images.begin(), images.end()), images.end());

	return images;
}

void process_graph(vx_graph graph)
{
	string main = read_file(hipaVX_folder + "/hipacc_main.template");

	string images;
	const string image_decl_template = "\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@);\n";

	auto used_images = get_all_images(graph);
	for(const auto image: used_images)
	{
		string image_name = generate_image_name(image);

		string temp = image_decl_template;

		// BAD HACKs
		const HipaVX::FileinputImage* fim = dynamic_cast<const HipaVX::FileinputImage*>(image);
		const HipaVX::Array* arr = dynamic_cast<const HipaVX::Array*>(image);
		if(fim)
		{
			string fim_decl_template;
			if (fim->col == VX_DF_IMAGE_RGBX)
			{
				fim_decl_template = "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = (@@@DATATYPE@@@*) load_data<uchar>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, 4, \"@@@FILENAME@@@\");\n"
									"\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
			}
			else
			{
				fim_decl_template = "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = load_data<@@@DATATYPE@@@>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, 1, \"@@@FILENAME@@@\");\n"
									"\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
			}

			temp = fim_decl_template;
			temp = use_template(temp, "FILENAME", fim->file);
		}
		else if(arr)
		{
			continue;//Currently arr is only used in Harris corner
			// There is a bug where reads of images after writes are not transpiled correctly
			// Therefore i write to this array when constructing -> constructor has to be called later
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

string kernelcall_builder(const std::vector<Kernelcall_Variable*>& call_variables)
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
		string s = "\t@@@IMAGE_DATATYPE@@@ *data_@@@IMAGE@@@_@@@ID@@@ = @@@IMAGE@@@.data();\n";
		s       += "\tsave_data(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_CHANNELS@@@, @@@CONVERSION@@@ data_@@@IMAGE@@@_@@@ID@@@, \"@@@FILENAME@@@\");\n";
		s       += "\n";


		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "IMAGE", generate_image_name(n->in));
		s = use_template(s, "IMAGE_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);

		s = use_template(s, "IMAGE_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_HEIGHT", n->in->h);
		if (n->in->col == VX_DF_IMAGE_RGBX)
		{
			s = use_template(s, "IMAGE_CHANNELS", 4);
			s = use_template(s, "CONVERSION", "(uchar*)");
		}
		else
		{
			s = use_template(s, "IMAGE_CHANNELS", 1); //TODO
			s = use_template(s, "CONVERSION", "");
		}
		s = use_template(s, "FILENAME", n->out_file);

		return s;
	}
	return "SOMETHING IS WRONG";
}
string node_generator(HipaVX::ConvertDepthNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/convert.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "SHIFT", n->shift->i32);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/convert.call");
		string s = kernelcall_builder(cs.kcv);


		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
		s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);
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
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/not.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/not.call");
		string s = kernelcall_builder(cs.kcv);

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

string node_generator(HipaVX::Dilate* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/local/dilate_3x3.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/local/dilate_3x3.call");
		string s = kernelcall_builder(cs.kcv);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);
		s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
string node_generator(HipaVX::Erode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/local/erode_3x3.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/local/erode_3x3.call");
		string s = kernelcall_builder(cs.kcv);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);
		s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}

string node_generator(HipaVX::VXChannelExtractNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/channel_extract.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		string channel;
		switch(n->channel)
		{
		case VX_CHANNEL_B:
		case VX_CHANNEL_0:
			channel = "x";
			break;
		case VX_CHANNEL_G:
		case VX_CHANNEL_1:
			channel = "y";
			break;
		case VX_CHANNEL_R:
		case VX_CHANNEL_2:
			channel = "z";
			break;
		case VX_CHANNEL_A:
		case VX_CHANNEL_3:
			channel = "w";
			break;
		default:
			throw std::runtime_error("Unsupported channeltype in string node_generator(HipaVX::VXChannelExtractNode* n, Type t)");
		}
		s = use_template(s, "CHANNEL", channel);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/channel_extract.call");
		string s = kernelcall_builder(cs.kcv);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);
		s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
string node_generator(HipaVX::VXChannelCombineNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/channel_combine.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in_1->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/channel_combine.call");
		string s = kernelcall_builder(cs.kcv);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in_1->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);
		s = use_template(s, "IMAGE_IN_WIDTH", n->in_1->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in_1->h);

		s = use_template(s, "INPUT_IMAGE_1", generate_image_name(n->in_1));
		s = use_template(s, "INPUT_IMAGE_2", generate_image_name(n->in_2));
		s = use_template(s, "INPUT_IMAGE_3", generate_image_name(n->in_3));
		s = use_template(s, "INPUT_IMAGE_4", generate_image_name(n->in_4));
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}





#include <type_traits>

template <typename T>
std::string node_generator(HipaVX::LinearMask<T>* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/local/linear_mask.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "ELEMENT_OPERATION", "");
		switch(n->normalization->data_type)
		{
		case VX_TYPE_UINT8:
			s = use_template(s, "SUM_OPERATION", " * " + std::to_string(n->normalization->ui8));
			break;
		case VX_TYPE_INT32:
			s = use_template(s, "SUM_OPERATION", " * " + std::to_string(n->normalization->i32));
			break;
		case VX_TYPE_FLOAT32:
			s = use_template(s, "SUM_OPERATION", " * " + std::to_string(n->normalization->f32));
			break;
		default:
			throw std::runtime_error("std::string node_generator(HipaVX::LinearMask<T>* n, Type t):\n\tNot supported Scalar type");
		}
		/*if (std::is_same<T, int>::value)
			s = use_template(s, "SUM_DATATYPE", "int");
		else if (std::is_same<T, float>::value)
			s = use_template(s, "SUM_DATATYPE", "float");
		else
			throw std::runtime_error("Unsupported type in node_generator(HipaVX::LinearMask<T>* n, Type t)");*/

		if (n->use_image_datatype_for_sum)
			s = use_template(s, "SUM_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);
		else
			s = use_template(s, "SUM_DATATYPE", VX_DF_IMAGE_to_hipacc[n->sum_datatype]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/local/linear_mask.call");

		//Modify the mask
		auto mask_it = std::find_if(cs.kcv.begin(), cs.kcv.end(), [](Kernelcall_Variable* kcv)
		{
			return kcv->get_real_name() == "mask";
		});
		Kernelcall_Mask *kcm = dynamic_cast<Kernelcall_Mask*>(*mask_it);
		kcm->len_dims[0] = n->matrix.dim[0];
		kcm->len_dims[1] = n->matrix.dim[1];
		kcm->flat_mask.clear();
		for (int i = 0; i < kcm->len_dims[0]*kcm->len_dims[1]; i++)
			kcm->flat_mask.push_back(std::to_string(n->matrix.mask[i]));

		string s = kernelcall_builder(cs.kcv);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
// Explicit instantiation
template std::string node_generator<float>(HipaVX::LinearMask<float>* n, Type t);
template std::string node_generator<int>(HipaVX::LinearMask<int>* n, Type t);
template std::string node_generator<short>(HipaVX::LinearMask<short>* n, Type t);

template <typename T>
std::string node_generator(HipaVX::SimplePointScalar<T>* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/simple_scalar.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "OPERATION", n->operation);
		s = use_template(s, "SCALAR", std::to_string(n->scalar));

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/simple_scalar.call");
		string s = kernelcall_builder(cs.kcv);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
		s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
// Explicit instantiation
template std::string node_generator<float>(HipaVX::SimplePointScalar<float>* n, Type t);
template std::string node_generator<int>(HipaVX::SimplePointScalar<int>* n, Type t);
template std::string node_generator<uint>(HipaVX::SimplePointScalar<uint>* n, Type t);

template <typename T>
std::string node_generator(HipaVX::ConditionalAssignmentNode<T>* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/conditional_assigment.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		std::string condition = "";

		for(unsigned int i = 0; i < n->comparision.size(); i++)
		{
			HipaVX::ImageComparision &ic = n->comparision[i];
			if (ic.image_is_first_operand)
				condition += "input() " + ic.comp_op + " " + ic.value;
			else
				condition += ic.value + " " + ic.comp_op + " input()";

			if (i < n->chaining_operators.size())
				condition += " " + n->chaining_operators[i] + " ";
		}

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);
		s = use_template(s, "CONDITION", condition);
		s = use_template(s, "TRUE_VALUE", n->true_value);
		s = use_template(s, "FALSE_VALUE", n->false_value);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/conditional_assigment.call");
		string s = kernelcall_builder(cs.kcv);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
		s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
// Explicit instantiation
template std::string node_generator<float>(HipaVX::ConditionalAssignmentNode<float>* n, Type t);
template std::string node_generator<int>(HipaVX::ConditionalAssignmentNode<int>* n, Type t);
template std::string node_generator<u_char>(HipaVX::ConditionalAssignmentNode<u_char>* n, Type t);

string node_generator(HipaVX::SimplePoint* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/simple.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in_1->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "CONVERT_DATATYPE", "");
		s = use_template(s, "OPERATION", n->operation);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/simple.call");
		string s = kernelcall_builder(cs.kcv);

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

string node_generator(HipaVX::HarrisCorners* n, Type t)
{
	//return "";

	if (t == Type::Definition)
	{
		auto abstract_1_acc_cs = read_config_def(hipaVX_folder + "/kernels/point/abstract_1_acc.def");

		// V_c Kernel
		auto v_c = kernel_builder(abstract_1_acc_cs.kv, abstract_1_acc_cs.k, abstract_1_acc_cs.name + "_V_c");
		v_c = use_template(v_c, "KERNEL", "float out = input();\n"
										  "\t\tif (out <= @@@THRESHOLD@@@)\n"
										  "\t\t\tout = 0;\n"
										  "\t\toutput() = out;");
		v_c = use_template(v_c, "THRESHOLD", std::to_string(n->strength_thresh->f32));
		v_c = use_template(v_c, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[VX_TYPE_FLOAT32]);
		v_c = use_template(v_c, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[VX_TYPE_FLOAT32]);

		string s;
		s += v_c;

		s = use_template(s, "ID", n->my_id);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/harris_corners.call");
		string s = kernelcall_builder(cs.kcv);


		s += "\t@@@V_C_TYPE_OUT@@@ *v_c_@@@ID@@@_data = @@@IMAGE_V_C@@@.data();\n";
		s += "\tstd::vector<@@@V_C_TYPE_OUT@@@> v_c_suppressed_@@@ID@@@(@@@IMAGE_IN_WIDTH@@@ * @@@IMAGE_IN_HEIGHT@@@);\n";
		s += "\tnon_max_supression(@@@IMAGE_IN_WIDTH@@@, @@@IMAGE_IN_HEIGHT@@@, v_c_@@@ID@@@_data, v_c_suppressed_@@@ID@@@.data());\n";
		s += "\tauto features_@@@ID@@@ = euclidian_single_feature(@@@IMAGE_IN_WIDTH@@@, @@@IMAGE_IN_HEIGHT@@@, v_c_suppressed_@@@ID@@@.data(), @@@EUCLIDIAN_DISTANCE@@@);\n";
		s += "\t@@@NUM_KEYPOINTS@@@ = features_@@@ID@@@.size();\n";
		s += "\tfeatures_@@@ID@@@.resize(@@@MAX_KEYPOINTS@@@);\n";
		s += "\tstd::vector<int> cont_features_@@@ID@@@ = create_contiguous_array_from_keypoints(features_@@@ID@@@);\n";
		//s += "\t@@@CORNERS_ARRAY@@@ = cont_features_@@@ID@@@.data();\n"; //Not working due to a current bug

		s += "\tImage<int> @@@CORNERS_ARRAY@@@(7, @@@MAX_KEYPOINTS@@@, cont_features_@@@ID@@@.data());\n";

		s += "\n";





		s += "\t@@@SOBEL_TYPE_IN@@@ *input_image_@@@ID@@@_data = @@@INPUT_IMAGE@@@.data();\n";
		s += "\tstd::vector<uchar> out_data_@@@ID@@@(@@@IMAGE_IN_WIDTH@@@ * @@@IMAGE_IN_HEIGHT@@@);\n";
		s += "\tdraw_cross(@@@IMAGE_IN_WIDTH@@@, @@@IMAGE_IN_HEIGHT@@@, input_image_@@@ID@@@_data, features_@@@ID@@@, out_data_@@@ID@@@.data(), (uchar) 255);\n";
		s += "\tsave_data(@@@IMAGE_IN_WIDTH@@@, @@@IMAGE_IN_HEIGHT@@@, 1, out_data_@@@ID@@@.data(), \"akif-200x300_bw_harris.png\");\n";





		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO
		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
		s = use_template(s, "EUCLIDIAN_DISTANCE", n->min_distance->f32);

		s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);


		s = use_template(s, "V_C_TYPE_IN", VX_DF_IMAGE_to_hipacc[VX_TYPE_FLOAT32]);
		s = use_template(s, "V_C_TYPE_OUT", VX_DF_IMAGE_to_hipacc[VX_TYPE_FLOAT32]);
		s = use_template(s, "SOBEL_TYPE_IN", VX_DF_IMAGE_to_hipacc[VX_DF_IMAGE_U8]);


		s = use_template(s, "IMAGE_M_C", generate_image_name(&n->Mc));
		s = use_template(s, "IMAGE_V_C", generate_image_name(&n->Vc));


		//s = use_template(s, "NUM_KEYPOINTS", n->num_corners); // TODO
		s = use_template(s, "NUM_KEYPOINTS", "int a");
		s = use_template(s, "MAX_KEYPOINTS", n->corners->h);
		s = use_template(s, "CORNERS_ARRAY", generate_image_name(n->corners));


		return s;
	}
	return "SOMETHING IS WRONG";
}

string node_generator(HipaVX::SaturateNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/saturate.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "U8", std::to_string(n->out->col == VX_DF_IMAGE_U8));
		s = use_template(s, "S16", std::to_string(n->out->col == VX_DF_IMAGE_S16));

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/saturate.call");
		string s = kernelcall_builder(cs.kcv);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
		s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}
string node_generator(HipaVX::UnaryFunctionNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/unary_function.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);
		s = use_template(s, "FUNCTION", n->function);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/unary_function.call");
		string s = kernelcall_builder(cs.kcv);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
		s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}

std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_iterationspace(HipaVX::Image *image)
{
	std::vector<Kernelcall_Variable*> to_return_call_parameters;
	std::vector<Kernelcall_Variable*> to_return;

	Kernelcall_IterationSpace *is = new Kernelcall_IterationSpace();
	string iter_name = "iterspace_" + generate_image_name(image);
	is->set_real_name(iter_name);
	is->datatype = VX_DF_IMAGE_to_hipacc[image->col];
	is->image = generate_image_name(image);

	to_return_call_parameters.push_back(is);
	to_return.push_back(is);

	return {to_return_call_parameters,to_return};
}
std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_accessor(HipaVX::Image *image, HipaVX::VX_Matrix *mat)
{
	std::vector<Kernelcall_Variable*> to_return_call_parameters;
	std::vector<Kernelcall_Variable*> to_return;

	Kernelcall_Mask *mask = new Kernelcall_Mask;
	string mask_name = "mask_" + generate_image_name(image);
	mask->set_real_name(mask_name);
	mask->datatype = VX_DF_IMAGE_to_hipacc[mat->data_type];
	mask->len_dims[0] = mat->rows;
	mask->len_dims[1] = mat->columns;
	switch(mat->data_type)
	{
	case VX_TYPE_UINT8:
		for(size_t i = 0; i < mat->rows * mat->columns; i++)
			mask->flat_mask.emplace_back(std::to_string(((uint8_t*) mat->mat.data())[i]));
		break;
	case VX_TYPE_INT32:
		for(size_t i = 0; i < mat->rows * mat->columns; i++)
			mask->flat_mask.emplace_back(std::to_string(((int32_t*) mat->mat.data())[i]));
		break;
	case VX_TYPE_FLOAT32:
		for(size_t i = 0; i < mat->rows * mat->columns; i++)
			mask->flat_mask.emplace_back(std::to_string(((float_t*) mat->mat.data())[i]));
		break;
	}

	Kernelcall_Domain *domain = new Kernelcall_Domain();
	string domain_name = "domain_" + generate_image_name(image);
	domain->set_real_name(domain_name);
	domain->argument = mask;

	Kernelcall_BoundaryCondition_from_Dom *boundary = new Kernelcall_BoundaryCondition_from_Dom();
	string boundary_name = "boundary_" + generate_image_name(image);
	boundary->set_real_name(boundary_name);
	boundary->datatype = VX_DF_IMAGE_to_hipacc[image->col];
	boundary->image = generate_image_name(image);
	boundary->bc = "@@@BOUNDARY_CONDITION@@@";
	boundary->argument = domain;

	Kernelcall_Accessor *accessor = new Kernelcall_Accessor();
	string accessor_name = "accessor_" + generate_image_name(image);
	accessor->set_real_name(accessor_name);
	accessor->datatype = VX_DF_IMAGE_to_hipacc[image->col];
	accessor->argument = boundary;


	to_return_call_parameters.push_back(accessor);
	to_return_call_parameters.push_back(domain);
	to_return_call_parameters.push_back(mask);

	to_return.push_back(mask);
	to_return.push_back(domain);
	to_return.push_back(boundary);
	to_return.push_back(accessor);

	return {to_return_call_parameters,to_return};
}
std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_scalar(HipaVX::Scalar *scalar)
{
	static int id = 0;

	std::vector<Kernelcall_Variable*> to_return_call_parameters;
	std::vector<Kernelcall_Variable*> to_return;

	Kernelcall_General_Variable *gv = new Kernelcall_General_Variable();
	gv->set_real_name("general_variable_" + std::to_string(id++));
	gv->datatype = VX_DF_IMAGE_to_hipacc[scalar->data_type];
	switch(scalar->data_type)
	{
	case VX_TYPE_FLOAT32:
		gv->value = std::to_string(scalar->f32);
		break;
	case VX_TYPE_INT32:
		gv->value = std::to_string(scalar->i32);
		break;
	case VX_TYPE_INT8:
		gv->value = std::to_string(scalar->i8);
		break;
	default:
		throw std::runtime_error("std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_scalar(HipaVX::Scalar *scalar):\n\tNot supported scalar type");
	}

	to_return_call_parameters.push_back(gv);
	to_return.push_back(gv);

	return {to_return_call_parameters,to_return};
}
Kernelcall_Variable* generate_kernelcall(std::string kernel_name, std::vector<Kernelcall_Variable*> parameters)
{
	Kernelcall *call = new Kernelcall();
	call->set_real_name("hipacc_kernel_call");
	call->kernel_name = kernel_name;
	call->arguments = parameters;

	return call;
}

string node_generator(HipaVX::HipaccNode* n, Type t)
{
	if (t == Type::Definition)
	{
		string kernel = read_file(n->filename);

		size_t class_index = kernel.find("class");
		size_t kernelname_index = kernel.find(" ", class_index) + 1;
		size_t kernelname_end_index = kernel.find(" ", kernelname_index);

		n->kernel_name = kernel.substr(kernelname_index, kernelname_end_index - kernelname_index);
		const size_t kernel_name_length = n->kernel_name.length();
		while(true)
		{
			kernelname_index = kernel.find(n->kernel_name, kernelname_index);
			if (kernelname_index == string::npos)
				break;
			kernel.replace(kernelname_index, kernel_name_length, n->kernel_name + "_" + std::to_string(n->my_id));
			kernelname_index += kernel_name_length;
		}
		return kernel;
	}
	else if (t == Type::Call)
	{
		config_struct_call___ hipacc_call;
		std::vector<Kernelcall_Variable*> kernel_parameters;

		std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> tuple;

		tuple = generate_iterationspace(n->out);
		kernel_parameters.insert(kernel_parameters.end(), std::get<0>(tuple).begin(), std::get<0>(tuple).end());
		hipacc_call.kcv.insert(hipacc_call.kcv.end(), std::get<1>(tuple).begin(), std::get<1>(tuple).end());

		for(unsigned int i = 0; i < n->parameters.size(); i++)
		{
			switch(n->parameters[i]->type)
			{
			case VX_TYPE_SCALAR:
				tuple = generate_scalar((HipaVX::Scalar*) n->parameters[i]);
				break;
			case VX_TYPE_IMAGE:
				if (i+1 < n->parameters.size() && n->parameters[i+1]->type == VX_TYPE_MATRIX)
				{
					tuple = generate_accessor((HipaVX::Image*) n->parameters[i], (HipaVX::VX_Matrix*) n->parameters[i+1]);
					i++;
				}
				break;
			default:
				throw std::runtime_error("string node_generator(HipaVX::HipaccNode* n, Type t):\n\tUnsupported OpenVX Parameter");
			}

			kernel_parameters.insert(kernel_parameters.end(), std::get<0>(tuple).begin(), std::get<0>(tuple).end());
			hipacc_call.kcv.insert(hipacc_call.kcv.end(), std::get<1>(tuple).begin(), std::get<1>(tuple).end());
		}

		hipacc_call.kcv.push_back(generate_kernelcall(n->kernel_name, kernel_parameters));
		string s = kernelcall_builder(hipacc_call.kcv);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}

string node_generator(HipaVX::VXScaleNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/scale.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/scale.call");
		string s = kernelcall_builder(cs.kcv);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);
		s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);

		s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
		s = use_template(s, "OUTPUT_IMAGE", generate_image_name(n->out));

		s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

		return s;
	}
	return "SOMETHING IS WRONG";
}



}








namespace function_ast
{

std::string to_string(Datatype d)
{
	std::string datatype = "Unsupported Datatype";
	switch(d)
	{
	case Datatype::FLOAT:
		datatype = "float";
		break;
	case Datatype::UINT8:
		datatype = "uchar";
		break;
	case Datatype::INT16:
		datatype = "short";
		break;
	case Datatype::INT32:
		datatype = "int";
		break;
	}
	return datatype;
}

std::string generate(SimpleBinaryNode *s)
{
	std::string op;

	switch(s->type)
	{
	case NodeType::Add:
		op = "+";
		break;
	case NodeType::Sub:
		op = "-";
		break;
	case NodeType::Mul:
		op = "*";
		break;
	case NodeType::Div:
		op = "/";
		break;
	}

	auto left = s->subnodes[0]->generate_source();
	auto right = s->subnodes[1]->generate_source();

	return "(" + left + op + right + ")";
}
std::string generate(SimpleUnaryFunctionNode *s)
{
	std::string func;

	switch(s->type)
	{
	case NodeType::Sqrt:
		func = "sqrt";
		break;
	case NodeType::Exp:
		func = "exp";
		break;
	case NodeType::Conversion:
		func = "(" + to_string(dynamic_cast<Conversion*>(s)->to) + ")";
		break;
	case NodeType::Square:
	{
		auto mul = new Mul();
		mul->subnodes[0] = s->subnodes[0];
		mul->subnodes[1] = s->subnodes[0];
		return mul->generate_source();
	}break;
	}

	auto argument = s->subnodes[0]->generate_source();

	return func + "(" + argument + ")";
}
std::string generate(Variable *s)
{
	return s->name;
}
std::string generate(VariableDefinition *s)
{
	std::string datatype = to_string(dynamic_cast<Variable*>(s->subnodes[0])->datatype);
	return datatype + " " + s->subnodes[0]->generate_source();
}
std::string generate(Assignment *s)
{
	auto left = s->subnodes[0]->generate_source();
	auto right = s->subnodes[1]->generate_source();

	return left + "=" + right;
}
std::string generate(TargetPixel *s)
{
	return "output()";
}
std::string generate(Statements *s)
{
	std::string to_return;

	for(auto statement: s->statements)
	{
		to_return += statement->generate_source() + ";\n";
	}

	return to_return;
}
std::string generate(CurrentPixelvalue *s)
{
	return generate_image_name(dynamic_cast<Image*>(s->subnodes[0])->image) + "()";
}

std::string generate(Image *s)
{
	return generate_image_name(s->image);
}
std::string generate(ForEveryPixel *s)
{
	string member_variables;
	string constructor_parameters;
	string constructor_init_list;
	string add_accessor;

	string tabs = "\t";

	std::vector<Variable*> variables;

	for(auto node: s->inputs)
	{
		string name;
		string type;
		if (node->type == NodeType::Variable)
		{
			name = dynamic_cast<Variable*>(node)->generate_source();
			type = to_string(dynamic_cast<Variable*>(node)->datatype);
		}
		if (node->type == NodeType::Image)
		{
			Image *i = dynamic_cast<Image*>(node);
			name = i->generate_source();
			type = "Accessor<" + VX_DF_IMAGE_to_hipacc[i->image->col] + ">&";
		}
		if (node->type == NodeType::Stencil)
		{
			Stencil *s = dynamic_cast<Stencil*>(node);
			name = s->name;
			type = "Domain&";

			member_variables += tabs + type + " " + name + ";\n";
			constructor_parameters += ", " + type + " " + name;
			constructor_init_list += ", " + name + "(" + name + ")";

			name = name + "_mask";
			type = "Mask<" + to_string(s->datatype) + ">&";
		}

		member_variables += tabs + type + " " + name + ";\n";
		constructor_parameters += ", " + type + " " + name;
		constructor_init_list += ", " + name + "(" + name + ")";


		if (node->type == NodeType::Image)
		{
			add_accessor += tabs + '\t' + "add_accessor(&" + name + ");\n";
		}
	}

	if (s->output->type != NodeType::Image)
		throw std::runtime_error("std::string generate(ForEveryPixel *s)");

	string kernel = generate(&s->function);

	string def = read_file(hipaVX_folder + "/kernels/general_kernel_definition");
	def = use_template(def, "KERNEL_NAME", "Kernel");
	def = use_template(def, "VARIABLES", member_variables);
	def = use_template(def, "VARIABLES_CONSTRUCTOR_PARAMS", constructor_parameters);
	def = use_template(def, "VARIABLES_CONSTRUCTOR_INIT_LIST", constructor_init_list);
	def = use_template(def, "ADD_ACCESSOR", add_accessor);
	def = use_template(def, "MISC_CONSTRUCTOR", "");
	def = use_template(def, "KERNEL", kernel);
	def = use_template(def, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[dynamic_cast<Image*>(s->output)->image->col]);
	def = use_template(def, "ID", std::to_string(s->id));

	return def;
}
std::string generate(Stencil *s)
{
	return "Stencil generate todo";
}



std::string generate(ReductionAroundPixel *s)
{
	return "Reduction generate still to implement";
}
std::string generate(IterateAroundPixel *s)
{
	auto stencil = dynamic_cast<Stencil*>(s->subnodes[1]);
	std::string t = "iterate(@@@DOM_NAME@@@, [&] () -> void {\n"
			"@@@BODY@@@\n"
			"})";
	t = use_template(t, "DOM_NAME", stencil->name);
	t = use_template(t, "BODY", s->subnodes[2]->generate_source());
	return t;
}
std::string generate(PixelvalueAtCurrentStencilPos *s)
{
	auto iterate = dynamic_cast<IterateAroundPixel*>(s->subnodes[0]);
	auto image = dynamic_cast<Image*>(iterate->subnodes[0]);
	auto stencil = dynamic_cast<Stencil*>(iterate->subnodes[1]);

	return image->generate_source() + "(" + stencil->name + ")";
}
std::string generate(StencilvalueAtCurrentStencilPos *s)
{
	auto iterate = dynamic_cast<IterateAroundPixel*>(s->subnodes[0]);
	auto stencil = dynamic_cast<Stencil*>(iterate->subnodes[1]);

	return stencil->name + "_mask" + "(" + stencil->name + ")";
}














std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_accessor(HipaVX::Image *image, function_ast::Stencil *stencil)
{
	std::vector<Kernelcall_Variable*> to_return_call_parameters;
	std::vector<Kernelcall_Variable*> to_return;

	Kernelcall_Mask *mask = new Kernelcall_Mask;
	string mask_name = "mask_" + generate_image_name(image);
	mask->set_real_name(mask_name);
	mask->datatype = to_string(stencil->datatype);
	mask->len_dims[0] = stencil->dim[0];
	mask->len_dims[1] = stencil->dim[1];
	mask->flat_mask = stencil->mask;

	Kernelcall_Domain *domain = new Kernelcall_Domain();
	string domain_name = "domain_" + generate_image_name(image);
	domain->set_real_name(domain_name);
	domain->argument = mask;

	Kernelcall_BoundaryCondition_from_Dom *boundary = new Kernelcall_BoundaryCondition_from_Dom();
	string boundary_name = "boundary_" + generate_image_name(image);
	boundary->set_real_name(boundary_name);
	boundary->datatype = VX_DF_IMAGE_to_hipacc[image->col];
	boundary->image = generate_image_name(image);
	boundary->bc = "@@@BOUNDARY_CONDITION@@@";
	boundary->argument = domain;

	Kernelcall_Accessor *accessor = new Kernelcall_Accessor();
	string accessor_name = "accessor_" + generate_image_name(image);
	accessor->set_real_name(accessor_name);
	accessor->datatype = VX_DF_IMAGE_to_hipacc[image->col];
	accessor->argument = boundary;


	to_return_call_parameters.push_back(accessor);
	to_return_call_parameters.push_back(domain);
	to_return_call_parameters.push_back(mask);

	to_return.push_back(mask);
	to_return.push_back(domain);
	to_return.push_back(boundary);
	to_return.push_back(accessor);

	return {to_return_call_parameters,to_return};
}



std::string generate_call(ForEveryPixel *fep)
{
	config_struct_call___ hipacc_call;
	std::vector<Kernelcall_Variable*> kernel_parameters;

	std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> tuple;

	HipaVX::Image *image = dynamic_cast<Image*>(fep->output)->image;
	tuple = generator::generate_iterationspace(image);
	kernel_parameters.insert(kernel_parameters.end(), std::get<0>(tuple).begin(), std::get<0>(tuple).end());
	hipacc_call.kcv.insert(hipacc_call.kcv.end(), std::get<1>(tuple).begin(), std::get<1>(tuple).end());

	for(unsigned int i = 0; i < fep->inputs.size(); i++)
	{
		auto node = fep->inputs[i];
		switch(node->type)
		{
		/*case secret::NodeType::Variable:
			tuple = generator::generate_scalar((HipaVX::Scalar*) n->parameters[i]);
			break;*/
		case function_ast::NodeType::Image:
		{
			auto image = dynamic_cast<Image*>(node);
			HipaVX::Image *HVX_image = image->image;
			if (i+1 < fep->inputs.size() && fep->inputs[i+1]->type == function_ast::NodeType::Stencil)
			{
				auto stencil = dynamic_cast<Stencil*>(fep->inputs[i+1]);
				tuple = generate_accessor(HVX_image, stencil);
				i++;
			}
			break;
		}
		default:
			throw std::runtime_error("std::string generate_call(ForEveryPixel *fep):\n\tdefault in switchcase");
		}

		kernel_parameters.insert(kernel_parameters.end(), std::get<0>(tuple).begin(), std::get<0>(tuple).end());
		hipacc_call.kcv.insert(hipacc_call.kcv.end(), std::get<1>(tuple).begin(), std::get<1>(tuple).end());
	}

	hipacc_call.kcv.push_back(generator::generate_kernelcall("Kernel", kernel_parameters));
	string s = generator::kernelcall_builder(hipacc_call.kcv);

	s = use_template(s, "ID", fep->id);
	s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

	return s;
}



}




