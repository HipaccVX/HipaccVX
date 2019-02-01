//#include "../VX/vx.h"
//#include "ast.hpp"
//#include <string>
//#include "domVX_types.hpp"
//#include "domVX_kernels.hpp"
#include "domVX_kernels.hpp"

namespace HipaVX
{

FileinputImage::FileinputImage(vx_uint32 width, vx_uint32 height, vx_df_image color, std::string filename)
    :Image(width, height, color), file(filename)
{
}

WriteImageNode::WriteImageNode()
{
    node_name = "Image Writer";
}
std::vector<Object *> WriteImageNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}
std::vector<Object *> WriteImageNode::get_outputs()
{
    std::vector<Object*> used_objects;
    return used_objects;
}

Sobel3x3Node::Sobel3x3Node()
{
    node_name = "Sobel 3x3";
}
std::vector<Object *> Sobel3x3Node::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}
std::vector<Object *> Sobel3x3Node::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out_x);
    used_objects.emplace_back(out_y);
    return used_objects;
}
std::vector<Node*> Sobel3x3Node::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&sobel_x);
    subnodes.push_back(&sobel_y);
    return subnodes;
}
void Sobel3x3Node::build()
{
    sobel_x.mask.dim[0] = sobel_x.mask.dim[1] = 3;
    sobel_y.mask.dim[0] = sobel_y.mask.dim[1] = 3;

    sobel_x.mask.mask = {-1,  0,  1,
                    -2,  0,  2,
                    -1,  0,  1};

    sobel_y.mask.mask = {-1, -2, -1,
                     0,  0,  0,
                     1,  2,  1};

    uint32_t one = 1;
    Scalar* x_normalization = new Scalar(VX_TYPE_INT32, &one);
    Scalar* y_normalization = new Scalar(VX_TYPE_INT32, &one);
    sobel_x.normalization.reset(x_normalization);
    sobel_y.normalization.reset(y_normalization);

    sobel_x.in = in;
    sobel_x.out = out_x;
    sobel_y.in = in;
    sobel_y.out = out_y;

    sobel_x.build();
    sobel_y.build();
}

Add3_3::Add3_3()
{
    node_name = "Add 3x3";
}
std::vector<Object *> Add3_3::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}
std::vector<Object *> Add3_3::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}
std::vector<Node*> Add3_3::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&add);
    return subnodes;
}
void Add3_3::build()
{
    add.mask.dim[0] = add.mask.dim[1] = 3;

    add.mask.mask = {1, 1, 1,
                1, 1, 1,
                1, 1, 1};

    uint32_t one = 1;
    Scalar* normalization = new Scalar(VX_TYPE_INT32, &one);
    add.normalization.reset(normalization);

    add.in = in;
    add.out = out;

    add.build();
}

std::vector<Object *> HarrisCorners::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}

std::vector<Object *> HarrisCorners::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(&Vc);
    used_objects.emplace_back(corners);
    return used_objects;
}

std::vector<Node*> HarrisCorners::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&sobel);
    subnodes.push_back(&sobel_x_norm);
    subnodes.push_back(&sobel_y_norm);
    subnodes.push_back(&sobel_x_square);
    subnodes.push_back(&sobel_y_square);
    subnodes.push_back(&sobel_x_y);
    subnodes.push_back(&gx_square_A);
    subnodes.push_back(&gy_square_A);
    subnodes.push_back(&gx_gy_A);

    subnodes.push_back(&trace_add);
    subnodes.push_back(&gx_A_gy_A);
    subnodes.push_back(&gx_gy_A_square);
    subnodes.push_back(&det_kernel);
    subnodes.push_back(&trace_A_square_kernel);
    subnodes.push_back(&trace_A_square_k_kernel);
    subnodes.push_back(&Mc_kernel);
    return subnodes;
}
void HarrisCorners::build()
{
    sobel.in = in;
    sobel.out_x = &sob_x;
    sobel.out_y = &sob_y;

    sobel_x_norm.in = &sob_x;
    sobel_x_norm.out = &Gx;
    sobel_y_norm.in = &sob_y;
    sobel_y_norm.out = &Gy;
    sobel_x_norm.scalar = sobel_y_norm.scalar = 1.f / (4 * 255 * 3/*b*/); // For 3x3 Sobel

    sobel_x_square.in_1 = &Gx;
    sobel_x_square.in_2 = &Gx;
    sobel_x_square.out = &square_Gx;
    sobel_y_square.in_1 = &Gy;
    sobel_y_square.in_2 = &Gy;
    sobel_y_square.out = &square_Gy;

    sobel_x_y.in_1 = &Gx;
    sobel_x_y.in_2 = &Gy;
    sobel_x_y.out = &det_A_mul_Gx_Gy;

    gx_square_A.in = &square_Gx;
    gx_square_A.out = &square_Gx_sum;
    gy_square_A.in = &square_Gy;
    gy_square_A.out = &square_Gy_sum;
    gx_gy_A.in = &det_A_mul_Gx_Gy;
    gx_gy_A.out = &det_A_mul_Gx_Gy_sum;

    trace_add.in_1 = &square_Gx_sum;
    trace_add.in_2 = &square_Gy_sum;
    trace_add.out = &trace_A;

    gx_A_gy_A.in_1 = &square_Gx_sum;
    gx_A_gy_A.in_2 = &square_Gy_sum;
    gx_A_gy_A.out = &det_A_minuend;

    gx_gy_A_square.in_1 = &det_A_mul_Gx_Gy_sum;
    gx_gy_A_square.in_2 = &det_A_mul_Gx_Gy_sum;
    gx_gy_A_square.out = &det_A_subtrahend;

    det_kernel.in_1 = &det_A_minuend;
    det_kernel.in_2 = &det_A_subtrahend;
    det_kernel.out = &det_A;

    trace_A_square_kernel.in_1 = &trace_A;
    trace_A_square_kernel.in_2 = &trace_A;
    trace_A_square_kernel.out = &trace_A_square;

    trace_A_square_k_kernel.in = &trace_A_square;
    trace_A_square_k_kernel.scalar = sensitivity->f32;
    trace_A_square_k_kernel.out = &trace_A_square_k;

    Mc_kernel.in_1 = &det_A;
    Mc_kernel.in_2 = &trace_A_square_k;
    Mc_kernel.out = &Mc;

    sobel.build();
    sobel_x_norm.build();
    sobel_y_norm.build();
    sobel_x_square.build();
    sobel_y_square.build();
    sobel_x_y.build();
    gx_square_A.build();
    gy_square_A.build();
    gx_gy_A.build();
    trace_add.build();
    gx_A_gy_A.build();
    gx_gy_A_square.build();
    det_kernel.build();
    trace_A_square_kernel.build();
    trace_A_square_k_kernel.build();
    Mc_kernel.build();
}

VXConvolveNode::VXConvolveNode()
{
    node_name = "VX Convolution";
}
std::vector<Object *> VXConvolveNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    used_objects.emplace_back(convolution);
    return used_objects;
}
std::vector<Object *> VXConvolveNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}
std::vector<Node*> VXConvolveNode::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&lin_mask_node);
    if (out->col == VX_DF_IMAGE_U8)
        subnodes.push_back(&saturate_node);
    return subnodes;
}
void VXConvolveNode::build()
{
	lin_mask_node.in = in;
    lin_mask_node.mask.dim[0] = convolution->rows;
    lin_mask_node.mask.dim[1] = convolution->columns;
	lin_mask_node.mask.mask.resize(lin_mask_node.mask.dim[0] * lin_mask_node.mask.dim[1]);
    for (unsigned int i = 0; i < lin_mask_node.mask.dim[0] * lin_mask_node.mask.dim[1]; i++)
        lin_mask_node.mask.mask[i] = convolution->coefficients[i];

    float one = 1.f / convolution->scale;
    Scalar* normalization = new Scalar(VX_TYPE_FLOAT32, &one);
    lin_mask_node.normalization.reset(normalization);

    if (out->col != VX_DF_IMAGE_U8)
    {
        lin_mask_node.out = out;
    }
    else
    {
        lin_mask_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S16));
        lin_mask_node.out = lin_mask_image.get();

        saturate_node.in = lin_mask_image.get();
        saturate_node.out = out;
    }
    lin_mask_node.build();
    saturate_node.build();
}

HipaccNode::HipaccNode()
{
    node_name = "Custom Hipacc";
}
std::vector<Object *> HipaccNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.insert(used_objects.end(), parameters.begin(), parameters.end());
    return used_objects;
}
std::vector<Object *> HipaccNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

VXBilateralFilterNode::VXBilateralFilterNode()
{
    node_name = "Bilateral Filter";

    /*auto g_1 = new secret::Variable();
    g_1->name = "g_1";
    auto y = new secret::Variable();
    y->name = "y";

    auto g_2 = new secret::Variable();
    g_2->name = "g_2";
    auto t = new secret::Variable();
    t->name = "t";




    {
        auto zero = new secret::Constant<float>();
        zero->value = 0.f;
        auto one = new secret::Constant<float>();
        one->value = 1.f;
        auto two = new secret::Constant<float>();
        two->value = 2.f;
        auto one_half = new secret::Div();
        one_half->subnodes[0] = one;
        one_half->subnodes[1] = two;
        auto neg_one_half = new secret::Sub();
        neg_one_half->subnodes[0] = zero;
        neg_one_half->subnodes[1] = one_half;

        auto sigma_y = new secret::Variable();
        sigma_y->name = "sigma_y";
        auto y_square = new secret::Square();
        y_square->subnodes[0] = y;
        auto sigma_y_square = new secret::Square();
        sigma_y_square->subnodes[0] = sigma_y;
        auto some_div = new secret::Div();
        some_div->subnodes[0] = y_square;
        some_div->subnodes[1] = sigma_y_square;

        auto some_mul = new secret::Mul();
        some_mul->subnodes[0] = neg_one_half;
        some_mul->subnodes[1] = some_div;

        auto exp = new secret::Exp();
        exp->subnodes[0] = some_mul;

        auto pi = new secret::Constant<float>();
        pi->value = 3.14159f;

        auto some_mul_2 = new secret::Mul();
        some_mul_2->subnodes[0] = two;
        some_mul_2->subnodes[1] = pi;

        auto some_mul_3 = new secret::Mul();
        some_mul_3->subnodes[0] = some_mul_2;
        some_mul_3->subnodes[1] = sigma_y;

        auto sqrt = new secret::Sqrt();
        sqrt->subnodes[0] = some_mul_3;

        auto some_div_2 = new secret::Div();
        some_div_2->subnodes[0] = one;
        some_div_2->subnodes[1] = some_mul_3;

        auto last_mul = new secret::Mul();
        last_mul->subnodes[0] = some_div_2;
        last_mul->subnodes[1] = exp;


        auto assignment = new secret::Assignment();
        assignment->subnodes[0] = g_1;
        assignment->subnodes[1] = last_mul;

        auto g_1_def = new secret::VariableDefinition();
        g_1_def->subnodes[0] = g_1;
        g_1_def->datatype = secret::Datatype::FLOAT;

        kernel.function.statements.emplace_back(g_1_def);
        kernel.function.statements.emplace_back(assignment);
    }

    {
        auto zero = new secret::Constant<float>();
        zero->value = 0.f;
        auto one = new secret::Constant<float>();
        one->value = 1.f;
        auto two = new secret::Constant<float>();
        two->value = 2.f;
        auto one_half = new secret::Div();
        one_half->subnodes[0] = one;
        one_half->subnodes[1] = two;
        auto neg_one_half = new secret::Sub();
        neg_one_half->subnodes[0] = zero;
        neg_one_half->subnodes[1] = one_half;

        auto sigma_t = new secret::Variable();
        sigma_t->name = "sigma_t";
        auto y_square = new secret::Square();
        y_square->subnodes[0] = t;
        auto sigma_y_square = new secret::Square();
        sigma_y_square->subnodes[0] = sigma_t;
        auto some_div = new secret::Div();
        some_div->subnodes[0] = y_square;
        some_div->subnodes[1] = sigma_y_square;

        auto some_mul = new secret::Mul();
        some_mul->subnodes[0] = neg_one_half;
        some_mul->subnodes[1] = some_div;

        auto exp = new secret::Exp();
        exp->subnodes[0] = some_mul;

        auto pi = new secret::Constant<float>();
        pi->value = 3.14159f;

        auto some_mul_2 = new secret::Mul();
        some_mul_2->subnodes[0] = two;
        some_mul_2->subnodes[1] = pi;

        auto some_mul_3 = new secret::Mul();
        some_mul_3->subnodes[0] = some_mul_2;
        some_mul_3->subnodes[1] = sigma_t;

        auto sqrt = new secret::Sqrt();
        sqrt->subnodes[0] = some_mul_3;

        auto some_div_2 = new secret::Div();
        some_div_2->subnodes[0] = one;
        some_div_2->subnodes[1] = some_mul_3;

        auto last_mul = new secret::Mul();
        last_mul->subnodes[0] = some_div_2;
        last_mul->subnodes[1] = exp;


        auto assignment = new secret::Assignment();
        assignment->subnodes[0] = g_2;
        assignment->subnodes[1] = last_mul;

        auto g_2_def = new secret::VariableDefinition();
        g_2_def->subnodes[0] = g_2;
        g_2_def->datatype = secret::Datatype::FLOAT;

        kernel.function.statements.emplace_back(g_2_def);
        kernel.function.statements.emplace_back(assignment);
    }*/

}
std::vector<Object *> VXBilateralFilterNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}
std::vector<Object *> VXBilateralFilterNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

AnotherBilateralFilterNode::AnotherBilateralFilterNode()
{
    node_name = "Another Bilateral Filter";
}
std::vector<Object *> AnotherBilateralFilterNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}
std::vector<Object *> AnotherBilateralFilterNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}
void AnotherBilateralFilterNode::build()
{
    // this is a stencil function
    stencil = std::make_shared<function_ast::Stencil>();
    stencil->dim[0] = stencil->dim[1] = 5;
    stencil->mask = function_ast::Stencil::from_t<float>({
        0.018316f, 0.082085f, 0.135335f, 0.082085f, 0.018316f ,
        0.082085f, 0.367879f, 0.606531f, 0.367879f, 0.082085f ,
        0.135335f, 0.606531f, 1.000000f, 0.606531f, 0.135335f ,
        0.082085f, 0.367879f, 0.606531f, 0.367879f, 0.082085f ,
        0.018316f, 0.082085f, 0.135335f, 0.082085f, 0.018316f
    });
    stencil->name = "stencil";
    stencil->datatype = function_ast::Datatype::FLOAT;

    // this function operates on Images
    auto in_node = std::make_shared<function_ast::Image>(in);
    kernel.inputs.push_back(in_node);
    kernel.inputs.push_back(stencil);
    auto out_node = std::make_shared<function_ast::Image>(out);
    kernel.output = out_node;

    auto c_r = std::make_shared<function_ast::Variable>("c_r", function_ast::Datatype::FLOAT);
    auto d = std::make_shared<function_ast::Variable>("d", function_ast::Datatype::FLOAT);
    auto p = std::make_shared<function_ast::Variable>("p", function_ast::Datatype::FLOAT);
    auto center = std::make_shared<function_ast::Variable>("center", function_ast::Datatype::FLOAT);

    kernel.function << define(c_r)    << assign(c_r, constant(0.5f) / (constant(sigma_r) * constant(sigma_r)))
                     << define(d)      << assign(d, constant(0.f))
                     << define(p)      << assign(p, constant(0.f))
                     << define(center) << assign(center, current_pixel(in_node));

    auto iterate = std::make_shared<function_ast::IterateAroundPixel>();
    auto iterate_body = std::make_shared<function_ast::Statements>();
    {
        auto diff = std::make_shared<function_ast::Variable>("diff", function_ast::Datatype::FLOAT);
        auto s = std::make_shared<function_ast::Variable>("s", function_ast::Datatype::FLOAT);

        *iterate_body << define(diff) << assign(diff, iterate->pixel_value - center)
                      << define(s)    << assign(s, exp(constant(0.f) - c_r * (diff * diff)) * iterate->stencil_value)
                      << assign(d, d+s)
                      << assign(p, p + s * iterate->pixel_value);
    }

    iterate->subnodes[0] = in_node;
    iterate->subnodes[1] = stencil;
    iterate->subnodes[2] = iterate_body; //The body
    kernel.function << iterate;

    kernel.function << assign(target_pixel(out_node), convert(p / d + constant(0.5f), function_ast::Datatype::UINT8));
}
}
