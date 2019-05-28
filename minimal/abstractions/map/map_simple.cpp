#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/dsl/abstractions.hpp"
#include "../../../hipaVX/dsl/api.hpp"
#include "../../../hipaVX/dsl/types.hpp"
#include "../../../hipaVX/gen/cpp_graph.hpp"

int main(int argc, const char *argv[]) {
  auto image_i = new DomVX::Image(1024, 1024, VX_DF_IMAGE_U8);
  auto image_o = new DomVX::Image(1024, 1024, VX_DF_IMAGE_U8);

  // Create a Statement with 1 output pixel accessor and 1 input pixel accessor
  auto ast_fun = create_p2p(1, 1);
  ast_fun->d_out(0) = sqrt(ast_fun->d_in(0));

  auto map = create_point_op();
  // Set the map statement and bind the images to the correct pixel accessors
  map->op(ast_fun)->out(image_o)->in(image_i);

  std::cout << cpp_abstraction_code_generator::dump_code(map, {}, {}, {}, true);

  return 0;
}
