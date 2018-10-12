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
	{VX_TYPE_FLOAT32, "float"} //Not really a vx_df_image type
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

void process_graph(vx_graph graph)
{
	string main = read_file(hipaVX_folder + "/hipacc_main.template");

	string images;
	const string image_decl_template = "\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@);\n";

	for(const auto image: graph->used_images)
	{
		string image_name = generate_image_name(image);

		string temp = image_decl_template;

		// BAD HACKs
		const HipaVX::FileinputImage* fim = dynamic_cast<const HipaVX::FileinputImage*>(image);
		const HipaVX::Array* arr = dynamic_cast<const HipaVX::Array*>(image);
		if(fim)
		{
			const string fim_decl_template = "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = load_data<@@@DATATYPE@@@>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, 1, \"@@@FILENAME@@@\");\n"
											 "\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
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
		s       += "\tsave_data(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_CHANNELS@@@, data_@@@IMAGE@@@_@@@ID@@@, \"@@@FILENAME@@@\");\n";
		s       += "\n";


		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "IMAGE", generate_image_name(n->in));
		s = use_template(s, "IMAGE_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);

		s = use_template(s, "IMAGE_WIDTH", n->in->w);
		s = use_template(s, "IMAGE_HEIGHT", n->in->h);
		s = use_template(s, "IMAGE_CHANNELS", 1); //TODO
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
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/and.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/and.call");
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
		s = use_template(s, "SUM_OPERATION", " * " + std::to_string(n->normalization));
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
		kcm->len_dims[0] = n->dim[0];
		kcm->len_dims[1] = n->dim[1];
		kcm->flat_mask.clear();
		for (int i = 0; i < kcm->len_dims[0]*kcm->len_dims[1]; i++)
			kcm->flat_mask.push_back(std::to_string(n->mask[i]));

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
		s += "\tsave_data(@@@IMAGE_IN_WIDTH@@@, @@@IMAGE_IN_HEIGHT@@@, 1, out_data_@@@ID@@@.data(), \"Please just work.png\");\n";





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
string node_generator(HipaVX::SqrtNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/sqrt.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/sqrt.call");
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
string node_generator(HipaVX::AbsNode* n, Type t)
{
	if (t == Type::Definition)
	{
		auto cs = read_config_def(hipaVX_folder + "/kernels/point/abs.def");

		string s = kernel_builder(cs.kv, cs.k, cs.name);

		s = use_template(s, "ID", n->my_id);
		s = use_template(s, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);
		s = use_template(s, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[n->out->col]);

		return s;
	}
	else if (t == Type::Call)
	{
		auto cs = read_config_call(hipaVX_folder + "/kernels/point/abs.call");
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


}
