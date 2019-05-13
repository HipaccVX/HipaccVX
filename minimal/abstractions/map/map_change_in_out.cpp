#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/cpp_gen/cpp_gen.hpp"
#include "../../../hipaVX/dsl/types.hpp"
#include "../../../hipaVX/dsl/abstractions.hpp"
#include "../../../hipaVX/dsl/api.hpp"

int main(int argc, const char *argv[]) {
  auto image_0 = new DomVX::Image(1024, 1024, VX_DF_IMAGE_U8);
  auto image_1 = new DomVX::Image(1024, 1024, VX_DF_IMAGE_U8);

  // Create a Statement with 1 output pixel accessor and 1 input pixel accessor
  auto ast_fun = create_p2p(1, 1);
  ast_fun << assign(ast_fun->d_out(0), sqrt(ast_fun->d_in(0)));

  auto map = create_point_op();
  // Set the map statement
  map->set_statements(ast_fun);
  // Bind the images to the correct pixel accessors
  map->register_images({image_0}, {image_1});

  CPPVisitor v;

  // image_0 is the output pixel
  std::cout << v.visit(map);

  // Swap the input and output image
  map->register_images({image_1}, {image_0});

  // image_1 is now the output pixel
  std::cout << v.visit(map);

  return 0;
}
