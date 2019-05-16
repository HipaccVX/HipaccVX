#include <string>
#include "../../../VX/vx.h"
#include "../../../hipaVX/dsl/abstractions.hpp"
#include "../../../hipaVX/dsl/api.hpp"
#include "../../../hipaVX/dsl/types.hpp"
#include "../../../hipaVX/graph_gen/cpp_graph_gen.hpp"

int main() {
  // Create a LocalToPixel function
  auto l_to_p = create_l2p(1, 1);
  auto win_1 = l_to_p->w_in(0);
  l_to_p << assign(
      l_to_p->d_out(0),
      win_1->pixel_at(0, 2) + win_1->pixel_at(0, 4) * constant(3.14f));

  // Create dummy input window and set its domain
  auto window_in = create_window_desc(3, 5);

  auto local_to_pixel = create_window_op();
  local_to_pixel->set_window_inputs({window_in});
  local_to_pixel->to_pixel(l_to_p);

  //---------------------------- HipaVX -------------------------------------

  auto image_o = new DomVX::Image(1024, 512, VX_DF_IMAGE_U8);
  auto image_i = new DomVX::Image(1024, 512, VX_DF_IMAGE_U8);

  // Create the local operation
  auto local_op = create_local_op();

  // Create the domain
  auto dom = create_domain(3, 5, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0});

  // Bind the dummy window accessors to the input images
  local_op->set_input_window_desc({{image_i, window_in}});
  // Add the Local to Pixel operation
  local_op->add_operation(local_to_pixel, {image_o});

  std::cout << cpp_abstraction_code_generator::dump_code(local_op, {}, {}, {},
                                                         true);

  return 0;
}
