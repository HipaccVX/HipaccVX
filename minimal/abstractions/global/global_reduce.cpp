#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/dsl/abstractions.hpp"
#include "../../../hipaVX/dsl/api.hpp"
#include "../../../hipaVX/dsl/types.hpp"
#include "../../../hipaVX/graph_gen/cpp_graph_gen.hpp"

int main() {
  auto ast_reduction = create_reduction(ast4vx::Constant<int>(255));
  auto left_greater =
      IF(greater(ast_reduction->left(), ast_reduction->right()));
  left_greater << assign(ast_reduction->out(), ast_reduction->right());
  auto right_greater = ELSE();
  right_greater << assign(ast_reduction->out(), ast_reduction->left());
  ast_reduction << left_greater << right_greater;

  //---------------------------- DomVX --------------------------------------

  auto image_i = create_image(1024, 512, VX_DF_IMAGE_U8);
  int var = 0;
  auto var_out = new DomVX::Scalar(VX_TYPE_INT32, &var);

  // Create the local operation
  auto global_op = create_global_op();

  global_op->register_input_images({image_i});
  global_op->set_reduction_function(ast_reduction, var_out);

  std::cout << cpp_abstraction_code_generator::dump_code(global_op, {}, {},
                                                         true);

  return 0;
}
