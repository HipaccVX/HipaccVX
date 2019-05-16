#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/graph_gen/cpp_graph_gen.hpp"
#include "../../../hipaVX/dsl/types.hpp"
#include "../../../hipaVX/dsl/abstractions.hpp"
#include "../../../hipaVX/dsl/api.hpp"

int main() {
  // Created two sample pixel to pixel functions for later use
  auto ast_fun_1 = create_p2p(1, 1);
  ast_fun_1 << assign(ast_fun_1->d_out(0), ast_fun_1->d_in(0) * constant(3.4f));

  auto ast_fun_2 = create_p2p(1, 0);
  ast_fun_2 << assign(ast_fun_2->d_out(0), sqrt(constant(2.f)));

  // Reduction AST expects an initial value for the accumulator
  auto ast_reduction = create_reduction(ast4vx::Constant<int>(0));
  ast_reduction << assign(ast_reduction->out(),
                          ast_reduction->left() + ast_reduction->right());

  // Create dummy input window and set its domain
  auto window_in = create_window_desc(3, 5);

  // Associate pixel to pixel functions with the domain coordinates via
  // compute_at these pixel to pixel functions must have one output pixel
  // accessor
  auto window_op_1 = create_window_op();
  window_op_1->set_window_inputs({window_in});
  window_op_1->compute_at(2, 0, ast_fun_1);
  window_op_1->compute_at(0, 2, ast_fun_2);
  window_op_1->compute_at(2, 4, ast_fun_2);

  // Chain a reduce function
  // This is done via setting the window_op_1's output to this operations input
  auto reduction_op = create_window_op();
  reduction_op->set_window_inputs({window_op_1->get_window_output()});
  reduction_op->reduce(ast_reduction);

  //---------------------------- DomVX --------------------------------------

  auto image_i = new DomVX::Image(1024, 512, VX_DF_IMAGE_U8);
  auto image_o = new DomVX::Image(1024, 512, VX_DF_IMAGE_U8);

  // Create the local operation
  auto local_op = create_local_op();

  // Create the domain
  auto dom = create_domain(3, 5, {0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1});

  // Bind the dummy window accessors to the input images
  local_op->set_input_window_desc({{image_i, window_in}});

  // Bind the window accessors to the domains, if a window descriptor is not
  // set, it will inherit the domain from the predecessor
  local_op->set_domains({{window_in, dom}});

  // Add the Pixel to Pixel compute_at operation
  local_op->add_operation(window_op_1);
  // Add the reduction operation, needs an output image
  local_op->add_operation(reduction_op, {image_o});

  std::cout << cpp_abstraction_code_generator::dump_code(local_op, {}, {}, {}, true);

  return 0;
}
