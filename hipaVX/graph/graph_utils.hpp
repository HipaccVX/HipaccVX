#pragma once
#include <boost/graph/graph_utility.hpp>  // print_graph
#include <boost/graph/graphviz.hpp>
#include "graph_visitors.hpp"

namespace graphVX {

using VertexType = DomVX::Object;

// create an .dot file for the graphviz
template <class GraphT>
void _write_graphviz(GraphT _g, std::string name) {
  std::ofstream file_out(name + ".dot");
  boost::write_graphviz(file_out, _g,
      make_vertex_writer(boost::get(&VertexType::obj_name, _g),
                         boost::get(&VertexType::obj_task, _g)));
}


// print the whole graph
template <class GraphT>
void _print_graph(GraphT _g) {
  boost::print_graph(_g, get(&VertexType::obj_name, _g));
}


// print the graph and .dot file
template <class GraphT>
void _dump_graph(GraphT _g, std::string _name) {
  std::cout << _name << std::endl;
  graphVX::_print_graph(_g);
  graphVX::_write_graphviz(_g, _name);
  std::cout << "\n";
}


// print a given Vertex or Edge list
template <class VertexOrEdgeDesc, class GraphT>
void _print_list(std::list<VertexOrEdgeDesc> list, GraphT g,
                 std::string message = "") {
  std::cout << message << std::endl;
  for (auto i = list.begin(); i != list.end(); ++i)
    std::cout << g[*i].name() << " ";
  std::cout << std::endl << std::endl;
}


// topological sort
template <class VertexDesc, class GraphT>
void _topological_sort(std::list<VertexDesc>* sorted, GraphT g) {
  //using OrderedList = std::list<VertexDesc>;
  //OrderedList* sorted = new OrderedList;

  topological_sort(g, std::front_inserter(*sorted));
}


// dfs visit (not search)
template <class VertexDesc, class GraphT, class VisitorT, class TerminatorT>
void _depth_first_visit(VertexDesc root_vertex, GraphT g, VisitorT vis,
                        TerminatorT terminator) {
  // depth first visit needs a color map
  std::vector<boost::default_color_type> colors(boost::num_vertices(g));
  auto color_map = boost::make_iterator_property_map(
      colors.begin(), boost::get(boost::vertex_index, g));

  boost::depth_first_visit(g, root_vertex, vis, color_map, terminator);
}

#include "graph_visitors.hpp"

template<class GraphT>
bool _detect_cycles_and_back_edges(GraphT &g,
    std::vector<typename boost::graph_traits<GraphT>::edge_descriptor>& back_edges) {
  bool cycle_exist = false;

  using EdgeDesc = typename boost::graph_traits<GraphT>::edge_descriptor;

  cycle_detector_with_backedges_dfs<EdgeDesc> vis(cycle_exist, back_edges);
  depth_first_search(g, visitor(vis));
  return cycle_exist;
}

//*********************************************************************
// Random Graph Generation
//*********************************************************************

#include <boost/graph/adjacency_list.hpp>
#include <random>

template <class Node, class Data, typename GraphT>
void _gen_rand_graph(unsigned n, unsigned k, GraphT &g,
      std::vector<typename GraphT::vertex_descriptor>& inputs,
      std::vector<typename GraphT::vertex_descriptor>& outputs,
      unsigned num_inputs = 1, unsigned num_outputs = 2
      ) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, n - 1);
  std::uniform_int_distribution<> dis3(1, 3);

  using VertexDesc = typename GraphT::vertex_descriptor;

  VertexDesc images[n];
  VertexDesc nodes[n];

  for (unsigned v = 0; v < n; v++) {
    auto new_node = new Node();
    auto new_image = new Data();
    nodes[v] = boost::add_vertex(*new_node, g);
    images[v] = boost::add_vertex(*new_image, g);
  }

  // input and output images are not virtual
  for (unsigned i = 0; i < num_inputs; i++) {
    auto v = images[i];
    g[v].is_virtual(false);
    inputs.push_back(v);
    boost::add_edge(v, nodes[dis(gen)], g);
  }
  for (unsigned i = 0; i < num_outputs; i++) {
    auto v = images[n - 1 - i];
    g[v].is_virtual(false);
    outputs.push_back(v);
    boost::add_edge(nodes[dis(gen)], v, g);
  }

  for (unsigned i = 0; i < k; i++) {
    unsigned u = dis(gen);  // rand() % n;
    unsigned v = dis(gen);  // rand() % n;
    if (i % 2)
      boost::add_edge(images[u], nodes[v], g);
    else
      boost::add_edge(nodes[v], images[v], g);
  }
}


template <class Node, class Image, typename GraphT>
void _gen_rand_acyclic_graph(unsigned n, unsigned k, GraphT &g,
      std::vector<typename GraphT::vertex_descriptor>& inputs,
      std::vector<typename GraphT::vertex_descriptor>& outputs,
      unsigned num_inputs = 1, unsigned num_outputs = 2
      ) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, n - 1);
  std::uniform_int_distribution<> dis3(1, 3);

  using VertexDesc = typename GraphT::vertex_descriptor;
  VertexDesc images[n];
  VertexDesc nodes[n];

  // Create a random graph
  for (unsigned v = 0; v < n; v++) {
    auto new_node = new Node();
    new_node->is_virtual(true);
    auto new_image = new Image(1024, 1024, VX_DF_IMAGE_U8);
    nodes[v] = add_vertex(*new_node, g);
    images[v] = add_vertex(*new_image, g);
  }

  for (unsigned i = 0; i < k; i++) {
    unsigned u = dis(gen);  // rand() % n;
    unsigned v = dis(gen);  // rand() % n;
    if (i % 2)
      add_edge(images[u], nodes[v], g);
    else
      add_edge(nodes[v], images[v], g);
  }

  // cleanup cycles
  using EdgeDesc = typename GraphT::edge_descriptor;
  std::vector<EdgeDesc> back_edges;
  bool cycle_exist = _detect_cycles_and_back_edges(g, back_edges);
  while (cycle_exist) {
    boost::remove_edge(back_edges[0], g);
    // print_back_edges();
    back_edges.clear();
    cycle_exist = _detect_cycles_and_back_edges(g, back_edges);
  }

  std::list<VertexDesc> ordered;
  graphVX::_topological_sort(&ordered, g);
  //_print_list(ordered, g);
  // dump_graph();

  // inputs
  unsigned icntr = 0, ocntr = 0;
  for (auto v : ordered) {
    // std:: cout << "------ " <<  g[v].name() << ", degree" << boost::out_degree(v, g) << std::endl;
    // make sure that initial nodes are images
    if(g[v].task() == ObjectTask::Buffer) {
      if (icntr < num_inputs) {
        g[v].is_virtual(false);
        inputs.push_back(v);
        add_edge(v, nodes[dis(gen)], g);
        icntr++;
      }
      if (ocntr < num_outputs && boost::out_degree(v, g) == 0) {
        g[v].is_virtual(false);
        outputs.push_back(v);
        add_edge(nodes[dis(gen)], v, g);
        ocntr++;
      }
    }
  }

  for (auto v : ordered) {
    // std:: cout << "------ " <<  g[v].name() << ", degree" << boost::out_degree(v, g) << std::endl;
    // make sure that root and child nodes are images
    if (g[v].task() != ObjectTask::Buffer) {
      if(boost::in_degree(v, g) == 0 || boost::out_degree(v, g) == 0) {
        clear_vertex(v, g);
      }
    }
  }
  // print_io_nodes();
}

}  // namespace graphVX
