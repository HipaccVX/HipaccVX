#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../../hipaVX/cpp_gen/cpp_gen.hpp"
#include <string>

// A simple point operator
// TODO: generate functions should check whether the pointers are null or not

int main(int argc, const char *argv[]) {

    CPPVisitor v;
    auto map = std::make_shared<DomVX::Map>();
	
    auto image_i = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto image_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);

    auto ast_fun = std::make_shared<ast4vx::Statements>(1, 1);
    ast_fun << assign(ast_fun->d_out(0), sqrt(ast_fun->d_in(0)));

    map->set_statements(ast_fun);
    map->register_images({image_o}, {image_i});

    std::cout << v.visit(map);

    return 0;
}
