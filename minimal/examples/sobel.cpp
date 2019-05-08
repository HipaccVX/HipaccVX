#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../../hipaVX/cpp_gen/cpp_gen.hpp"
#include "../../hipaVX/graph_gen/hipacc_graph_gen.hpp"
#include "../../hipaVX/domVX_api.hpp"
#include <string>

int main()
{
  std::vector<std::shared_ptr<DomVX::AbstractionNode>> nodes;

  auto image_i = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);

  auto image_sob_x_unnorm = new HipaVX::Image(1024, 512, VX_TYPE_FLOAT32);
  auto image_sob_x = new HipaVX::Image(1024, 512, VX_TYPE_FLOAT32);
  auto image_sob_y_unnorm = new HipaVX::Image(1024, 512, VX_TYPE_FLOAT32);
  auto image_sob_y = new HipaVX::Image(1024, 512, VX_TYPE_FLOAT32);

  auto mask_x = create_mask(3, 3, {-1,  0,  1,
                                   -2,  0,  2,
                                   -1,  0,  1});
  auto mask_y = create_mask(3, 3, {-1, -2, -1,
                                    0,  0,  0,
                                    1,  2,  1});
  auto dom_x = create_dom(mask_x);
  auto dom_y = create_dom(mask_y);

  // Sobel
  //--- AST ---
  auto sobel_ast_forall = create_p2p_mask(1, 1, 1);
  sobel_ast_forall << assign(sobel_ast_forall->d_out(0), sobel_ast_forall->d_in(0) * sobel_ast_forall->m_in(0));

  auto sobel_ast_reduce = create_reduction(ast4vx::Constant<float>(0.f));
  sobel_ast_reduce << assign(sobel_ast_reduce->out(), sobel_ast_reduce->left() + sobel_ast_reduce->right());

  auto win_in = create_window_desc(3, 3);
  auto forall_op = forall(win_in, sobel_ast_forall);
  auto reduce_op = reduce(forall_op, sobel_ast_reduce); // TODO: I see, but then we should only give the reduce_op to the local operator

  auto sobel_ast_normalize = create_p2p(1, 1);
  sobel_ast_normalize << assign(sobel_ast_normalize->d_out(0), sobel_ast_normalize->d_in(0) * constant(1.f/(4*255*3)));

  //--- DomVX ---
  // TODO: maybe define images, variables and graph in create_() already
  // We should decide if we input the operators with image, domain or with both
  // There is also the roi and border handling so maybe it should be an accessor
  // We must have default values for roi, border handling to minimize the smallest description
  auto sobel_x_local_op = create_local_op();

  // TODO: then maybe we do not need these
  sobel_x_local_op->set_input_window_desc({{image_i, win_in}});
  sobel_x_local_op->set_domains({{win_in, dom_x}}); // TODO: (Simplify) create_window_desc, set_mask, set_domain basically have the same task
  sobel_x_local_op->set_masks({{forall_op, mask_x}}); // TODO: Which image if multiple?

  // TODO: consider this:
  // win0 = sobel_x->op(forall_op, sobel_x->im0, sobel_x->im1);
  // or
  // win0 = sobel_x->forall(sobel_x->in(0), sobel_x->out(0));
  // then
  // win1 = win0->op(reduce_op, win0, ...);
  sobel_x_local_op->add_operation(forall_op); // Maybe forall_op(mask_x)
  sobel_x_local_op->add_operation(reduce_op, {image_sob_x_unnorm});
  // TODO: consider this:
  //  win0 = sobel_x->op(for_all)->in(dom0)->out(im1)
  // sobel_x->op(reduce)->in(win0, win1)->out(im2)


  auto sobel_x_point_op = create_point_op();
  sobel_x_point_op->set_statements(sobel_ast_normalize);
  sobel_x_point_op->register_images({image_sob_x}, {image_sob_x_unnorm});

  auto sobel_y_local_op = create_local_op();
  sobel_y_local_op->set_input_window_desc({{image_i, win_in}});
  sobel_y_local_op->set_domains({{win_in, dom_y}});
  sobel_y_local_op->set_masks({{forall_op, mask_y}});
  sobel_y_local_op->add_operation(forall_op);
  sobel_y_local_op->add_operation(reduce_op, {image_sob_y_unnorm});
  auto sobel_y_point_op = create_point_op();
  sobel_y_point_op->set_statements(sobel_ast_normalize);
  sobel_y_point_op->register_images({image_sob_y}, {image_sob_y_unnorm});

  //nodes.emplace_back(sobel_x_local_op);
  //nodes.emplace_back(sobel_x_point_op);
  //nodes.emplace_back(sobel_y_local_op);
  //nodes.emplace_back(sobel_y_point_op);

  // graph
  dag g;
  auto sob_x = g.add_vertex(*(sobel_x_local_op).get());
  auto sob_xp = g.add_vertex(*(sobel_x_point_op).get());
  auto sob_y = g.add_vertex(*(sobel_y_local_op).get());
  auto sob_yp = g.add_vertex(*(sobel_y_point_op).get());
  auto im_i = g.add_vertex(*image_i);
  auto im_sobyun = g.add_vertex(*image_sob_y_unnorm);
  auto im_sobxun = g.add_vertex(*image_sob_x_unnorm);
  auto im_sobx = g.add_vertex(*image_sob_x);
  auto im_soby = g.add_vertex(*image_sob_y);

  g.inputs.push_back(im_i);
  g.outputs.push_back(im_soby);
  g.outputs.push_back(im_sobx);

  g.add_edge(im_i, sob_x);
  g.add_edge(im_i, sob_y);
  g.add_edge(sob_x, im_sobxun);
  g.add_edge(sob_y, im_sobyun);
  g.add_edge(im_sobyun, sob_yp);
  g.add_edge(im_sobxun, sob_xp);
  g.add_edge(sob_yp, im_sobx);
  g.add_edge(sob_xp, im_soby);

  g.eliminate_dead_nodes(); // TODO: this must be before the code gen, change this
  g.dump_graph();

  // generator functions
  hipacc_gen gen(g);
  gen.set_edges();

  // code generation, these will be implicit
  gen.iterate_spaces();
  gen.iterate_nodes();

  gen.dump_code();


  //CPPVisitor v;
  //for (auto node: nodes)
  //    std::cout << v.visit(node) << "\n";

  return 0;
}

