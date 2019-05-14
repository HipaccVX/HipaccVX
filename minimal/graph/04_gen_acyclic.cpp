// -------- Generate a random graph
// export CPLUS_INCLUDE_PATH=../../
// g++ -Wall -Werror 04*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/dsl/types.hpp"
#include "../../hipaVX/graph/graph.hpp"

using graphVX::AppGraphT;
using graphVX::dag;
using DomVX::Image;
using DomVX::Node;
using DomVX::Scalar;


dag* generate_acyclic_graph() {
  dag* g_ptr;

  bool has_cycle = true;
  while (has_cycle) {
    std::cout << "Create a new graph" << std::endl;
    g_ptr = new dag();
    g_ptr->gen_rand_graph<Node, Image>(10, 23);
    has_cycle = g_ptr->detect_cycles();
    std::cout << std::endl;
  }

  return g_ptr;
}


int main() {
  auto g_ptr = generate_acyclic_graph();
  dag g = *g_ptr;

  std::cout << "graph:" << std::endl;
  g.print_graph();
  g.write_graphviz("app");
  std::cout << "\n\n\n";
}
