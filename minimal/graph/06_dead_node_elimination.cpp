// export CPLUS_INCLUDE_PATH=../../
// g++ -Wall -Werror 06*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/graph.hpp"


using graphVX::AppGraphT;
using graphVX::dag;
using HipaVX::Image;
using HipaVX::Node;
using HipaVX::Scalar;

dag* generate_acyclic_graph() {
  dag* g_ptr;

  bool has_cycle = true;
  while (has_cycle) {
    std::cout << "Create a new graph" << std::endl;
    g_ptr = new dag();
    g_ptr->gen_rand_graph<Node, Image>(12, 42);
    has_cycle = g_ptr->detect_cycles();
    std::cout << std::endl;
  }

  return g_ptr;
}

int main() {
  auto g_ptr = generate_acyclic_graph();

  g_ptr->eliminate_dead_nodes();

  g_ptr->dump_graph("graph");
  g_ptr->dump_reversed("reversed");
  g_ptr->dump_optimized("optimized");
  g_ptr->print_io_nodes();
}
