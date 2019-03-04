#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../../hipaVX/cpp_gen/cpp_gen.hpp"
#include <string>

int main(int argc, const char *argv[])
{
    CPPVisitor v;

    auto map = std::make_shared<DomVX::MapTest>();

    auto image_0 = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto image_1 = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);

    auto ast_fun = std::make_shared<function_ast::Statements>(2);
    ast_fun << assign(ast_fun->pixel_accessor(0), sqrt(ast_fun->pixel_accessor(1)));

    // Map function, and changing its IOs
    map->set_statements(ast_fun);
    map->register_image({image_0, image_1});

    std::cout << "statement: " << v.visit(map->get_statements(), 0) << std::endl;
    std::cout << "generate: " << v.visit(map, 0) << std::endl;
    std::cout << std::endl;

    std::cout << "swap map input:" << std::endl;
    map->register_image({image_1, image_0});
    std::cout << "statement: " << v.visit(map->get_statements(), 0) << std::endl;
    std::cout << "generate: " << v.visit(map, 0) << std::endl;

    return 0;
}
