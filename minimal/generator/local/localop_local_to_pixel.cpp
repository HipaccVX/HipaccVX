#include "../../../VX/vx.h"
#include "../../../hipaVX/domVX_types.hpp"
#include "../../../hipaVX/abstractions.hpp"
#include "../../../hipaVX/cpp_gen/cpp_gen.hpp"
#include <string>

int main()
{
    // Create a LocalToPixel function
    auto l_to_p = std::make_shared<ast4vx::LocalToPixel>(1, 1);
    auto win_1 = l_to_p->window(0);
    l_to_p << assign(l_to_p->d_out(0), win_1->pixel_at(0, 2) + win_1->pixel_at(0, 4) * constant(3.14f));


    // Create dummy input window and set its domain
    auto window_in = std::make_shared<ast4vx::WindowDescriptor>(3, 5);

    auto local_to_pixel = std::make_shared<ast4vx::WindowOperation>();
    local_to_pixel->set_window_inputs({window_in});
    local_to_pixel->to_pixel(l_to_p);

    //---------------------------- HipaVX -------------------------------------

    auto image_o = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);
    auto image_i = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);

    // Create the local operation
    auto local_op = std::shared_ptr<DomVX::LocalOperation>(new DomVX::LocalOperation());

    // Create the domain
    auto dom = std::shared_ptr<DomVX::Domain>(new DomVX::Domain(3, 5, {0, 0, 1,
                                                                       0, 0, 0,
                                                                       0, 0, 0,
                                                                       0, 0, 0,
                                                                       1, 0, 0}));

    // Bind the dummy window accessors to the input images
    local_op->set_input_window_desc({{image_i, window_in}});
    // Add the Local to Pixel operation
    local_op->add_operation(local_to_pixel, {image_o});

    CPPVisitor v;
    std::cout << v.visit(local_op) << "\n";

    return 0;
}

