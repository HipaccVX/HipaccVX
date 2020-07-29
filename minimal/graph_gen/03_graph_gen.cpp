// export CPLUS_INCLUDE_PATH=../../
// g_dag++ -Wall -Werror 07*.cpp ../../hipaVX/domVX_types.cpp

#include <iostream>
#include "../../hipaVX/dsl/abstractions.hpp"
#include "../../hipaVX/graph/graph.hpp"
#include "../../hipaVX/gen/hipacc_graph.hpp"


using graphVX::AppGraphT;
using graphVX::OptGraphT;
using graphVX::dag;
using DomVX::Image;
using DomVX::Node;
using DomVX::Scalar;


int main() {
  // image
  DomVX::Image im0(1024, 1024, VX_DF_IMAGE_U8);
  DomVX::Image im1(1024, 1024);
  im1.set_dtype(VX_DF_IMAGE_U8);

 auto dom = create_dom(3, 5, {0, 0, 1,
                              0, 1, 0,
                              1, 0, 0,
                              0, 1, 0,
                              0, 0, 1});
 auto mask = create_mask(3, 5,
       {-1, -1,  1,
        -1,  0,  1,
        -1,  0,  1,
        -1,  0,  1,
        -1,  1,  1});

  // map
  auto ast_fun = std::make_shared<ast4vx::Statements>(1, 1);
  ast_fun << assign(ast_fun->d_out(0), sqrt(ast_fun->d_in(0)));

  auto map = create_point_op();
  map->set_statements(ast_fun);
  map->register_images({&im0}, {&im1});

  // local
  auto l_to_p = create_l2p(1, 1);
  auto win_1 = l_to_p->w_in(0);
  l_to_p << assign(l_to_p->d_out(0), win_1->pixel_at(0, 2) + win_1->pixel_at(0, 4) * constant(3.14f));

  auto window_in = create_window_desc(3, 5);

  auto local_to_pixel = create_window_op();
  local_to_pixel->set_window_inputs({window_in});
  local_to_pixel->to_pixel(l_to_p);

  auto local_op = create_local_op();
  local_op->set_input_window_desc({{&im0, window_in}});
  local_op->set_domains({{window_in, dom}});
  local_op->set_masks({{local_to_pixel, mask}});
  local_op->add_operation(local_to_pixel, {&im1});

  // graph
  dag g;
  auto v_im0 = g.add_vertex(im0);
  auto v_im1 = g.add_vertex(im1);
  auto v_n0  = g.add_vertex(*(local_op.get()));

  g.inputs.push_back(v_im0);
  g.outputs.push_back(v_im1);


  g.add_edge(v_im0, v_n0);
  g.add_edge(v_n0, v_im1);

  g.eliminate_dead_nodes(); // TODO: this must be before the code gen, change this
  g.dump_graph();

  // generator functions
  hipacc_gen gen(g);
  gen.set_edges();

  // code generation, these will be implicit
  gen.iterate_spaces();
  gen.iterate_nodes();

  gen.dump_code();

  return 0;
}
