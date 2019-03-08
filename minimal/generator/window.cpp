#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../../hipaVX/cpp_gen/cpp_gen.hpp"
#include <string>

int main()
{
    //---------------------------- AST ----------------------------------------

    //AST Functions, Statements, L2P, P2P,..
    auto ast_fun_1 = std::make_shared<ast4vx::Statements>(1, 1);
    ast_fun_1 << assign(ast_fun_1->d_out(0), ast_fun_1->d_in(0) * constant(3.4f));

    auto ast_fun_2 = std::make_shared<ast4vx::Statements>(1, 0);
    ast_fun_2 << assign(ast_fun_2->d_out(0), sqrt(constant(2.f)));

    auto ast_forall = std::make_shared<ast4vx::Statements>(1, 1);
    ast_forall << assign(ast_forall->d_out(0), ast_forall->d_in(0) * ast_forall->d_in(0));

    auto l_to_p = std::make_shared<ast4vx::LocalToPixel>(1, 0, 2);
    auto win_1 = l_to_p->window(0);
    auto win_2 = l_to_p->window(1);
    l_to_p << assign(l_to_p->d_out(0), win_1->pixel_at(0, 2) + win_2->pixel_at(2, 4));


    // Assigning the functions etc to window_1

    auto window_in = std::make_shared<ast4vx::WindowDescriptor>(3, 5);
    window_in->set_domain({0, 1, 1,
                           0, 0, 0,
                           1, 1, 0,
                           0, 0, 0,
                           0, 1, 1});

    auto window_op_forall = std::make_shared<ast4vx::WindowOperation>(3, 5);
    window_op_forall->set_window_inputs({window_in});
    window_op_forall->forall(ast_forall);

    auto window_desc_1 = window_op_forall->get_window_output();
    window_desc_1->set_domain({0, 0, 1,
                               0, 0, 0,
                               1, 0, 0,
                               0, 0, 0,
                               0, 0, 1});

    auto window_1_op_1 = std::make_shared<ast4vx::WindowOperation>(3, 5);
    window_1_op_1->set_window_inputs({window_desc_1});
    window_1_op_1->compute_at(2, 0, ast_fun_1);
    window_1_op_1->compute_at(0, 2, ast_fun_2);
    window_1_op_1->compute_at(2, 4, ast_fun_2);

    auto window_1_op_2 = std::make_shared<ast4vx::WindowOperation>(3, 5);
    window_1_op_2->set_window_inputs({window_desc_1});
    window_1_op_2->compute_at(2, 0, ast_fun_1);
    window_1_op_2->compute_at(0, 2, ast_fun_1);
    window_1_op_2->compute_at(2, 4, ast_fun_1);


    auto window_1_op_1_desc = window_1_op_1->get_window_output();
    window_1_op_1_desc->set_domain({0, 0, 0,
                                    0, 0, 0,
                                    0, 0, 0,
                                    0, 0, 0,
                                    0, 0, 1});
    auto window_1_op_2_desc = window_1_op_2->get_window_output();
    window_1_op_2_desc->set_domain({0, 0, 0,
                                    0, 0, 0,
                                    1, 0, 0,
                                    0, 0, 0,
                                    0, 0, 0});

    auto window_1_op_2_op_1 = std::make_shared<ast4vx::WindowOperation>(3, 5);
    window_1_op_2_op_1->set_window_inputs({window_1_op_2_desc});
    window_1_op_2_op_1->compute_at(0, 2, ast_fun_1);

    auto window_1_op_2_op1_desc = window_1_op_2_op_1->get_window_output();
    window_1_op_2_op1_desc->set_domain({0, 0, 0,
                                        0, 0, 0,
                                        1, 0, 0,
                                        0, 0, 0,
                                        0, 0, 0});

    // Get output of window_1 and call it window_2, where it gets "reduced" down to pixel
    auto last_op = std::make_shared<ast4vx::WindowOperation>(3, 5);
    last_op->set_window_inputs({window_1_op_1_desc, window_1_op_2_op1_desc});
    last_op->to_pixel(l_to_p);

    //---------------------------- HipaVX -------------------------------------
    //Create HipaVX input and outputs

    auto image_o = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);
    auto image_i = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);


    //---------------------------- DomVX --------------------------------------
    auto local_op = std::shared_ptr<DomVX::LocalOperation>(new DomVX::LocalOperation());

    local_op->set_input_window_desc({{image_i, window_in}});

    local_op->add_operation(window_op_forall, {}, {image_i});
    local_op->add_operation(window_1_op_1, {}, {});
    local_op->add_operation(window_1_op_2, {}, {});
    local_op->add_operation(window_1_op_2_op_1, {}, {});
    local_op->add_operation(last_op, {image_o}, {});

    CPPVisitor v;
    std::cout << v.visit(local_op) << "\n";

    return 0;
}

