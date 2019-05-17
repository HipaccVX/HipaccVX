#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/gen/cpp_graph.hpp"
#include "../../../hipaVX/dsl/types.hpp"
#include "../../../hipaVX/dsl/abstractions.hpp"
#include "../../../hipaVX/dsl/api.hpp"

int main(int argc, const char *argv[]) {
  auto image_0 = new DomVX::Image(1024, 1024, VX_DF_IMAGE_U8);
  auto image_1 = new DomVX::Image(1024, 1024, VX_DF_IMAGE_U8);
  vx_uint8 var_0_value = 42;
  auto var_0 = new DomVX::Scalar(VX_TYPE_UINT8, &var_0_value);

  // Create a Statement with 1 output pixel accessor and 1 input pixel accessor
  auto ast_fun = create_p2p(1, 1);
  // Add 0 output variables and 1 input variable
  ast_fun->set_variable_inout_count(0, 1);
  ast_fun << assign(ast_fun->d_out(0),
                    sqrt(ast_fun->d_in(0)) + ast_fun->v_in(0));

  auto map = create_point_op();
  // Set the map statement
  map->set_statements(ast_fun);
  // Bind the images to the correct pixel accessors
  map->register_images({image_0}, {image_1});
  // Bind the variables to the correct variable accessors
  map->register_variables({}, {var_0});

  std::cout << cpp_abstraction_code_generator::dump_code(map, {}, {}, {}, true);

  return 0;
}
