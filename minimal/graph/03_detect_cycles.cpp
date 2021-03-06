// -------- Generate a random graph
// export CPLUS_INCLUDE_PATH=../../
// g++ -Wall -Werror 03*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/dsl/types.hpp"
#include "../../hipaVX/graph/graph.hpp"

using graphVX::AppGraphT;
using DomVX::Image;
using DomVX::Node;
using DomVX::Scalar;

#define PRINT_BACK_EDGES


int main() {
  graphVX::dag g;
  //g.gen_rand_graph<Node, Image>(10, 23);
  g.gen_rand_acyclic_graph<Node, Image>(10, 23);

#ifndef PRINT_BACK_EDGES
  g.detect_cycles();
#else
  g.detect_cycles_and_back_edges();
  g.print_back_edges();
#endif

  std::cout << "graph:" << std::endl;
  g.print_graph();
  g.write_graphviz("app");
  std::cout << "\n\n\n";
}
