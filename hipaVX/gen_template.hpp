#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#pragma once

std::string use_template(const std::string &template_string,
					const std::string &template_variable,
					const std::string &actual_variable);

std::string use_template(const std::string &template_string,
					const std::string &template_variable, const int actual_variable);

std::string use_template(const std::string &template_string,
					const std::string &template_variable,
                    const unsigned int actual_variable);

std::string use_template(const std::string &template_string,
					const std::string &template_variable,
                    const double actual_variable);

std::string read_file(const std::string &filename);

void _write_to_file(const std::string &filename, const std::string &content);
