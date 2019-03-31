// export CPLUS_INCLUDE_PATH=../../
// g_dag++ -Wall -Werror 07*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/graph.hpp"
#include "../../hipaVX/graph_gen/hipacc_graph_gen.hpp"


using graphVX::AppGraphT;
using graphVX::OptGraphT;
using graphVX::dag;
using HipaVX::Image;
using HipaVX::Node;
using HipaVX::Scalar;


int main() {
  hipacc_writer gen;
  std::cout << gen.dump_code();

  // image
  HipaVX::Image im(1024, 1024, VX_DF_IMAGE_U8);
  im.set_dtype(VX_DF_IMAGE_U32);

  // accessor
  //HipaccAccessor acc;
  HipaccAccessor acc(&im);

  std::stringstream ss;
  gen.def(ss, &im);
  gen.def(ss, &acc);

  std::cout << ss.str();
  // Or
  // gen.def(&im);
  // std::cout << gen.ss_im.str();
  return 0;
}
