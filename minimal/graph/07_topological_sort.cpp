// export CPLUS_INCLUDE_PATH=../../
// g++ -Wall -Werror 07*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/graph.hpp"


using graphVX::AppGraphT;
using graphVX::dag;
using HipaVX::Image;
using HipaVX::Node;
using HipaVX::Scalar;

int main() {
  dag g;
  g.gen_rand_acyclic_graph<Node, Image>(12, 42);
  g.eliminate_dead_nodes();

  g.set_order_of_exec();
  g.print_order_of_exec();

  // auto _g_opt = g._g_opt;
  // auto ordered = graphVX::_topological_sort(*_g_opt);
  // graphVX::_print_list(*ordered, *_g_opt, "order of execution: ");

  g.dump_graph("graph");
  g.dump_optimized("optimized");
  g.print_io_nodes();
  return 0;
}
