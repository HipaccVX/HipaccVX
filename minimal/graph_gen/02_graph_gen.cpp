// export CPLUS_INCLUDE_PATH=../../
// g++ -Wall -Werror 07*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/dsl/abstractions.hpp"
#include "../../hipaVX/graph/graph.hpp"
#include "../../hipaVX/gen/hipacc_graph.hpp"


using graphVX::AppGraphT;
using graphVX::dag;
using DomVX::Image;
using DomVX::Node;
using DomVX::Scalar;
using DomVX::CustomNode;

int main() {
  dag g;
  g.gen_rand_acyclic_graph<CustomNode, Image>(12, 42);
  g.eliminate_dead_nodes();

  // g.set_order_of_exec();
  // g.print_order_of_exec();
  // g.dump_graph("graph");
  // g.dump_optimized("optimized");
  // g.print_io_nodes();

  hipacc_gen gen(g);
  gen.set_edges();

  gen.iterate_nodes();
  gen.iterate_spaces();

  gen.dump_code();

  return 0;
}
