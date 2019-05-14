#pragma once
#include <boost/graph/graph_utility.hpp>  // print_graph
#include <boost/graph/graphviz.hpp>
#include "graph_visitors.hpp"
#include "../dsl/types.hpp"

namespace graphVX {

using VertexType = DomVX::Object;
using DomVX::ObjectTask;

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

}  // namespace graphVX
