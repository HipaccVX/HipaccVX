#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/dsl/abstractions.hpp"
#include "../../../hipaVX/dsl/api.hpp"
#include "../../../hipaVX/dsl/types.hpp"
#include "../../../hipaVX/gen/cpp_graph.hpp"

int main() {
  //---------------------------- DomVX --------------------------------------

  auto image_i = new DomVX::Image(1024, 512, VX_DF_IMAGE_U8);
  auto image_o = new DomVX::Image(1024, 512, VX_DF_IMAGE_U8);
  // Create the domain
  auto dom = create_domain(3, 5, {0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1});

  auto win_in = get_window(image_i, dom);

  //---------------------------- AST --------------------------------------

  // Created two sample pixel to pixel functions for later use
  auto ast_forall = create_p2p(1, 1);
  ast_forall << assign(ast_forall->d_out(0),
                       ast_forall->d_in(0) * ast_forall->d_in(0));

  // Reduction AST expects an initial value for the accumulator
  auto ast_reduction = create_reduction(ast4vx::Constant<int>(0));
  ast_reduction << assign(ast_reduction->out(),
                          ast_reduction->left() + ast_reduction->right());

  // Set the Pixel to Pixel function which should be done for all pixels in the
  // domain
  auto window_op_1 = forall(win_in, ast_forall);

  // Chain a reduce function
  // This is done via setting the window_op_1's output to this operations input
  auto reduction_op = reduce(window_op_1, ast_reduction);

  //---------------------------- DomVX --------------------------------------

  // Create the local operation
  auto local_op = create_local_op();

  // Add all operations which leads to this reduction operation (included),
  // needs an output image
  local_op->add_operation(reduction_op, {image_o});

  std::cout << cpp_abstraction_code_generator::dump_code(local_op, {}, {}, {},
                                                         true);

  return 0;
}
