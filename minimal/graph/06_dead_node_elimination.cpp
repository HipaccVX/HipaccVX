// export CPLUS_INCLUDE_PATH=../../
// g++ -Wall -Werror 06*.cpp ../../hipaVX/domVX_types.cpp

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
    g_ptr->gen_rand_acyclic_graph<Node, Image>(12, 42);
    has_cycle = g_ptr->detect_cycles();
    std::cout << std::endl;
  }

  return g_ptr;
}

dag* eliminate_dead_nodes() {
  auto g_ptr = generate_acyclic_graph();
  g_ptr->eliminate_dead_nodes();

  return g_ptr;
}

dag* init_ios_and_eliminate_dead_nodes() {
  // create a random graph and clear io_node vectors
  auto g_ptr = generate_acyclic_graph();
  g_ptr->inputs.clear();
  g_ptr->outputs.clear();

  // find input-output nodes
  g_ptr->set_io_nodes();
  g_ptr->eliminate_dead_nodes();

  return g_ptr;
}

int main() {
  //auto g_ptr = eliminate_dead_nodes();
  auto g_ptr = init_ios_and_eliminate_dead_nodes();

  g_ptr->dump_graph("graph");
  g_ptr->dump_reversed("reversed");
  g_ptr->dump_optimized("optimized");
  g_ptr->print_io_nodes();
}
