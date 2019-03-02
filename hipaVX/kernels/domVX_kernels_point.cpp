//#include "../VX/vx.h"
//#include "ast.hpp"
//#include <string>
//#include "domVX_types.hpp"
//#include "domVX_kernels.hpp"
#include "domVX_kernels_point.hpp"

namespace HipaVX
{

// ----------- Single Operation Unary (1to1) Point Operators ----------------

void SimplePointUnary::build()
{
	auto in_node = std::make_shared<function_ast::Image>(in);
    kernel->inputs.push_back(in_node);
	auto out_node = std::make_shared<function_ast::Image>(out);
    kernel->output = out_node;

	switch(operation)
	{
	case function_ast::NodeType::Sqrt:
        kernel->function << assign(target_pixel(out_node), sqrt(current_pixel(in_node)));
		break;
    case function_ast::NodeType::Abs:
        kernel->function << assign(target_pixel(out_node), abs(current_pixel(in_node)));
        break;
	case function_ast::NodeType::Atan2:
        kernel->function << assign(target_pixel(out_node), atan2(current_pixel(in_node)));
		break;
	case function_ast::NodeType::Not:
        kernel->function << assign(target_pixel(out_node), ~current_pixel(in_node));
		break;
	case function_ast::NodeType::Mul: // Unary node Mul is "square"
        kernel->function << assign(target_pixel(out_node), current_pixel(in_node) * current_pixel(in_node));
		break;
	}
    inputs.emplace_back(in);
    outputs.emplace_back(out);
}

// ----------- Single Operation Binary (2to1) Point Operators ----------------
void SimplePointBinary::build()
{
	auto in_node_1 = std::make_shared<function_ast::Image>(in_1);
	auto in_node_2 = std::make_shared<function_ast::Image>(in_2);
    kernel->inputs.push_back(in_node_1);
    kernel->inputs.push_back(in_node_2);
	auto out_node = std::make_shared<function_ast::Image>(out);
    kernel->output = out_node;

	switch(operation)
	{
	case function_ast::NodeType::Add:
        kernel->function << assign(target_pixel(out_node), current_pixel(in_node_1) + current_pixel(in_node_2));
		break;
	case function_ast::NodeType::Sub:
        kernel->function << assign(target_pixel(out_node), current_pixel(in_node_1) - current_pixel(in_node_2));
		break;
	case function_ast::NodeType::Mul:
        kernel->function << assign(target_pixel(out_node), current_pixel(in_node_1) * current_pixel(in_node_2));
		break;
	case function_ast::NodeType::Div:
        kernel->function << assign(target_pixel(out_node), current_pixel(in_node_1) / current_pixel(in_node_2));
		break;
	case function_ast::NodeType::BitwiseAnd:
        kernel->function << assign(target_pixel(out_node), current_pixel(in_node_1) & current_pixel(in_node_2));
		break;
	case function_ast::NodeType::BitwiseOr:
        kernel->function << assign(target_pixel(out_node), current_pixel(in_node_1) | current_pixel(in_node_2));
		break;
	case function_ast::NodeType::BitwiseXor:
        kernel->function << assign(target_pixel(out_node), current_pixel(in_node_1) ^ current_pixel(in_node_2));
		break;
    }

    inputs.emplace_back(in_1);
    inputs.emplace_back(in_2);
    outputs.emplace_back(out);
}

SaturateNode::SaturateNode()
{
    node_name = "Saturate";
}

void SaturateNode::build()
{
	auto in_node = std::make_shared<function_ast::Image>(in);
    kernel->inputs.push_back(in_node);
	auto out_node = std::make_shared<function_ast::Image>(out);
    kernel->output = out_node;

	auto temp = std::make_shared<function_ast::Variable>("temp", function_ast::Datatype::INT32);
    kernel->function << define(temp) << assign(temp, current_pixel(in_node));

	std::shared_ptr<function_ast::Node> min, max;

	if (out->col == VX_DF_IMAGE_U8)
	{
		min = constant<int>(0);
		max = constant<int>(255);
	}
    // TODO: Add other datatypes
	else
	{
		min = constant<int>(-32768);
		max = constant<int>(32767);
	}

	auto greater_if = IF(greater(temp, max));
	greater_if->body << assign(temp, max);
	auto greater_else = ELSE();
	auto less_if = IF(less(temp, min));
	less_if->body << assign(temp, min);
	greater_else->body << less_if;

    kernel->function << greater_if;
    kernel->function << greater_else;

    kernel->function << assign(target_pixel(out_node), temp);

    inputs.emplace_back(in);
    outputs.emplace_back(out);
}

ConvertDepthNode::ConvertDepthNode()
{
    node_name = "Depth Converter";
}

void ConvertDepthNode::build() {
	auto in_node = std::make_shared<function_ast::Image>(in);
    kernel->inputs.push_back(in_node);
	auto out_node = std::make_shared<function_ast::Image>(out);
    kernel->output = out_node;

    // Shift right
	auto temp = std::make_shared<function_ast::Variable>("value", function_ast::Datatype::INT16); // TODO: datatype
	auto shift_c = constant<>(shift->i32);
    kernel->function << define(temp) << assign(temp, current_pixel(in_node) >> shift_c);

	std::shared_ptr<function_ast::Node> min = constant<int>(0);
	std::shared_ptr<function_ast::Node> max = constant<int>(255);

    // max/min overflow
	auto less_if = IF(less(temp, min));
	     less_if->body << assign(temp, min);

	auto greater_if = IF(greater(temp, max));
	     greater_if->body << assign(temp, max);
	auto greater_else = ELSE();
	     greater_else->body << less_if;

    kernel->function << greater_if;
    kernel->function << greater_else;

    kernel->function << assign(target_pixel(out_node), temp);

    inputs.emplace_back(in);
    outputs.emplace_back(out);
}

VXSubtractNode::VXSubtractNode()
{
    node_name = "VX Subtraction Node";
}

void VXSubtractNode::build()
{
    diff_node.in_1 = in_1;
    diff_node.in_2 = in_2;
    if (policy != VX_CONVERT_POLICY_SATURATE)
    {
        diff_node.out = out;
    }
    else
    {
        diff_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
        diff_node.out = diff_image.get();

        saturate_node.in = diff_image.get();
        saturate_node.out = out;
    }

    diff_node.build();
    if (policy == VX_CONVERT_POLICY_SATURATE)
        saturate_node.build();

    subnodes.push_back(&diff_node);
    if (policy == VX_CONVERT_POLICY_SATURATE)
        subnodes.push_back(&saturate_node);

    inputs.emplace_back(in_1);
    inputs.emplace_back(in_2);
    outputs.emplace_back(out);
}

VXThresholdNode::VXThresholdNode()
{
    node_name = "VX Threshold";
}

void VXThresholdNode::build()
{
	auto in_node = std::make_shared<function_ast::Image>(in);
    kernel->inputs.push_back(in_node);
	auto out_node = std::make_shared<function_ast::Image>(out);
    kernel->output = out_node;

    // buffer input image
	// auto in_d = std::make_shared<function_ast::Variable>("in_d", in_node->type); //TODO: how to gen in dtype? function_ast::Datatype::INT16);
    // kernel->function << define(in_d) << assign(in_d, current_pixel(in_node));

    // TODO: Even move the outer selection to kernels_point.cpp
    if (threshold->threshold_type == VX_THRESHOLD_TYPE_BINARY)
    {
	    auto if_less = IF(less(current_pixel(in_node), constant<>(threshold->value)));
             if_less->body << assign(target_pixel(out_node), constant<>(threshold->true_value));
	    auto else_less = ELSE();
	         else_less->body << assign(target_pixel(out_node), constant<>(threshold->true_value));
        kernel->function << if_less;
        kernel->function << else_less;
    }
    else
    {
	    auto if_outrange = IF(greater(current_pixel(in_node), constant<>(threshold->upper)) || less(current_pixel(in_node), constant<>(threshold->lower)));
	         if_outrange->body << assign(target_pixel(out_node), constant<>(threshold->false_value));
	    auto else_outrange = ELSE();
	         else_outrange->body << assign(target_pixel(out_node), constant<>(threshold->true_value));
        kernel->function << if_outrange;
        kernel->function << else_outrange;
    }

    inputs.emplace_back(in);
    outputs.emplace_back(out);
}

AbsDiffNode::AbsDiffNode()
{
    node_name = "Absolute Difference";
}

void AbsDiffNode::build()
{
    //saturate_node is only used when input is S16
    saturate = in_1->col == VX_DF_IMAGE_S16 || in_2->col == VX_DF_IMAGE_S16;

    diff_node.in_1 = in_1;
    diff_node.in_2 = in_2;
    diff_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
    diff_node.out = diff_image.get();

    abs_node.in = diff_image.get();
    if (!saturate)
    {
        abs_node.out = out;
    }
    else
    {
        abs_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
        abs_node.out = abs_image.get();

        saturate_node.in = abs_image.get();
        saturate_node.out = out;
    }

    diff_node.build();
    abs_node.build();
    if (saturate)
        saturate_node.build();

    subnodes.push_back(&diff_node);
    subnodes.push_back(&abs_node);
    if (saturate)
        subnodes.push_back(&saturate_node);

    inputs.emplace_back(in_1);
    inputs.emplace_back(in_2);
    outputs.emplace_back(out);
}

VXAddNode::VXAddNode()
{
    node_name = "VX Addition Node";
}

void VXAddNode::build()
{
    add_node.in_1 = in_1;
    add_node.in_2 = in_2;
    if (policy != VX_CONVERT_POLICY_SATURATE)
    {
        add_node.out = out;
    }
    else
    {
        add_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
        add_node.out = add_image.get();

        saturate_node.in = add_image.get();
        saturate_node.out = out;
    }

    add_node.build();
    if (policy == VX_CONVERT_POLICY_SATURATE)
        saturate_node.build();

    subnodes.push_back(&add_node);
    if (policy == VX_CONVERT_POLICY_SATURATE)
        subnodes.push_back(&saturate_node);

    inputs.emplace_back(in_1);
    inputs.emplace_back(in_2);
    outputs.emplace_back(out);
}

PhaseNode::PhaseNode()
{
    node_name = "VX Phase";
}

void PhaseNode::build()
{
    div_node.in_1 = in_1;
    div_node.in_2 = in_2;
    div_image.reset(new Image(in_1->w, in_2->h, VX_TYPE_FLOAT32));
    div_node.out = div_image.get();

    atan2_node.in = div_image.get();
    atan2_image.reset(new Image(in_1->w, in_2->h, VX_TYPE_FLOAT32));
    atan2_node.out = atan2_image.get();

    mapping_node.in = atan2_image.get();
    mapping_node.scalar = 255.f / (M_2_PI);
    mapping_node.out = out;

    div_node.build();
    atan2_node.build();
    mapping_node.build();

    subnodes.push_back(&div_node);
    subnodes.push_back(&atan2_node);
    subnodes.push_back(&mapping_node);

    inputs.emplace_back(in_1);
    inputs.emplace_back(in_2);
    outputs.emplace_back(out);
}

VXMultiplyNode::VXMultiplyNode()
{
    node_name = "VX Multiplication";
}

void VXMultiplyNode::build()
{
    mul_node.in_1 = in_1;
    mul_node.in_2 = in_2;
    mul_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
    mul_node.out = mul_image.get();

    mul_scalar_node.in = mul_image.get();
    mul_scalar_node.scalar = scalar->f32;
    if (overflow_policy != VX_CONVERT_POLICY_SATURATE)
    {
        mul_scalar_node.out = out;
    }
    else
    {
        mul_scalar_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
        mul_scalar_node.out = mul_scalar_image.get();

        saturate_node.in = mul_scalar_image.get();
        saturate_node.out = out;
    }

    mul_node.build();
    mul_scalar_node.build();
    if (overflow_policy == VX_CONVERT_POLICY_SATURATE)
        saturate_node.build();

    subnodes.push_back(&mul_node);
    subnodes.push_back(&mul_scalar_node);
    if (overflow_policy == VX_CONVERT_POLICY_SATURATE)
        subnodes.push_back(&saturate_node);

    inputs.emplace_back(in_1);
    inputs.emplace_back(in_2);
    outputs.emplace_back(out);
}

VXAccumulateNode::VXAccumulateNode()
{
    node_name = "VX Accumulation";
}

void VXAccumulateNode::build()
{
    add_node.in_1 = in;
    add_node.in_2 = in_out;
    add_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S32));
    add_node.out = add_image.get();

    saturate_node.in = add_image.get();
    saturate_node.out = in_out;

    add_node.build();
    saturate_node.build();

    subnodes.push_back(&add_node);
    subnodes.push_back(&saturate_node);

    inputs.emplace_back(in);
    inputs.emplace_back(in_out);
    outputs.emplace_back(in_out);
}

VXAccumulateSquareNode::VXAccumulateSquareNode()
{
    node_name = "VX Squared Accumulation";
}

void VXAccumulateSquareNode::build()
{
    square_node.in = in;
    square_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S16));
    square_node.out = square_image.get();

    depth_node.in = in;
    depth_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S16));
    depth_node.out = square_image.get();
    depth_node.policy = VX_CONVERT_POLICY_WRAP;
    int32_t signed_shift = shift->ui32;
    depth_scalar.reset(new Scalar(VX_TYPE_UINT32, &signed_shift));
    depth_node.shift = depth_scalar.get();


    add_node.in_1 = square_image.get();
    add_node.in_2 = in_out;
    add_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S16));
    add_node.out = add_image.get();

    saturate_node.in = add_image.get();
    saturate_node.out = in_out;

    square_node.build();
    depth_node.build();
    add_node.build();
    saturate_node.build();

    subnodes.push_back(&square_node);
    subnodes.push_back(&depth_node);
    subnodes.push_back(&add_node);
    subnodes.push_back(&saturate_node);

    inputs.emplace_back(in);
    inputs.emplace_back(in_out);
    inputs.emplace_back(shift);
    outputs.emplace_back(in_out);
}

VXAccumulateWeightedNode::VXAccumulateWeightedNode()
{
    node_name = "VX Weighted Accumulation";
}

void VXAccumulateWeightedNode::build()
{
    mul_scalar_left_node.in = in;
    mul_scalar_left_image.reset(new Image(in->w, in->h, VX_TYPE_FLOAT32));
    mul_scalar_left_node.out = mul_scalar_left_image.get();
    mul_scalar_left_node.scalar = 1.f - alpha->f32;

    mul_scalar_right_node.in = in_out;
    mul_scalar_right_image.reset(new Image(in->w, in->h, VX_TYPE_FLOAT32));
    mul_scalar_right_node.out = mul_scalar_right_image.get();
    mul_scalar_right_node.scalar = alpha->f32;

    add_node.in_1 = mul_scalar_left_image.get();
    add_node.in_2 = mul_scalar_right_image.get();
    add_image.reset(new Image(in->w, in->h, VX_DF_IMAGE_S16));
    add_node.out = in_out;


    mul_scalar_left_node.build();
    mul_scalar_right_node.build();
    add_node.build();

    subnodes.push_back(&mul_scalar_left_node);
    subnodes.push_back(&mul_scalar_right_node);
    subnodes.push_back(&add_node);

    inputs.emplace_back(in);
    inputs.emplace_back(in_out);
    inputs.emplace_back(alpha);
    outputs.emplace_back(in_out);
}

VXChannelExtractNode::VXChannelExtractNode()
{
    node_name = "VX Channel Extraction";
}

void VXChannelExtractNode::build()
{
	auto in_node = std::make_shared<function_ast::Image>(in);
    kernel->inputs.push_back(in_node);

	auto out_node = std::make_shared<function_ast::Image>(out);
    kernel->output = out_node;

    function_ast::VectChannelType channel_ast; 
    switch(channel_vx)
    {
    case VX_CHANNEL_B:
    case VX_CHANNEL_0:
        channel_ast = function_ast::VectChannelType::CHANNEL0;
        break;
    case VX_CHANNEL_G:
    case VX_CHANNEL_1:
        channel_ast = function_ast::VectChannelType::CHANNEL1;
        break;
    case VX_CHANNEL_R:
    case VX_CHANNEL_2:
        channel_ast = function_ast::VectChannelType::CHANNEL2;
        break;
    case VX_CHANNEL_A:
    case VX_CHANNEL_3:
        channel_ast = function_ast::VectChannelType::CHANNEL3;
        break;
    default:
        throw std::runtime_error("VXChannelExtractNode: Unsupported Channel Type");
    }

    kernel->function << assign(target_pixel(out_node), extract4(current_pixel(in_node), function_ast::Datatype::UCHAR4, channel_ast));

    inputs.emplace_back(in);
    outputs.emplace_back(out);
}

VXChannelCombineNode::VXChannelCombineNode()
{
    node_name = "VX Channel Combination";
}

void VXChannelCombineNode::build()
{
	auto in_node_1 = std::make_shared<function_ast::Image>(in_1);
	auto in_node_2 = std::make_shared<function_ast::Image>(in_2);
	auto in_node_3 = std::make_shared<function_ast::Image>(in_3);
	auto in_node_4 = std::make_shared<function_ast::Image>(in_4);
    kernel->inputs.push_back(in_node_1);
    kernel->inputs.push_back(in_node_2);
    kernel->inputs.push_back(in_node_3);
    kernel->inputs.push_back(in_node_4);

	auto out_node = std::make_shared<function_ast::Image>(out);
    kernel->output = out_node;

    kernel->function << assign(target_pixel(out_node), vect4(current_pixel(in_node_1), current_pixel(in_node_2), current_pixel(in_node_3), current_pixel(in_node_4), function_ast::Datatype::UCHAR4));

    inputs.emplace_back(in_1);
    inputs.emplace_back(in_2);
    inputs.emplace_back(in_3);
    inputs.emplace_back(in_4);
    outputs.emplace_back(out);
}

VXCopy::VXCopy()
{
    node_name = "VX copy";
}

void VXCopy::build()
{
    auto in_node = std::make_shared<function_ast::Image>(in);
    kernel->inputs.push_back(in_node);
    auto out_node = std::make_shared<function_ast::Image>(out);
    kernel->output = out_node;

    kernel->function << assign(target_pixel(out_node), current_pixel(in_node));

    inputs.emplace_back(in);
    outputs.emplace_back(out);
}

}
