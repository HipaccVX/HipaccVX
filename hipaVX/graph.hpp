#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>  // dfs search
#include <boost/graph/graph_utility.hpp>       // print_graph
#include <boost/graph/graphviz.hpp>
#include <boost/property_map/property_map.hpp>
#include <random>

#include "domVX_types.hpp"

#include <cstdarg>
#include <iostream>

//#include "../VX/vx_types.h"
//#include "hipaVX/domVX_types.hpp"

#pragma once

// Vertex { std::string name }
// VertexTask { std::string name }

namespace graphVX {

using VertexType = HipaVX::Object;

using HipaVX::VertexTask;

// ------------------- graphviz custom node writer ----------------------------
template <class NameMap, class TaskMap>
class vertex_writer {
 public:
  vertex_writer(NameMap _namem, TaskMap _taskm)
      : name_m(_namem), task_m(_taskm) {}

  template <class Vertex>
  void operator()(std::ostream& out, const Vertex& v) const {
    auto task = task_m[v];
    out << " [label = " << name_m[v] << ", style = filled, ";
    switch (task) {
      case VertexTask::Buffer:
        out << "shape = box, fillcolor = grey]";
        break;
      case VertexTask::Computation:
        out << "shape = circle, fillcolor = orange]";
        break;
      default:
        out << "shape = diamond, fillcolor = red]";
        ;
        break;
    }
  }

 private:
  NameMap name_m;
  TaskMap task_m;
};

template <class NameMap, class TaskMap>
inline vertex_writer<NameMap, TaskMap> make_vertex_writer(NameMap nm,
                                                          TaskMap tm) {
  return vertex_writer<NameMap, TaskMap>(nm, tm);
}

// --------------------------- wrapper class -------------------------------
//  boost::adjacency_list<
//          OutEdgeList, VertexList, Directed,
//          VertexProperties, EdgeProperties, GraphProperties, EdgeList >
//      GraphType;
//
//  Stability of Iterator Pointers:
//      In general, if you want your vertex and edge descriptors to be stable
//      (never invalidated) then use listS or setS for the VertexList and
//      OutEdgeList template parameters of adjacency_list. If you are not as
//      concerned about descriptor and iterator stability, and are more
//      concerned about memory consumption and graph traversal speed, use vecS
//      for the VertexList and/or OutEdgeList template parameters.
//  link:
//  https://www.boost.org/doc/libs/1_60_0/libs/graph/doc/adjacency_list.html

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
                              VertexType>
    AppGraphT;

typedef boost::adjacency_list<> _GraphT;

typedef _GraphT::vertex_descriptor VertexDesc;

typedef _GraphT::edge_descriptor EdgeDesc;

template <class GraphT>
class dag {
 private:
  GraphT g;

 public:
  GraphT get_graph() { return g; }

  // filling out the graph
  template <class Vertex>
  VertexDesc add_vertex(Vertex v);

  VertexDesc get_vertex(int n);

  VertexDesc get_vertex_object(int n);

  // TODO: we can do the error checking and return edge_descriptor
  std::pair<EdgeDesc, bool> add_edge(VertexDesc src, VertexDesc dst);


  // printing the graph
  void print_graph();

  void write_graphviz(std::string filename = "out");
};



// ------------------------- filling out the graph -----------------------------
template <class GraphT>
template <class Vertex>
VertexDesc dag<GraphT>::add_vertex(Vertex v) {
    return boost::add_vertex(v, g);
}

template <class GraphT>
VertexDesc dag<GraphT>::get_vertex(int n) {
  return boost::vertex(n, g);
}

template <class GraphT>
VertexDesc dag<GraphT>::get_vertex_object(int n) {
  auto v = get_vertex(n);
  return g[v];
}

template <class GraphT>
std::pair<EdgeDesc, bool> dag<GraphT>::add_edge(VertexDesc src, VertexDesc dst) {
  return boost::add_edge(src, dst, g);
}


// ------------------ class methods for printing -------------------------------
template <class GraphT>
void dag<GraphT>::print_graph() {
  boost::print_graph(g, get(&VertexType::name, g));
}

template <class GraphT>
void dag<GraphT>::write_graphviz(std::string filename) {
  std::ofstream file_out(filename + ".dot");
  boost::write_graphviz(file_out, g,
                        make_vertex_writer(boost::get(&VertexType::name, g),
                                           boost::get(&VertexType::task, g)));
}

}  // namespace graphVX
