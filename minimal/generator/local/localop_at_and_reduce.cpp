#include "../../../VX/vx.h"
#include "../../../hipaVX/domVX_types.hpp"
#include "../../../hipaVX/abstractions.hpp"
#include "../../../hipaVX/cpp_gen/cpp_gen.hpp"
#include <string>

int main()
{
    // Created two sample pixel to pixel functions for later use
    auto ast_fun_1 = std::make_shared<ast4vx::Statements>(1, 1);
    ast_fun_1 << assign(ast_fun_1->d_out(0), ast_fun_1->d_in(0) * constant(3.4f));

    auto ast_fun_2 = std::make_shared<ast4vx::Statements>(1, 0);
    ast_fun_2 << assign(ast_fun_2->d_out(0), sqrt(constant(2.f)));

    // Reduction AST expects an initial value for the accumulator
    auto ast_reduction = std::make_shared<ast4vx::Reduction>(ast4vx::Constant<int>(0));
    ast_reduction << assign(ast_reduction->out(), ast_reduction->left() + ast_reduction->right());


    // Create dummy input window and set its domain
    auto window_in = std::make_shared<ast4vx::WindowDescriptor>(3, 5);
    window_in->set_domain({0, 0, 1,
                           0, 0, 0,
                           1, 0, 0,
                           0, 0, 0,
                           0, 0, 1});

    // Associate pixel to pixel functions with the domain coordinates via compute_at
    // these pixel to pixel functions must have one output pixel accessor
    auto window_op_1 = std::make_shared<ast4vx::WindowOperation>();
    window_op_1->set_window_inputs({window_in});
    window_op_1->compute_at(2, 0, ast_fun_1);
    window_op_1->compute_at(0, 2, ast_fun_2);
    window_op_1->compute_at(2, 4, ast_fun_2);

    // Chain a reduce function
    // This is done via setting the window_op_1's output to this operations input
    auto reduction_op = std::make_shared<ast4vx::WindowOperation>();
    reduction_op->set_window_inputs({window_op_1->get_window_output()});
    reduction_op->reduce(ast_reduction);

    //---------------------------- DomVX --------------------------------------

    auto image_i = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);
    auto image_o = new HipaVX::Image(1024, 512, VX_DF_IMAGE_U8);

    // Create the local operation
    auto local_op = std::shared_ptr<DomVX::LocalOperation>(new DomVX::LocalOperation());

    // Bind the dummy window accessors to the input images
    local_op->set_input_window_desc({{image_i, window_in}});

    // Add the Pixel to Pixel compute_at operation
    local_op->add_operation(window_op_1);
    // Add the reduction operation, needs an output image
    local_op->add_operation(reduction_op, {image_o});

    CPPVisitor v;
    std::cout << v.visit(local_op) << "\n";

    return 0;
}

