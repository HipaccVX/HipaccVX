#include "../../../VX/vx.h"
#include "../../../hipaVX/domVX_types.hpp"
#include "../../../hipaVX/abstractions.hpp"
#include "../../../hipaVX/cpp_gen/cpp_gen.hpp"
#include <string>



/*
                                                        +-------------+                                                        +---------------+
                                                    +-->|window_1_op_1|---------------------------+                            |               |
                                +----------------+  |   +-------------+                           |   +-----------------+      |               |
+---------------+        +----->|window_op_forall|--+                                             +-->|local_to_pixel_op|----->|   IMAGE_O_1   |
|               |        |      +----------------+  |   +-------------+      +------------------+ |   +-----------------+      |               |
|               |        |                          +-->|window_1_op_2|----->|window_1_op_2_op_1|-+                            |               |
|    IMAGE_I    |--------+                              +-------------+      +------------------+                              +---------------+
|               |        |
|               |        |                                                                                                     +---------------+
+---------------+        |                                                                                                     |               |
                         |                                         +------------+                                              |               |
                         +---------------------------------------->|reduction_op|--------------------------------------------->|   IMAGE_O_2   |
                                                                   +------------+                                              |               |
                                                                                                                               |               |
                                                                                                                               +---------------+
*/
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

    auto ast_reduction = std::make_shared<ast4vx::Reduction>(ast4vx::Constant<int>(0));
    ast_reduction << assign(ast_reduction->out(), ast_reduction->left() + ast_reduction->right());

    auto l_to_p = std::make_shared<ast4vx::LocalToPixel>(1, 2);
    auto win_1 = l_to_p->window(0);
    auto win_2 = l_to_p->window(1);
    l_to_p << assign(l_to_p->d_out(0), win_1->pixel_at(0, 2) + win_2->pixel_at(2, 4));


    // Assigning the WindowOperations to functions and descriptors...
    auto window_in = std::make_shared<ast4vx::WindowDescriptor>(3, 5);

    auto window_op_forall = forall(window_in, ast_forall);

    auto window_1_op_1 = std::make_shared<ast4vx::WindowOperation>(3, 5);
    window_1_op_1->set_window_inputs({window_op_forall->get_window_output()});
    window_1_op_1->compute_at(2, 0, ast_fun_1);
    window_1_op_1->compute_at(0, 2, ast_fun_2);
    window_1_op_1->compute_at(2, 4, ast_fun_2);

    auto window_1_op_2 = std::make_shared<ast4vx::WindowOperation>(3, 5);
    window_1_op_2->set_window_inputs({window_op_forall->get_window_output()});
    window_1_op_2->compute_at(2, 0, ast_fun_1);
    window_1_op_2->compute_at(0, 2, ast_fun_1);
    window_1_op_2->compute_at(2, 4, ast_fun_1);


    auto window_1_op_2_op_1 = std::make_shared<ast4vx::WindowOperation>(3, 5);
    window_1_op_2_op_1->set_window_inputs({window_1_op_2->get_window_output()});
    window_1_op_2_op_1->compute_at(0, 2, ast_fun_1);

    auto local_to_pixel_op = std::make_shared<ast4vx::WindowOperation>(3, 5);
    local_to_pixel_op->set_window_inputs({window_1_op_1->get_window_output(), window_1_op_2_op_1->get_window_output()});
    local_to_pixel_op->to_pixel(l_to_p);

    auto reduction_op = reduce(window_in, ast_reduction);

    //---------------------------- DomVX --------------------------------------

    auto image_o_1 = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);
    auto image_o_2 = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);
    auto image_i = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);


    auto local_op = std::shared_ptr<DomVX::LocalOperation>(new DomVX::LocalOperation());

    auto dom_in = std::shared_ptr<DomVX::Domain>(new DomVX::Domain(3, 5, {0, 1, 1,
                                                                          0, 0, 0,
                                                                          1, 1, 0,
                                                                          0, 0, 0,
                                                                          0, 1, 1}));
    auto dom_desc_1 = std::shared_ptr<DomVX::Domain>(new DomVX::Domain(3, 5, {0, 0, 1,
                                                                              0, 0, 0,
                                                                              1, 0, 0,
                                                                              0, 0, 0,
                                                                              0, 0, 1}));
    auto dom_1_op_1_desc = std::shared_ptr<DomVX::Domain>(new DomVX::Domain(3, 5, {0, 0, 0,
                                                                                   0, 0, 0,
                                                                                   0, 0, 0,
                                                                                   0, 0, 0,
                                                                                   0, 0, 1}));
    auto dom_1_op_2_desc = std::shared_ptr<DomVX::Domain>(new DomVX::Domain(3, 5, {0, 0, 0,
                                                                                   0, 0, 0,
                                                                                   1, 0, 0,
                                                                                   0, 0, 0,
                                                                                   0, 0, 0}));

    local_op->set_input_window_desc({{image_i, window_in}});
    local_op->set_domains({{window_in, dom_in}, {window_op_forall->get_window_output(), dom_desc_1}, {window_1_op_1->get_window_output(), dom_1_op_1_desc}, {window_1_op_2->get_window_output(), dom_1_op_2_desc}});

    local_op->add_operation(window_op_forall);
    local_op->add_operation(window_1_op_1);
    local_op->add_operation(window_1_op_2);
    local_op->add_operation(window_1_op_2_op_1);
    local_op->add_operation(local_to_pixel_op, {image_o_1});
    local_op->add_operation(reduction_op, {image_o_2});

    CPPVisitor v;
    std::cout << v.visit(local_op) << "\n";

    return 0;
}

