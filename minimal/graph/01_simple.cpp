// -------- generate a new random graph untill there are no cycles
// export CPLUS_INCLUDE_PATH=../../
// g++ -Wall -Werror 01_simple.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/graph.hpp"

using graphVX::AppGraphT;
using HipaVX::Image;
using HipaVX::Node;
using HipaVX::Scalar;


Node kernelx("kernX"), kernely("kernY"), kernelz("kernZ"), kernelt("kernT");
Image im0("im"), im1("im"), im2("im"), im3("im"), var0("var0");

template <class Graph_t>
void ex_graph(Graph_t &g) {
  auto im0_v = g.add_vertex(im0);
  auto kernx_v = g.add_vertex(kernelx);
  auto im1_v = g.add_vertex(im1);
  auto kerny_v = g.add_vertex(kernely);
  auto im2_v = g.add_vertex(im2);
  auto kernz_v = g.add_vertex(kernelz);
  auto kernt_v = g.add_vertex(kernelt);
  auto im3_v = g.add_vertex(im3);
  auto var0_v = g.add_vertex(var0);

  g.add_edge(im0_v, kernx_v);
  g.add_edge(kernx_v, im1_v);
  g.add_edge(im1_v, kerny_v);
  g.add_edge(im1_v, kernz_v);
  g.add_edge(kerny_v, im2_v);
  g.add_edge(kernz_v, im3_v);
  g.add_edge(im3_v, kernt_v);
  g.add_edge(im2_v, kernt_v);
  g.add_edge(var0_v, kernt_v);
}


int main() {
  graphVX::dag<AppGraphT> g;
  ex_graph(g);

  std::cout << "graph:" << std::endl;
  g.print_graph();
  g.write_graphviz("app");
  std::cout << "\n\n\n";
}
