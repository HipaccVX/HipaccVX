// export CPLUS_INCLUDE_PATH=../../
// g_dag++ -Wall -Werror 07*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/graph.hpp"
#include "../../hipaVX/graph_gen/cpp_gen.hpp"


using graphVX::AppGraphT;
using graphVX::OptGraphT;
using graphVX::dag;
using HipaVX::Image;
using HipaVX::Node;
using HipaVX::Scalar;


int main() {
  hipacc_writer gen;
  std::cout << gen.dump_code();

  HipaVX::Image c; 
  c.set_dtype(VX_DF_IMAGE_U8);

  gen.def(&c);


  std::cout << gen.ss_im.str();
  return 0;
}
