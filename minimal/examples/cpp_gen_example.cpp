// export CPLUS_INCLUDE_PATH=../../
// g_dag++ -Wall -Werror 07*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/dsl/abstractions.hpp"
#include "../../hipaVX/dsl/api.hpp"
#include "../../hipaVX/dsl/ast.hpp"
#include "../../hipaVX/dsl/types.hpp"
#include "../../hipaVX/graph/graph.hpp"
#include "../../hipaVX/graph_gen/cpp_graph_gen.hpp"

using DomVX::Image;
using DomVX::Node;
using DomVX::Scalar;
using graphVX::AppGraphT;
using graphVX::dag;
using graphVX::OptGraphT;

int main() {
  // image
  DomVX::Image im0(1024, 1024, VX_DF_IMAGE_U8);
  DomVX::Image im1(1024, 1024);
  im1.set_dtype(VX_DF_IMAGE_U8);
  DomVX::Image im2(1024, 1024, VX_DF_IMAGE_U8);

  auto dom = create_domain(3, 5, {0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1});
  auto mask =
      create_mask(3, 5, {-1, -1, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 1, 1});

  // for forall
  auto ast_p2p = create_p2p_mask(1, 1, 1);
  ast_p2p << assign(ast_p2p->d_out(0),
                    sqrt(ast_p2p->d_in(0) * ast_p2p->m_in(0)));

  // local
  auto l_to_p = create_l2p(1, 1);
  auto win_1 = l_to_p->w_in(0);
  l_to_p << assign(
      l_to_p->d_out(0),
      win_1->pixel_at(0, 2) + win_1->pixel_at(0, 4) * constant(3.14f));

  auto window_in = create_window_desc(3, 5);

  auto forall_op = forall(window_in, ast_p2p);
  auto local_to_pixel = create_window_op();
  local_to_pixel->set_window_inputs({forall_op->get_window_output()});
  local_to_pixel->to_pixel(l_to_p);

  auto local_op = create_local_op();
  local_op->set_input_window_desc({{&im0, window_in}});
  local_op->set_domains({{window_in, dom}});
  local_op->set_masks({{forall_op, mask}});
  local_op->add_operation(forall_op);
  local_op->add_operation(local_to_pixel, {&im1});

  auto ast_mul = create_p2p(1, 1);
  ast_mul << assign(ast_mul->d_out(0), ast_mul->d_in(0) * constant(0.5f));

  auto point_op = create_point_op();
  point_op->set_statements(ast_mul);
  point_op->register_images({&im2}, {&im1});

  // graph
  dag g;
  auto v_im0 = g.add_vertex(im0);
  auto v_im1 = g.add_vertex(im1);
  auto v_im2 = g.add_vertex(im2);
  auto v_n0 = g.add_vertex(*(local_op.get()));
  auto v_n1 = g.add_vertex(*(point_op.get()));

  g.inputs.push_back(v_im0);
  g.outputs.push_back(v_im2);

  g.add_edge(v_im0, v_n0);
  g.add_edge(v_n0, v_im1);
  g.add_edge(v_im1, v_n1);
  g.add_edge(v_n1, v_im2);

  g.eliminate_dead_nodes();  // TODO: this must be before the code gen, change
                             // this
  g.dump_graph();

  // generator functions
  cpp_gen gen(g);
  gen.set_edges();

  // code generation, these will be implicit
  gen.iterate_spaces();
  gen.iterate_nodes();

  gen.dump_code();

  return 0;
}
