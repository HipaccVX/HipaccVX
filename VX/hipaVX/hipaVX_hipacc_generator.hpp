#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <map>


#include "hipaVX_internal.hpp"
#include "config_reader.hpp"

static std::string hipaVX_folder = "VX/hipaVX";

using std::string;

string read_file(const string& filename);

string use_template(const string& template_string, const string& template_variable, const string& actual_variable);
string use_template(const string& template_string, const string& template_variable, const int actual_variable);
string use_template(const string& template_string, const string& template_variable, const unsigned int actual_variable);

void write_file(const string& filename, const string& content);

static std::map<vx_df_image, string> VX_DF_IMAGE_to_hipacc = {
	{VX_DF_IMAGE_U8, "uchar"},
	{VX_DF_IMAGE_S16, "short"},
};

std::string generate_image_name(vx_image image);

void process_graph(vx_graph graph);



















