#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../../hipaVX/cpp_gen/cpp_gen.hpp"
#include <string>

int main()
{
    //AST
    auto ast_fun = std::make_shared<ast4vx::Statements>(1, 2);
    ast_fun << assign(ast_fun->d_out(0), ast_fun->d_in(0) * (ast_fun->d_in(1) + ast_fun->d_in(0)));

    CPPVisitor v;

    //DomVX
    auto image_o = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto image_i_1 = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);
    auto image_i_2 = new HipaVX::Image(1024, 1024, VX_DF_IMAGE_U8);

    auto t = std::make_shared<DomVX::Map>();
    t->set_statements(ast_fun);
    t->register_images({image_o}, {image_i_1, image_i_2});

    std::cout << "AST Generation:\n";
    std::cout << v.visit(ast_fun, 0);
    std::cout << "\n";

    std::cout << "DomVX Map Generation:\n";
    std::cout << v.visit(t, 0);
    std::cout << "\n";

    return 0;
}
