#include "graph.hpp"

namespace graphVX {

// ------------------------- filling out the graph -----------------------------
//template <class Vertex>
//VertexDesc dag::add_vertex(Vertex v) {
//  return boost::add_vertex(v, g);
//}

VertexDesc dag::get_vertex(int n) { return boost::vertex(n, g); }

VertexType dag::get_vertex_object(int n) {
  auto v = get_vertex(n);
  return g[v];
}

std::pair<EdgeDesc, bool> dag::add_edge(VertexDesc src, VertexDesc dst) {
  // TODO: we can do the error checking and return edge_descriptor
  return boost::add_edge(src, dst, g);
}

// ------------------ class methods for printing -------------------------------
void dag::print_graph() { graphVX::_print_graph(g); }

void dag::write_graphviz(std::string _name) {
  graphVX::_write_graphviz(g, _name);
}

void dag::dump_graph(std::string _name) { graphVX::_dump_graph(g, _name); }

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

// ------------------ class methods for detecting cycles -----------------------
bool dag::detect_cycles() {
  cycle_detector_dfs vis(cycle_exist);
  depth_first_search(g, visitor(vis));

  if (cycle_exist) {
    std::cout << "The graph has a cycle " << std::endl;
  }

  return cycle_exist;
}

bool dag::detect_cycles_and_back_edges() {
  cycle_detector_with_backedges_dfs<EdgeDesc> vis(cycle_exist, back_edges);
  depth_first_search(g, visitor(vis));
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

// ------------------ class methods for dead node elimination ------------------
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
    _depth_first_visit(root, g_trans, vis, terminator(inputs));
  }

  auto is_alive = [this](VertexDesc vd) { return this->g_trans[vd].is_alive(); };

  _g_opt = new OptGraphT(g, boost::keep_all{}, is_alive);

  return _g_opt;
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
    g[v].virt = false;
    inputs.push_back(v);
    add_edge(v, nodes[dis(gen)]);
  }
  for (int i = 0; i < n_out; i++) {
    auto v = images[n - 1 - i];
    g[v].virt = false;
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
        g[v].virt = false;
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
          g[v].virt = false;
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
