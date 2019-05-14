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


typedef boost::adjacency_list<boost::listS,boost::vecS,
          boost::bidirectionalS, VertexType, EdgeType> AppGraphT;

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

class dag {
  using OrderedList = std::list<VertexDesc>;

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

  // topological sort for order of execution
  OrderedList* set_order_of_exec() {
     order_of_exec = new OrderedList;
    _topological_sort(order_of_exec, *_g_opt);
    return order_of_exec;
  };

  OrderedList* get_order_of_exec() { return order_of_exec; };

  // random graphs for testing
  template <class Node, class Image>
  void gen_rand_graph(unsigned nvertex, unsigned nedges);

  template <class Node, class Image>
  void gen_rand_acyclic_graph(unsigned nvertex, unsigned nedges);

 private:
  bool cycle_exist = false;

  std::vector<EdgeDesc> back_edges;
};


// template methods
template <class Vertex>
VertexDesc dag::add_vertex(Vertex v) {
  return boost::add_vertex(v, g);
}

// ------------------- random graph generation ---------------------------------
template <class Node, class Image>
void dag::gen_rand_graph(unsigned n, unsigned k) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, n - 1);
  std::uniform_int_distribution<> dis3(1, 3);

  VertexDesc images[n];
  VertexDesc nodes[n];

  for (unsigned v = 0; v < n; v++) {
    auto new_node = new Node();
    auto new_image = new Image();
    nodes[v] = add_vertex(*new_node);
    images[v] = add_vertex(*new_image);
  }

  // input and output images are not virtual
  int n_in = 1;   // dis3(gen);
  int n_out = 2;  // dis3(gen);

  for (int i = 0; i < n_in; i++) {
    auto v = images[i];
    g[v].is_virtual(false);
    inputs.push_back(v);
    add_edge(v, nodes[dis(gen)]);
  }
  for (int i = 0; i < n_out; i++) {
    auto v = images[n - 1 - i];
    g[v].is_virtual(false);
    outputs.push_back(v);
    add_edge(nodes[dis(gen)], v);
  }

  for (unsigned i = 0; i < k; i++) {
    unsigned u = dis(gen);  // rand() % n;
    unsigned v = dis(gen);  // rand() % n;
    if (i % 2)
      add_edge(images[u], nodes[v]);
    else
      add_edge(nodes[v], images[v]);
  }
}

template <class Node, class Image>
void dag::gen_rand_acyclic_graph(unsigned n, unsigned k) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, n - 1);
  std::uniform_int_distribution<> dis3(1, 3);

  VertexDesc images[n];
  VertexDesc nodes[n];

  // Create a random graph
  for (unsigned v = 0; v < n; v++) {
    auto new_node = new Node();
    auto new_image = new Image(1024, 1024, VX_DF_IMAGE_U8);
    nodes[v] = add_vertex(*new_node);
    images[v] = add_vertex(*new_image);
  }

  for (unsigned i = 0; i < k; i++) {
    unsigned u = dis(gen);  // rand() % n;
    unsigned v = dis(gen);  // rand() % n;
    if (i % 2)
      add_edge(images[u], nodes[v]);
    else
      add_edge(nodes[v], images[v]);
  }

  // cleanup cycles
  detect_cycles_and_back_edges();
  while (cycle_exist) {
    boost::remove_edge(back_edges[0], g);
    // print_back_edges();
    back_edges.clear();
    detect_cycles_and_back_edges();
  }

  OrderedList* ordered = new OrderedList;
  graphVX::_topological_sort(ordered, g);
  //_print_list(*ordered, g);
  // dump_graph();

  // input and output images
  int n_in = dis3(gen);
  int n_out = dis3(gen);

  // inputs
  int cntr = 0;
  for (auto v : *ordered) {
    // make sure that initial nodes are images
    // if (boost::in_degree(v, g) == 1) break;
    if (g[v].task() != ObjectTask::Buffer) {
      clear_vertex(v, g);
      // remove_vertex(v, g);
    } else {
      if (cntr < n_in) {
        g[v].is_virtual(false);
        inputs.push_back(v);
        add_edge(v, nodes[dis(gen)]);
        cntr++;
      } else {
        break;
      }
    }
  }

  // outputs
  cntr = 0;
  for (auto v_it = ordered->rbegin(); v_it != ordered->rend(); ++v_it) {
    auto v = *v_it;
    // std:: cout << "------ " <<  g[v].name() << "
    //              , degree" << boost::out_degree(v, g) << std::endl;
    if (boost::out_degree(v, g) == 0) {
      if (g[v].task() == ObjectTask::Buffer) {
        // if(boost::in_degree(v, g) > 0 && cntr < n_out) {
        if (cntr < n_out) {
        g[v].is_virtual(false);
          outputs.push_back(v);
          add_edge(nodes[dis(gen)], v);
          cntr++;
        }
      } else {
        clear_vertex(v, g);
        // remove_vertex(v, g);
      }
    }
  }
  // print_io_nodes();
}

}  // namespace graphVX
