#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/dsl/abstractions.hpp"
#include "../../../hipaVX/dsl/api.hpp"
#include "../../../hipaVX/dsl/types.hpp"
#include "../../../hipaVX/gen/cpp_graph.hpp"

int main() {
  // Created two sample pixel to pixel functions for later use
  auto ast_p2p_mult = create_p2p_mask(1, 1, 1);
  ast_p2p_mult->d_out(0) = ast_p2p_mult->d_in(0) * ast_p2p_mult->m_in(0);

  // Reduction AST expects an initial value for the accumulator
  auto ast_red_sum = create_reduction(ast4vx::Constant<float>(.0f));
  ast_red_sum->out() = ast_red_sum->left() + ast_red_sum->right();

  // Create dummy input window and set its domain
  // There are handy functions for "forall" and "reduce", which
  // reduces the mundane calls
  auto window_in = create_window_desc(3, 5);
  auto forall_op = forall(window_in, ast_p2p_mult);
  auto reduction_op = reduce(forall_op, ast_red_sum);

  //---------------------------- DomVX --------------------------------------

  auto image_i = create_image(1024, 512, VX_DF_IMAGE_U8);
  auto image_o = create_image(1024, 512, VX_DF_IMAGE_U8);

  auto mask =
      create_mask(3, 5, {-1, -1, 1, -1, 0, 1, -1, 0, 1, -1, 0, 1, -1, 1, 1});
  auto dom = create_domain(mask);

  // Create the local operation
  // Bind the dummy window accessors to the input images
  // Bind the masks to their corresponding WindowOperations
  // Add the Pixel to Pixel compute_at operation
  // Add the reduction operation, needs an output image
  auto local = create_local_op();
  local->set_domains({{window_in, dom}});
  local->set_input_window_desc({{image_i, window_in}});
  local->set_masks({{forall_op, mask}});
  local->add_operation(forall_op);
  local->add_operation(reduction_op, {image_o});

  std::cout << cpp_abstraction_code_generator::dump_code(local, {}, {}, {},
                                                         true);
  return 0;
}
