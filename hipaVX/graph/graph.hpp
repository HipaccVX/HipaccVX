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

#pragma once
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/transpose_graph.hpp>
#include <boost/property_map/property_map.hpp>

#include <algorithm>
#include <iterator>
#include <random>

#include "../dsl/types.hpp"
#include "graph_utils.hpp"
#include "graph_visitors.hpp"

#include <cstdarg>
#include <iostream>

// Vertex { std::string name, VertexTask task}

namespace graphVX {

using VertexType = DomVX::Object;
using EdgeType = DomVX::Acc;
using DomVX::ObjectTask;

typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS,
                              VertexType, EdgeType>
    AppGraphT;

// typedef boost::adjacency_list<> _GraphT;

// typedef _GraphT::vertex_descriptor VertexDesc;
//
// typedef _GraphT::edge_descriptor EdgeDesc;

typedef boost::adjacency_list<AppGraphT> _GraphT;

typedef boost::graph_traits<AppGraphT>::vertex_descriptor VertexDesc;

typedef boost::graph_traits<AppGraphT>::edge_descriptor EdgeDesc;

struct terminator {
  std::vector<VertexDesc> dest;

  terminator(std::vector<VertexDesc>& _dest) { dest = _dest; }

  template <class Vertex, class Graph>
  bool operator()(const Vertex& v, const Graph& g) {
    if (boost::out_degree(v, g) == 0) return true;

    // this loop should be unnecessary for an OpenVX graph
    for (auto v_dest : dest) {
      if (v == v_dest) return true;
    }

    return false;
  }
};

using VertPredicate = std::function<bool(VertexDesc)>;

using OptGraphT =
    boost::filtered_graph<AppGraphT, boost::keep_all, VertPredicate>;

typedef boost::graph_traits<OptGraphT>::out_edge_iterator OptGraphOutEdgeIter;
typedef boost::graph_traits<OptGraphT>::in_edge_iterator OptGraphInEdgeIter;
typedef boost::graph_traits<OptGraphT>::adjacency_iterator OptGraphAdjIter;

typedef std::list<VertexDesc> OrderedList;

class dag {
 public:
  AppGraphT g, g_trans;

  OptGraphT* _g_opt;

  std::vector<VertexDesc> inputs, outputs;

  OrderedList* order_of_exec;

 public:
  AppGraphT get_graph() { return g; }

  AppGraphT get_reversed_graph() { return g_trans; }

  // filling out the graph
  template <class Vertex>
  VertexDesc add_vertex(Vertex v);

  VertexDesc get_vertex(int n);

  VertexType get_vertex_object(int n);

  std::pair<EdgeDesc, bool> add_edge(VertexDesc src, VertexDesc dst);

  // printing the graph
  void print_io_nodes();

  void print_graph();

  void write_graphviz(std::string filename = "graph");

  void dump_graph(std::string name = "graph");

  void dump_reversed(std::string name = "reversed");

  void dump_optimized(std::string name = "optimized");

  void print_order_of_exec();

  // detecting cycles
  bool detect_cycles();

  bool detect_cycles_and_back_edges();

  bool has_cycle();

  void print_back_edges();

  // reverse graph and dead node elimination
  AppGraphT* reverse();

  OptGraphT* eliminate_dead_nodes();
  OptGraphT* dont_eliminate_dead_nodes(); // temporary

  // topological sort for order of execution
  OrderedList* set_order_of_exec();

  OrderedList* get_order_of_exec() { return order_of_exec; }

  // random graphs for testing
  template <class Node, class Image>
  void gen_rand_graph(unsigned nvertex, unsigned nedges,
                      unsigned num_inputs = 1, unsigned num_outputs = 2);

  template <class Node, class Image>
  void gen_rand_acyclic_graph(unsigned nvertex, unsigned nedges,
                              unsigned num_inputs = 1,
                              unsigned num_outputs = 2);

 private:
  bool cycle_exist = false;

  std::vector<EdgeDesc> back_edges;
};

//*********************************************************************
// Template methods
//*********************************************************************
template <class Vertex>
VertexDesc dag::add_vertex(Vertex v) {
  return boost::add_vertex(v, g);
}

template <class Node, class Image>
void dag::gen_rand_graph(unsigned n, unsigned k, unsigned n_in,
                         unsigned n_out) {
  graphVX::_gen_rand_graph<Node, Image>(n, k, g, inputs, outputs, n_in, n_out);
}
template <class Node, class Image>
void dag::gen_rand_acyclic_graph(unsigned n, unsigned k, unsigned n_in,
                                 unsigned n_out) {
  graphVX::_gen_rand_acyclic_graph<Node, Image>(n, k, g, inputs, outputs, n_in,
                                                n_out);
  // print_io_nodes();
}

}  // namespace graphVX

namespace DomVX {

class Graph : public Object {
 public:
  std::vector<Node*> graph;

  std::unique_ptr<graphVX::dag> dag;
  std::map<DomVX::Node*, graphVX::VertexDesc> refs;

  bool built = false;

  Context* context;

  void build() {
    for (auto& node : graph) {
      node->build();
    }

    built = true;
  }
};
};  // namespace DomVX
