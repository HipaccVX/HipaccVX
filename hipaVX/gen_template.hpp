#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#pragma once


// TODO: these functions were static before ???

using std::string;

string use_template(const string &template_string, const string &template_variable, const string &actual_variable);

string use_template(const string &template_string, const string &template_variable, const int actual_variable);

string use_template(const string &template_string, const string &template_variable, const unsigned int actual_variable);

string use_template(const string &template_string, const string &template_variable, const double actual_variable);

string read_file(const string &filename);