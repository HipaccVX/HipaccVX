#include "../../../VX/vx.h"
#include "../../../hipaVX/domVX_types.hpp"
#include "../../../hipaVX/abstractions.hpp"
#include "../../../hipaVX/cpp_gen/cpp_gen.hpp"
#include "../../../hipaVX/domVX_api.hpp"
#include <string>
#include <iostream>

int main(int argc, const char *argv[])
{
    auto image_i = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto image_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);

    // Create a Statement with 1 output pixel accessor and 1 input pixel accessor
    auto ast_fun = create_p2p(1, 1);
    ast_fun << assign(ast_fun->d_out(0), sqrt(ast_fun->d_in(0)));

    auto map = create_point_op();
    // Set the map statement
    map->set_statements(ast_fun);
    // Bind the images to the correct pixel accessors
    map->register_images({image_o}, {image_i});

    CPPVisitor v;
    std::cout << v.visit(map);

    return 0;
}
