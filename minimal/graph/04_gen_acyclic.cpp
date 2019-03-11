// -------- Generate a random graph
// export CPLUS_INCLUDE_PATH=../../
// g++ -Wall -Werror 04*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/graph.hpp"

using graphVX::AppGraphT;
using graphVX::dag;
using HipaVX::Image;
using HipaVX::Node;
using HipaVX::Scalar;

dag<AppGraphT>* generate_acyclic_graph() {
  dag<AppGraphT>* g_ptr;

  bool has_cycle = true;
  while (has_cycle) {
    std::cout << "Create a new graph" << std::endl;
    g_ptr = new dag<AppGraphT>();
    g_ptr->gen_rand_graph<Node, Image>(10, 23);
    has_cycle = g_ptr->detect_cycles();
    std::cout << std::endl;
  }

  return g_ptr;
}


int main() {
  auto g_ptr = generate_acyclic_graph();
  dag<AppGraphT> g = *g_ptr;

  std::cout << "graph:" << std::endl;
  g.print_graph();
  g.write_graphviz("app");
  std::cout << "\n\n\n";
}
