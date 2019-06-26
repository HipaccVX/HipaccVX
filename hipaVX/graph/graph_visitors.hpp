#pragma once
#include <boost/graph/depth_first_search.hpp>  // dfs search
#include <boost/graph/graphviz.hpp>
#include "../dsl/types.hpp"

namespace graphVX {

using DomVX::ObjectTask;  // graphviz

//------------------------------------------------------------------
//                   Cycle Detection
//------------------------------------------------------------------
struct cycle_detector_dfs : public boost::dfs_visitor<> {
  cycle_detector_dfs(bool& has_cycle) : m_has_cycle(has_cycle) {
    has_cycle = false;
  }

  template <class Edge, class Graph>
  void back_edge(Edge, Graph&) {
    m_has_cycle = true;
  }

 protected:
  bool& m_has_cycle;
};

// backedges are stored in a vector
template <class EdgeDesc>
struct cycle_detector_with_backedges_dfs : public boost::dfs_visitor<> {
  cycle_detector_with_backedges_dfs(bool& has_cycle_,
                                    std::vector<EdgeDesc>& back_edges_)
      : has_cycle(has_cycle_), back_edges(back_edges_) {
    has_cycle = false;
  }

  template <class Edge, class Graph>
  void back_edge(Edge e, Graph&) {
    has_cycle = true;
    back_edges.push_back(e);
  }

 protected:
  bool& has_cycle;
  std::vector<EdgeDesc>& back_edges;
};

template <class EdgeDesc>
inline cycle_detector_with_backedges_dfs<EdgeDesc> make_cycle_dbe(EdgeDesc e) {
  return cycle_detector_with_backedges_dfs<EdgeDesc>(e);
}

//------------------------------------------------------------------
//            Initialize all the graph nodes
//------------------------------------------------------------------
// dfs visitor for initialization: traverses all the nodes
template <class GraphType>
struct mark_as_alive : public boost::dfs_visitor<> {
  mark_as_alive() {}
  mark_as_alive(GraphType& g_) : g(g_) {}

  template <class Vertex, class Graph>
  void discover_vertex(Vertex v, Graph&) {
    g[v].is_alive(true);
  };

  //  template <class Vertex, class Graph>
  //  void start_vertex(Vertex v, Graph&) { g[v].alive = true; }
  //
  //  template <class Edge, class Graph>
  //  void tree_edge(Edge e, Graph&) { g[target(e, g)].alive = true; }

 private:
  GraphType& g;
};

//------------------------------------------------------------------
//            Graphviz .dot File Generation
//------------------------------------------------------------------
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
      case ObjectTask::Buffer:
        out << "shape = box, fillcolor = grey]";
        break;
      case ObjectTask::Computation:
        out << "shape = circle, fillcolor = orange]";
        break;
      default:
        out << "shape = diamond, fillcolor = red]";
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

}  // namespace graphVX
