#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#pragma once

using std::string;

string use_template(const string &template_string,
                    const string &template_variable,
                    const string &actual_variable);

string use_template(const string &template_string,
                    const string &template_variable, const int actual_variable);

string use_template(const string &template_string,
                    const string &template_variable,
                    const unsigned int actual_variable);

string use_template(const string &template_string,
                    const string &template_variable,
                    const double actual_variable);

string read_file(const string &filename);

void _write_to_file(const string &filename, const string &content);
