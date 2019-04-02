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
void WriteImageNode::build()
{
    inputs.emplace_back(in);
}

Sobel3x3Node::Sobel3x3Node()
{
    node_name = "Sobel 3x3";
}
void Sobel3x3Node::build()
{
    sobel_x.mask.dim[0] = sobel_x.mask.dim[1] = 3;
    sobel_y.mask.dim[0] = sobel_y.mask.dim[1] = 3;

    sobel_x.mask.matrix = {-1,  0,  1,
                    -2,  0,  2,
                    -1,  0,  1};

    sobel_y.mask.matrix = {-1, -2, -1,
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

    subnodes.push_back(&sobel_x);
    subnodes.push_back(&sobel_y);

    inputs.emplace_back(in);
    outputs.emplace_back(out_x);
    outputs.emplace_back(out_y);
}

Add3_3::Add3_3()
{
    node_name = "Add 3x3";
}
void Add3_3::build()
{
    add.mask.dim[0] = add.mask.dim[1] = 3;

    add.mask.matrix = {1, 1, 1,
                1, 1, 1,
                1, 1, 1};

    uint32_t one = 1;
    Scalar* normalization = new Scalar(VX_TYPE_INT32, &one);
    add.normalization.reset(normalization);

    add.in = in;
    add.out = out;

    add.build();

    subnodes.push_back(&add);

    inputs.emplace_back(in);
    outputs.emplace_back(out);
}

HarrisCorners::HarrisCorners(Image *in)
    :in(in),
      sob_x(in->w, in->h, VX_TYPE_FLOAT32),
      sob_y(in->w, in->h, VX_TYPE_FLOAT32),
      Gx(in->w, in->h, VX_TYPE_FLOAT32),
      Gy(in->w, in->h, VX_TYPE_FLOAT32),
      square_Gx(in->w, in->h, VX_TYPE_FLOAT32),
      square_Gy(in->w, in->h, VX_TYPE_FLOAT32),
      square_Gx_sum(in->w, in->h, VX_TYPE_FLOAT32),
      square_Gy_sum(in->w, in->h, VX_TYPE_FLOAT32),
      trace_A(in->w, in->h, VX_TYPE_FLOAT32),
      det_A_minuend(in->w, in->h, VX_TYPE_FLOAT32),
      det_A_mul_Gx_Gy(in->w, in->h, VX_TYPE_FLOAT32),
      det_A_mul_Gx_Gy_sum(in->w, in->h, VX_TYPE_FLOAT32),
      det_A_subtrahend(in->w, in->h, VX_TYPE_FLOAT32),
      det_A(in->w, in->h, VX_TYPE_FLOAT32),
      trace_A_square(in->w, in->h, VX_TYPE_FLOAT32),
      trace_A_square_k(in->w, in->h, VX_TYPE_FLOAT32),
      Mc(in->w, in->h, VX_TYPE_FLOAT32),
      Vc(in->w, in->h, VX_TYPE_FLOAT32)
{
    node_name = "Harris Corner";
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


    inputs.emplace_back(in);
    outputs.emplace_back(&Vc);
    outputs.emplace_back(corners);
}

VXConvolveNode::VXConvolveNode()
{
    node_name = "VX Convolution";
}
void VXConvolveNode::build()
{
	lin_mask_node.in = in;
    lin_mask_node.mask.dim[0] = convolution->rows;
    lin_mask_node.mask.dim[1] = convolution->columns;
	lin_mask_node.mask.matrix.resize(lin_mask_node.mask.dim[0] * lin_mask_node.mask.dim[1]);
    for (unsigned int i = 0; i < lin_mask_node.mask.dim[0] * lin_mask_node.mask.dim[1]; i++)
        lin_mask_node.mask.matrix[i] = convolution->coefficients[i];

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


    subnodes.push_back(&lin_mask_node);
    if (out->col == VX_DF_IMAGE_U8)
        subnodes.push_back(&saturate_node);

    inputs.emplace_back(in);
    inputs.emplace_back(convolution);
    outputs.emplace_back(out);
}

HipaccNode::HipaccNode()
{
    node_name = "Custom Hipacc";

}
void HipaccNode::build()
{
    inputs.insert(inputs.end(), parameters.begin(), parameters.end());
    outputs.emplace_back(out);
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
    }

    inputs.emplace_back(in);
    outputs.emplace_back(out);
*/

}

AnotherBilateralFilterNode::AnotherBilateralFilterNode()
{
    node_name = "Another Bilateral Filter";
}
void AnotherBilateralFilterNode::build()
{
    // this is a stencil function
    stencil = std::make_shared<ast4vx::Stencil>();
    stencil->dim[0] = stencil->dim[1] = 5;
    stencil->mask = ast4vx::Stencil::from_t<float>({
        0.018316f, 0.082085f, 0.135335f, 0.082085f, 0.018316f ,
        0.082085f, 0.367879f, 0.606531f, 0.367879f, 0.082085f ,
        0.135335f, 0.606531f, 1.000000f, 0.606531f, 0.135335f ,
        0.082085f, 0.367879f, 0.606531f, 0.367879f, 0.082085f ,
        0.018316f, 0.082085f, 0.135335f, 0.082085f, 0.018316f
    });
    stencil->name = "stencil";
    stencil->datatype = ast4vx::Datatype::FLOAT;

    // this function operates on Images
    auto in_node = std::make_shared<ast4vx::Image>(in);
    kernel->inputs.push_back(in_node);
    kernel->inputs.push_back(stencil);
    auto out_node = std::make_shared<ast4vx::Image>(out);
    kernel->output = out_node;

    auto c_r = std::make_shared<ast4vx::Variable>("c_r", ast4vx::Datatype::FLOAT);
    auto d = std::make_shared<ast4vx::Variable>("d", ast4vx::Datatype::FLOAT);
    auto p = std::make_shared<ast4vx::Variable>("p", ast4vx::Datatype::FLOAT);
    auto center = std::make_shared<ast4vx::Variable>("center", ast4vx::Datatype::FLOAT);

    kernel->function << define(c_r)    << assign(c_r, constant(0.5f) / (constant(sigma_r) * constant(sigma_r)))
                     << define(d)      << assign(d, constant(0.f))
                     << define(p)      << assign(p, constant(0.f))
                     << define(center) << assign(center, current_pixel(in_node));

    auto iterate = std::make_shared<ast4vx::IterateAroundPixel>();
    auto iterate_body = std::make_shared<ast4vx::Statements>();
    {
        auto diff = std::make_shared<ast4vx::Variable>("diff", ast4vx::Datatype::FLOAT);
        auto s = std::make_shared<ast4vx::Variable>("s", ast4vx::Datatype::FLOAT);

        *iterate_body << define(diff) << assign(diff, iterate->pixel_value - center)
                      << define(s)    << assign(s, exp(constant(0.f) - c_r * (diff * diff)) * iterate->stencil_value)
                      << assign(d, d+s)
                      << assign(p, p + s * iterate->pixel_value);
    }

    iterate->subnodes[0] = in_node;
    iterate->subnodes[1] = stencil;
    iterate->subnodes[2] = iterate_body; //The body
    kernel->function << iterate;

    kernel->function << assign(target_pixel(out_node), convert(p / d + constant(0.5f), ast4vx::Datatype::UINT8));


    inputs.emplace_back(in);
    outputs.emplace_back(out);
}
}
