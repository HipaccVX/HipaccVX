// export CPLUS_INCLUDE_PATH=../../
// g++ -Wall -Werror 07*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/graph.hpp"
#include "../../hipaVX/graph_gen/hipacc_graph_gen.hpp"


using graphVX::AppGraphT;
using graphVX::dag;
using HipaVX::Image;
using HipaVX::Node;
using HipaVX::Scalar;

int main() {
  dag g;
  g.gen_rand_acyclic_graph<Node, Image>(12, 42);
  g.eliminate_dead_nodes();

  // g.set_order_of_exec();
  // g.print_order_of_exec();
  // g.dump_graph("graph");
  // g.dump_optimized("optimized");
  // g.print_io_nodes();

  graph_gen gen(g);

  std::cout << "\nnodes according to the order of execution\n";
  gen.print_nodes();

  std::cout << "\nspaces according to the order of execution\n";
  gen.print_spaces();
  
  return 0;
}
