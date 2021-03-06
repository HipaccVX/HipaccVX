#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/dsl/abstractions.hpp"
#include "../../../hipaVX/dsl/api.hpp"
#include "../../../hipaVX/dsl/types.hpp"
#include "../../../hipaVX/gen/cpp_graph.hpp"

int main() {
  // Created two sample pixel to pixel functions for later use
  auto ast_forall = create_p2p(1, 1);
  ast_forall->set_variable_inout_count(0, 1);
  ast_forall << assign(ast_forall->d_out(0),
                       ast_forall->d_in(0) / ast_forall->v_in(0));

  // Reduction AST expects an initial value for the accumulator
  auto ast_reduction = create_reduction(ast4vx::Constant<float>(.0f));
  ast_reduction << assign(ast_reduction->out(),
                          ast_reduction->left() + ast_reduction->right());

  // Create dummy input window and set its domain
  auto window_in = create_window_desc(3, 3);

  // There are handy functions for "forall" and "reduce", which reduces the
  // mundane calls
  auto forall_op = forall(window_in, ast_forall);
  auto reduction_op = reduce(forall_op, ast_reduction);

  //---------------------------- DomVX --------------------------------------

  auto image_i = new DomVX::Image(1024, 512, VX_DF_IMAGE_U8);
  auto image_o = new DomVX::Image(1024, 512, VX_DF_IMAGE_U8);

  auto dom = create_domain(3, 3, {1, 1, 1, 1, 1, 1, 1, 1, 1});
  vx_uint8 var_0_value = 9;
  auto var_0 = new DomVX::Scalar(VX_TYPE_UINT8, &var_0_value);
  // Create the local operation
  auto local_op = create_local_op();

  local_op->set_domains({{window_in, dom}});

  // Bind the dummy window accessors to the input images
  local_op->set_input_window_desc({{image_i, window_in}});

  // Add the Pixel to Pixel compute_at operation
  local_op->add_operation(forall_op, {}, {var_0});
  // Add the reduction operation, needs an output image
  local_op->add_operation(reduction_op, {image_o});

  std::cout << cpp_abstraction_code_generator::dump_code(local_op, {}, {}, {},
                                                         true);

  return 0;
}
