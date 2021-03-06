#include "graph.hpp"

std::string hipaVX_output_filename = "main.hipaVX.cpp";

namespace graphVX {

//*********************************************************************
// Filling out the graph
//*********************************************************************
VertexDesc dag::get_vertex(int n) { return boost::vertex(n, g); }

VertexType dag::get_vertex_object(int n) {
  auto v = get_vertex(n);
  return g[v];
}

std::pair<EdgeDesc, bool> dag::add_edge(VertexDesc src, VertexDesc dst) {
  // TODO: we can do the error checking and return edge_descriptor
  return boost::add_edge(src, dst, g);
}

//*********************************************************************
// Graph Printing
//*********************************************************************
void dag::print_graph() { graphVX::_print_graph(g); }

void dag::write_graphviz(std::string _name) {
  graphVX::_write_graphviz(g, _name);
}

void dag::dump_graph(std::string _name) { graphVX::_dump_graph<AppGraphT>(this->g, _name); }

void dag::dump_reversed(std::string _name) {
  graphVX::_dump_graph(g_trans, _name);
}

void dag::dump_optimized(std::string _name) {
  graphVX::_dump_graph(*_g_opt, _name);
}

void dag::print_io_nodes() {
  std::cout << "inputs" << std::endl;
  for (auto i : inputs) std::cout << g[i].name() << " ";
  std::cout << "\noutputs" << std::endl;
  for (auto i : outputs) std::cout << g[i].name() << " ";
  std::cout << std::endl;
}

void dag::print_order_of_exec() {
  _print_list(*order_of_exec, *_g_opt, "order of execution: ");
}

//*********************************************************************
// Class methods for detecting cycles
//*********************************************************************
bool dag::detect_cycles() {
  cycle_detector_dfs vis(cycle_exist);
  depth_first_search(g, visitor(vis));

  if (cycle_exist) {
    std::cout << "The graph has a cycle " << std::endl;
  }

  return cycle_exist;
}

bool dag::detect_cycles_and_back_edges() {
  //cycle_detector_with_backedges_dfs<EdgeDesc> vis(cycle_exist,
  //                                                back_edges);
  //depth_first_search(g, visitor(vis));
  cycle_exist = _detect_cycles_and_back_edges(g, back_edges);
  return cycle_exist;
}

bool dag::has_cycle() { return cycle_exist; }

void dag::print_back_edges() {
  if (cycle_exist) {
    std::cout << "Edges at the cycles" << std::endl;
    for (auto it = begin(back_edges); it != end(back_edges); it++) {
      std::cout << g[source(*it, g)].name() << " --> "
                << g[target(*it, g)].name() << std::endl;
    }
    std::cout << "\n";
  }
}

//*********************************************************************
// Order of Execution (Topological Sort)
//*********************************************************************
OrderedList* dag::set_order_of_exec() {
   order_of_exec = new OrderedList;
  _topological_sort(order_of_exec, *_g_opt);
  return order_of_exec;
};

//*********************************************************************
// Dead node elimination
//*********************************************************************
AppGraphT* dag::reverse() {
  boost::transpose_graph(g, g_trans);
  return &g_trans;
}

// 1. g_trans : get a reverse view of the graph (g) (it is still the same graph)
// 2. mark the nodes from the results nodes to the inputs via depth first visit
// 3. filter the graph (g) according to the alive flag
OptGraphT* dag::eliminate_dead_nodes() {
  boost::transpose_graph(g, g_trans);

  mark_as_alive<AppGraphT> vis(g_trans);

  for (auto root : outputs) {
    _depth_first_visit(root, g_trans, vis, terminator(inputs, root));
  }

  auto is_alive = [this](VertexDesc vd) { return this->g_trans[vd].is_alive(); };

  _g_opt = new OptGraphT(g, boost::keep_all{}, is_alive);

  return _g_opt;
}

void dag::set_io_nodes() {
  AppGraphT::vertex_iterator v, vend;

  for (boost::tie(v, vend) = vertices(g); v != vend; ++v) {
    if(g[*v].is_virtual() == false && g[*v].task() == ObjectTask::Buffer) {
        bool is_in = (in_degree(*v, g) == 0);
        bool is_out = (out_degree(*v, g) == 0);

        //std::cout << "Node: " << g[*v].name()
        //          << ", in_degree = " << in_degree(*v, g)
        //          << ", out_degree = " << out_degree(*v, g) << std::endl;

        if(!is_in && !is_out) {
          inputs.push_back(*v);
          outputs.push_back(*v);
        }
        else if (is_in != is_out){
          if(is_in)
            inputs.push_back(*v);
          if(is_out)
            outputs.push_back(*v);
        }
    }
  }
}

OptGraphT* dag::dont_eliminate_dead_nodes() {
  boost::transpose_graph(g, g_trans);

  _g_opt = new OptGraphT(g, boost::keep_all{}, boost::keep_all{});

  return _g_opt;
}

}  // namespace graphVX
