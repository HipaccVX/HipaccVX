#include "gen_template.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

using std::string;

string read_file(const string &filename) {
  std::ifstream ifs(filename);
  if (!ifs)
    std::cerr << "The file could not be opened: " << filename << std::endl;

  // Fastet way to read a whole file https://stackoverflow.com/a/116220
  return static_cast<std::stringstream const &>(std::stringstream() << ifs.rdbuf()).str();
}
