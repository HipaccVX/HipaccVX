#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <map>


#include "hipaVX_internal.hpp"

static std::string hipaVX_folder = "VX/hipaVX";

using std::string;

string read_file(const string& filename)
{
	std::ifstream in(filename);

	// Fastet way to read a whole file https://stackoverflow.com/a/116220
	return static_cast<std::stringstream const&>(std::stringstream() << in.rdbuf()).str();
}

string use_template(const string& template_string, const string& template_variable, const string& actual_variable)
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
string use_template(const string& template_string, const string& template_variable, const int actual_variable)
{
	return use_template(template_string, template_variable, std::to_string(actual_variable));
}
string use_template(const string& template_string, const string& template_variable, const unsigned int actual_variable)
{
	return use_template(template_string, template_variable, std::to_string(actual_variable));
}

void write_file(const string& filename, const string& content)
{
	std::ofstream out(filename);
	out << content;
}

static std::map<vx_df_image, string> VX_DF_IMAGE_to_hipacc = {
	{VX_DF_IMAGE_U8, "uchar"},
	{VX_DF_IMAGE_S16, "short"},
};

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
std::string node_generator(HipaVX::ReadImageNode* n, Type t)
{
	return "\tint TODO_ReadImageNode;\n";
}

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
}
std::string node_generator(HipaVX::Sobel3x3Node* n, Type t)
{
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
}
}














