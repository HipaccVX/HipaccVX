#include "gen_template.hpp"

string use_template(const string &template_string, const string &template_variable, const string &actual_variable)
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

string use_template(const string &template_string, const string &template_variable, const int actual_variable)
{
    return use_template(template_string, template_variable, std::to_string(actual_variable));
}

string use_template(const string &template_string, const string &template_variable, const unsigned int actual_variable)
{
    return use_template(template_string, template_variable, std::to_string(actual_variable));
}

string use_template(const string &template_string, const string &template_variable, const double actual_variable)
{
    return use_template(template_string, template_variable, std::to_string(actual_variable));
}


string read_file(const string &filename)
{
    std::ifstream ifs(filename);
    if (!ifs) std::cerr << "The file could not be opened: " << filename << std::endl;

    // Fastet way to read a whole file https://stackoverflow.com/a/116220
    return static_cast<std::stringstream const&>(std::stringstream() << ifs.rdbuf()).str();
}

void _write_to_file(const string &filename, const string &content)
{
    std::ofstream out(filename);
    out << content;
}
