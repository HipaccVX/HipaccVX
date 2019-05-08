#include "../../VX/vx.h"
#include "../../hipaVX/domVX_types.hpp"
#include "../../hipaVX/abstractions.hpp"
#include "../../hipaVX/cpp_gen/cpp_gen.hpp"
#include "../../hipaVX/domVX_api.hpp"
#include <string>

int main()
{
    std::vector<std::shared_ptr<DomVX::AbstractionNode>> nodes;

    auto image_i = new DomVX::Image(1024, 512, VX_DF_IMAGE_U8);

    auto image_sob_x_unnorm = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);
    auto image_sob_x = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);
    auto image_sob_y_unnorm = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);
    auto image_sob_y = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);

    auto image_trace_x = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);
    auto image_trace_y = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);
    auto image_trace = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);

    auto image_det_x = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);
    auto image_det_y = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);
    auto image_det_xy = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);
    auto image_det = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);

    auto image_mc = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);

    auto image_vc = new DomVX::Image(1024, 512, VX_TYPE_FLOAT32);


    auto mask_x = create_mask(3, 3, {-1,  0,  1,
                                     -2,  0,  2,
                                     -1,  0,  1});
    auto mask_y = create_mask(3, 3, {-1, -2, -1,
                                      0,  0,  0,
                                      1,  2,  1});
    auto dom_x = create_dom(mask_x);
    auto dom_y = create_dom(mask_y);
    auto dom_a = create_dom(3, 3, {1, 1, 1,
                                   1, 1, 1,
                                   1, 1, 1});

    float k = 1.f; //Don't know what value to give
    auto scalar_k = new DomVX::Scalar(DomVX::ObjectType::VX_TYPE_FLOAT32, &k);
    float tc = 200.f; //Don't know what value to give
    auto scalar_tc = new DomVX::Scalar(DomVX::ObjectType::VX_TYPE_FLOAT32, &tc);


    // Sobel
    {
        //--- AST ---
        auto sobel_ast_forall = create_p2p_mask(1, 1, 1);
        sobel_ast_forall << assign(sobel_ast_forall->d_out(0), sobel_ast_forall->d_in(0) * sobel_ast_forall->m_in(0));

        auto sobel_ast_reduce = create_reduction(ast4vx::Constant<float>(0.f));
        sobel_ast_reduce << assign(sobel_ast_reduce->out(), sobel_ast_reduce->left() + sobel_ast_reduce->right());

        auto win_in = create_window_desc(3, 3);
        auto forall_op = forall(win_in, sobel_ast_forall);
        auto reduce_op = reduce(forall_op, sobel_ast_reduce);

        auto sobel_ast_normalize = create_p2p(1, 1);
        sobel_ast_normalize << assign(sobel_ast_normalize->d_out(0), sobel_ast_normalize->d_in(0) * constant(1.f/(4*255*3)));

        //--- DomVX ---
        auto sobel_x_local_op = create_local_op();
        sobel_x_local_op->set_input_window_desc({{image_i, win_in}});
        sobel_x_local_op->set_domains({{win_in, dom_x}});
        sobel_x_local_op->set_masks({{forall_op, mask_x}});
        sobel_x_local_op->add_operation(forall_op);
        sobel_x_local_op->add_operation(reduce_op, {image_sob_x_unnorm});
        auto sobel_x_point_op = create_point_op();
        sobel_x_point_op->set_statements(sobel_ast_normalize);
        sobel_x_point_op->register_images({image_sob_x}, {image_sob_x_unnorm});

        auto sobel_y_local_op = create_local_op();
        sobel_y_local_op->set_input_window_desc({{image_i, win_in}});
        sobel_y_local_op->set_domains({{win_in, dom_y}});
        sobel_y_local_op->set_masks({{forall_op, mask_y}});
        sobel_y_local_op->add_operation(forall_op);
        sobel_y_local_op->add_operation(reduce_op, {image_sob_y_unnorm});
        auto sobel_y_point_op = create_point_op();
        sobel_y_point_op->set_statements(sobel_ast_normalize);
        sobel_y_point_op->register_images({image_sob_y}, {image_sob_y_unnorm});

        nodes.emplace_back(sobel_x_local_op);
        nodes.emplace_back(sobel_x_point_op);
        nodes.emplace_back(sobel_y_local_op);
        nodes.emplace_back(sobel_y_point_op);
    }
    // trace(a)
    {
        //--- AST ---
        auto trace_forall = create_p2p(1, 1);
        trace_forall << assign(trace_forall->d_out(0), trace_forall->d_in(0) * trace_forall->d_in(0));
        auto trace_reduce = create_reduction(ast4vx::Constant<float>(0.f));
        trace_reduce << assign(trace_reduce->out(), trace_reduce->left() + trace_reduce->right());

        auto win_in = create_window_desc(3, 3);
        auto forall_op = forall(win_in, trace_forall);
        auto reduce_op = reduce(forall_op, trace_reduce);

        auto trace_add = create_p2p(1, 2);
        trace_add << assign(trace_add->d_out(0), trace_add->d_in(0) + trace_add->d_in(1));

        //--- DomVX ---
        auto trace_x_local_op = create_local_op();
        trace_x_local_op->set_input_window_desc({{image_sob_x, win_in}});
        trace_x_local_op->set_domains({{win_in, dom_a}});
        trace_x_local_op->add_operation(forall_op);
        trace_x_local_op->add_operation(reduce_op, {image_trace_x});

        auto trace_y_local_op = create_local_op();
        trace_y_local_op->set_input_window_desc({{image_sob_y, win_in}});
        trace_y_local_op->set_domains({{win_in, dom_a}});
        trace_y_local_op->add_operation(forall_op);
        trace_y_local_op->add_operation(reduce_op, {image_trace_y});

        auto treace_point_op = create_point_op();
        treace_point_op->set_statements(trace_add);
        treace_point_op->register_images({image_trace}, {image_trace_x, image_sob_y});

        nodes.emplace_back(trace_x_local_op);
        nodes.emplace_back(trace_y_local_op);
        nodes.emplace_back(treace_point_op);
    }
    // det(a)
    {
        //--- AST ---
        auto det_square = create_p2p(1, 1);
        det_square << assign(det_square->d_out(0), det_square->d_in(0) * det_square->d_in(0));

        auto det_mult = create_p2p(1, 2);
        det_mult << assign(det_mult->d_out(0), det_mult->d_in(0) * det_mult->d_in(1));

        auto det_reduce = create_reduction(ast4vx::Constant<float>(0.f));
        det_reduce << assign(det_reduce->out(), det_reduce->left() + det_reduce->right());

        auto win_in = create_window_desc(3, 3);
        auto win_in_2 = create_window_desc(3, 3);

        auto forall_op = forall(win_in, det_square);
        auto reduce_op = reduce(forall_op, det_reduce);

        auto forall_op_xy = forall({win_in, win_in_2}, det_mult);
        auto reduce_op_xy = reduce(forall_op_xy, det_reduce);

        auto det_point_ast = create_p2p(1, 3);
        det_point_ast << assign(det_point_ast->d_out(0), det_point_ast->d_in(0) * det_point_ast->d_in(1)
                                                       - det_point_ast->d_in(2) * det_point_ast->d_in(2));

        //--- DomVX ---
        auto det_x_local_op = create_local_op();
        det_x_local_op->set_input_window_desc({{image_sob_x, win_in}});
        det_x_local_op->set_domains({{win_in, dom_a}});
        det_x_local_op->add_operation(forall_op);
        det_x_local_op->add_operation(reduce_op, {image_det_x});

        auto det_y_local_op = create_local_op();
        det_y_local_op->set_input_window_desc({{image_sob_y, win_in}});
        det_y_local_op->set_domains({{win_in, dom_a}});
        det_y_local_op->add_operation(forall_op);
        det_y_local_op->add_operation(reduce_op, {image_det_y});

        auto det_xy_local_op = create_local_op();
        det_xy_local_op->set_input_window_desc({{image_sob_x, win_in}, {image_sob_y, win_in_2}});
        det_xy_local_op->set_domains({{win_in, dom_a}});
        det_xy_local_op->add_operation(forall_op_xy);
        det_xy_local_op->add_operation(reduce_op_xy, {image_det_xy});

        auto det_point_op = create_point_op();
        det_point_op->set_statements(det_point_ast);
        det_point_op->register_images({image_det}, {image_det_x, image_det_y, image_det_xy});

        nodes.emplace_back(det_x_local_op);
        nodes.emplace_back(det_y_local_op);
        nodes.emplace_back(det_xy_local_op);
        nodes.emplace_back(det_point_op);
    }
    // mc
    {
        auto mc_point_ast = create_p2p(1, 2);
        mc_point_ast->set_variable_inout_count(0, 1);
        mc_point_ast << assign(mc_point_ast->d_out(0), mc_point_ast->d_in(0) - mc_point_ast->v_in(0) * mc_point_ast->d_in(1) * mc_point_ast->d_in(1));

        auto mc_point_op = create_point_op();
        mc_point_op->set_statements(mc_point_ast);
        mc_point_op->register_images({image_mc}, {image_det, image_trace});
        mc_point_op->register_variables({}, {scalar_k});

        nodes.emplace_back(mc_point_op);
    }
    // vc
    {
        auto vc_point_ast = create_p2p(1, 1);
        vc_point_ast->set_variable_inout_count(0, 1);
        auto if_greater = IF(greater(vc_point_ast->d_in(0), vc_point_ast->v_in(0)));
        if_greater << assign(vc_point_ast->d_out(0), vc_point_ast->d_in(0));
        auto else_greater = ELSE();
        else_greater << assign(vc_point_ast->d_out(0), constant(0.f));
        vc_point_ast << if_greater << else_greater;

        auto vc_point_op = create_point_op();
        vc_point_op->set_statements(vc_point_ast);
        vc_point_op->register_images({image_vc}, {image_mc});
        vc_point_op->register_variables({}, {scalar_tc});

        nodes.emplace_back(vc_point_op);
    }


    CPPVisitor v;
    for (auto node: nodes)
        std::cout << v.visit(node) << "\n";

    return 0;
}

