#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/abstractions.hpp"
#include "../../../hipaVX/cpp_gen/cpp_gen.hpp"
#include "../../../hipaVX/domVX_types.hpp"

int main() {
  auto ast_reduction = create_reduction(ast4vx::Constant<int>(255));
  auto left_greater =
      IF(greater(ast_reduction->left(), ast_reduction->right()));
  left_greater << assign(ast_reduction->out(), ast_reduction->right());
  auto right_greater = ELSE();
  right_greater << assign(ast_reduction->out(), ast_reduction->left());
  ast_reduction << left_greater << right_greater;

  //---------------------------- DomVX --------------------------------------

  auto image_i = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);
  auto var_out = new HipaVX::Scalar(VX_TYPE_UINT8);

  // Create the local operation
  auto global_op = create_global_op();

  global_op->register_input_images({image_i});
  global_op->set_reduction_function(ast_reduction, var_out);

  CPPVisitor v;
  std::cout << v.visit(global_op) << "\n";

  return 0;
}
