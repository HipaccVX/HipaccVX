#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../../hipaVX/cpp_gen/cpp_gen.hpp"
#include <string>

// A simple point operator
// TODO: generate functions should check whether the pointers are null or not

int main(int argc, const char *argv[]) {

    CPPVisitor v;
    auto map = std::make_shared<DomVX::MapTest>();
	
    auto image_i = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto image_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);

    auto ast_fun = std::make_shared<function_ast::Statements>(2);
    ast_fun << assign(ast_fun->pixel_accessor(0), sqrt(ast_fun->pixel_accessor(1)));

    map->set_statements(ast_fun);
    map->register_image({image_o, image_i});

    std::cout << v.visit(map);

    return 0;
}
