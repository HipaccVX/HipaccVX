#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>  // dfs search
#include <boost/graph/transpose_graph.hpp>
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

// Vertex { std::string name, VertexTask task}

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


// ---------------------- dfs visitors for cycle detection ----------------------
struct cycle_detector_dfs : public boost::dfs_visitor<>
{
  cycle_detector_dfs(bool& has_cycle) : m_has_cycle(has_cycle) { }

  template <class Edge, class Graph>
  void back_edge(Edge, Graph&) { m_has_cycle = true; }
protected:
  bool& m_has_cycle;
};


template<class EdgeDesc>
struct cycle_detector_with_backedges_dfs : public boost::dfs_visitor<> {
    cycle_detector_with_backedges_dfs(bool& has_cycle_, std::vector<EdgeDesc>& back_edges_) :
        has_cycle(has_cycle_), back_edges(back_edges_) { }

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


// -------------- dfs visitors for dead node elimination ----------------------
template<class GraphType>
struct mark_as_alive: public boost::dfs_visitor<>
{
  mark_as_alive() { }
  mark_as_alive(GraphType& g_) : g(g_){ }

  template <class Vertex, class Graph>
  void discover_vertex(Vertex v, Graph&) { g[v].alive = true; };

//    template <class Vertex, class Graph>
//    void start_vertex(Vertex v, Graph&) { g[v].alive = true; }

 private:
  GraphType& g;
};

struct terminator {
    template<class Vertex, class Graph>
    bool operator()(const Vertex& v, const Graph& g) {
        return v == boost::vertex(0, g);
    }
};



// --------------------- independent functions -------------------------------
template <class GraphT>
void _print_graph(GraphT _g) {
  boost::print_graph(_g, get(&VertexType::name, _g));
}

template <class GraphT>
void _write_graphviz(GraphT _g, std::string name) {
  std::ofstream file_out(name + ".dot");
  boost::write_graphviz(file_out, _g,
                        make_vertex_writer(boost::get(&VertexType::name, _g),
                                           boost::get(&VertexType::task, _g)));
}

template <class GraphT>
void _dump_graph(GraphT _g, std::string _name) {
  std::cout << _name << std::endl;
  graphVX::_print_graph(_g);
  graphVX::_write_graphviz(_g, _name);
  std::cout << "\n";
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
 public:
  GraphT g, g_trans;

  GraphT get_graph() { return g; }

  GraphT get_reversed_graph() { return g_trans; }

  // filling out the graph
  template <class Vertex>
  VertexDesc add_vertex(Vertex v);

  VertexDesc get_vertex(int n);

  VertexDesc get_vertex_object(int n);

  std::pair<EdgeDesc, bool> add_edge(VertexDesc src, VertexDesc dst);

  // printing the graph
  void print_graph();

  void write_graphviz(std::string filename = "graph");

  void dump_graph(std::string name = "graph");

  void dump_reversed(std::string name = "reversed");

  // detecting cycles
  bool detect_cycles();

  bool detect_cycles_and_back_edges();

  bool has_cycle();

  void print_back_edges();

  // reverse graph
  GraphT* reverse();

  // random graphs for testing
  template <class Node, class Image>
  void gen_rand_graph(unsigned nvertex, unsigned nedges);

  private:
    bool cycle_exist = false;

    std::vector<EdgeDesc> back_edges;
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
  // TODO: we can do the error checking and return edge_descriptor
  return boost::add_edge(src, dst, g);
}


// ------------------ class methods for printing -------------------------------
template <class GraphT>
void dag<GraphT>::print_graph() {
  graphVX::_print_graph(g);
}

template <class GraphT>
void dag<GraphT>::write_graphviz(std::string _name) {
  graphVX::_write_graphviz(g, _name);
}

template <class GraphT>
void dag<GraphT>::dump_graph(std::string _name) {
  graphVX::_dump_graph(g, _name);
}

template <class GraphT>
void dag<GraphT>::dump_reversed(std::string _name) {
  graphVX::_dump_graph(g_trans, _name);
}

// ------------------ class methods for detecting cycles -----------------------
template <class GraphT>
bool dag<GraphT>::detect_cycles() {
    cycle_detector_dfs vis(cycle_exist);
    depth_first_search(g, visitor(vis));

    if (cycle_exist) {
        std::cout << "The graph has a cycle " << std::endl;
    }

    return cycle_exist;
}


template <class GraphT>
bool dag<GraphT>::detect_cycles_and_back_edges() {
    cycle_detector_with_backedges_dfs<EdgeDesc> vis(cycle_exist, back_edges);
    depth_first_search(g, visitor(vis));
    return cycle_exist;
}

template <class GraphT>
bool dag<GraphT>::has_cycle() { return cycle_exist; }

template <class GraphT>
void dag<GraphT>::print_back_edges() {
    if (cycle_exist) {
        std::cout << "Edges at the cycles" << std::endl;
        for(auto it = begin(back_edges); it != end(back_edges); it++) {
           std::cout << g[source(*it, g)].get_name() << " --> "
                     << g[target(*it, g)].get_name() << std::endl;
        }
        std::cout << "\n";
    }
}


// ------------------- random graph generation ---------------------------------
template <class GraphT>
template <class Node, class Image>
void dag<GraphT>::gen_rand_graph(unsigned n, unsigned k) {
    std::random_device rd;
    std::mt19937 gen(rd());
     std::uniform_int_distribution<> dis(0, n - 1);

    VertexDesc images[n];
    VertexDesc nodes[n];

    for (unsigned v = 0; v < n; v++ ) {
        auto new_node  = new Node();
        auto new_image = new Image();
        nodes[v] = add_vertex(*new_node);
        images[v] = add_vertex(*new_image);
    }

    // first and last images are not virtual
    g[images[0]].virt = false;
    g[images[n-1]].virt = false;
    add_edge(images[0], nodes[dis(gen)]);
    add_edge(nodes[dis(gen)], images[n-1]);

    for (unsigned i = 0; i < k; i++ ) {
        unsigned u = dis(gen); //rand() % n;
        unsigned v = dis(gen); //rand() % n;
        if (i % 2)
            add_edge(images[u], nodes[v]);
        else
            add_edge(nodes[v], images[v]);
    }
}


// ------------------ class methods for detecting cycles -----------------------
template <class GraphT>
GraphT* dag<GraphT>::reverse() {
  boost::transpose_graph(g, g_trans);
  return &g_trans;
}

}  // namespace graphVX
