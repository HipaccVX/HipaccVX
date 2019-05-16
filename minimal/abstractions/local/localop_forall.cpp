#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/dsl/abstractions.hpp"
#include "../../../hipaVX/dsl/api.hpp"
#include "../../../hipaVX/dsl/types.hpp"
#include "../../../hipaVX/graph_gen/cpp_graph_gen.hpp"

int main() {
  // Created two sample pixel to pixel functions for later use
  auto ast_forall = create_p2p(1, 1);
  ast_forall << assign(ast_forall->d_out(0),
                       ast_forall->d_in(0) * ast_forall->d_in(0));

  // Reduction AST expects an initial value for the accumulator
  auto ast_reduction = create_reduction(ast4vx::Constant<int>(0));
  ast_reduction << assign(ast_reduction->out(),
                          ast_reduction->left() + ast_reduction->right());

  // Create dummy input window and set its domain
  auto window_in = create_window_desc(3, 5);

  // Set the Pixel to Pixel function which should be done for all pixels in the
  // domain
  auto window_op_1 = forall(window_in, ast_forall);

  // Chain a reduce function
  // This is done via setting the window_op_1's output to this operations input
  auto reduction_op = reduce(window_op_1, ast_reduction);

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

  std::cout << cpp_abstraction_code_generator::dump_code(local_op, {}, {}, {},
                                                         true);

  return 0;
}
