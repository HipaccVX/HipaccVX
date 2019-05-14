// -------- Generate a random graph
// export CPLUS_INCLUDE_PATH=../../
// g++ -Wall -Werror 02*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/dsl/types.hpp"
#include "../../hipaVX/graph/graph.hpp"

using graphVX::AppGraphT;
using DomVX::Image;
using DomVX::Node;
using DomVX::Scalar;

int main() {
  graphVX::dag g;

  // Number of vertices and edges
  g.gen_rand_graph<Node, Image>(10, 23);

  std::cout << "graph:" << std::endl;
  g.print_graph();
  g.write_graphviz("app");
  std::cout << "\n";
}
