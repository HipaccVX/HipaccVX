#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../../hipaVX/cpp_gen/cpp_gen.hpp"
#include <string>

int main()
{
    //AST
    auto ast_fun = std::make_shared<function_ast::Statements>(3);
    ast_fun << assign(ast_fun->pixel_accessor(0), ast_fun->pixel_accessor(1) * (ast_fun->pixel_accessor(2) + ast_fun->pixel_accessor(1)));

    CPPVisitor v;

    //DomVX
    auto image_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto image_i_1 = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto image_i_2 = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);

    auto for_every_pixel = std::make_shared<DomVX::ForEveryPixelTest>(image_o);


    auto t = std::make_shared<DomVX::MapTest>();
    t->set_statements(ast_fun);
    t->register_input_image({image_i_1, image_i_2});

    for_every_pixel->map(t);

    std::cout << "AST Generation:\n";
    std::cout << v.visit(ast_fun, 0);
    std::cout << "\n";

    std::cout << "DomVX Map Generation:\n";
    std::cout << v.visit(t, 0);
    std::cout << "\n";

    std::cout << "DomVX ForEveryPixel Generation:\n";
    std::cout << v.visit(for_every_pixel, 0);
    std::cout << "\n";





    return 0;
}
