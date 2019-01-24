//#include "../VX/vx.h"
//#include "ast.hpp"
//#include <string>
//#include "domVX_types.hpp"
//#include "domVX_kernels.hpp"
#include "domVX_kernels_point.hpp"

namespace HipaVX
{

// ----------- Single Operation Unary (1to1) Point Operators ----------------
std::vector<Object *> SimplePointUnary::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}

std::vector<Object *> SimplePointUnary::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::string SimplePointUnary::generateClassDefinition()
{
	std::string s = function_ast::generate(&kernel);
	return s;
}

std::string SimplePointUnary::generateNodeCall()
{
	std::string s = function_ast::generate_call(&kernel);
	return s;
}

void SimplePointUnary::build()
{
	auto in_node = std::make_shared<function_ast::Image>(in);
	kernel.inputs.push_back(in_node);
	auto out_node = std::make_shared<function_ast::Image>(out);
	kernel.output = out_node;

	switch(operation)
	{
	case function_ast::NodeType::Sqrt:
		kernel.function << assign(target_pixel(out_node), sqrt(current_pixel(in_node)));
		break;
    case function_ast::NodeType::Abs:
        kernel.function << assign(target_pixel(out_node), abs(current_pixel(in_node)));
        break;
	case function_ast::NodeType::Atan2:
		kernel.function << assign(target_pixel(out_node), atan2(current_pixel(in_node)));
		break;
	case function_ast::NodeType::Not:
	    kernel.function << assign(target_pixel(out_node), ~current_pixel(in_node));
		break;
	case function_ast::NodeType::Mul: // Unary node Mul is "square"
	    kernel.function << assign(target_pixel(out_node), current_pixel(in_node) * current_pixel(in_node));
		break;
	}
}

// ----------- Single Operation Binary (2to1) Point Operators ----------------
std::vector<Object *> SimplePointBinary::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in_1);
    used_objects.emplace_back(in_2);
    return used_objects;
}

std::vector<Object *> SimplePointBinary::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::string SimplePointBinary::generateClassDefinition()
{
	std::string s = function_ast::generate(&kernel);
	return s;
}

std::string SimplePointBinary::generateNodeCall()
{
	std::string s = function_ast::generate_call(&kernel);
	return s;
}

void SimplePointBinary::build()
{
	auto in_node_1 = std::make_shared<function_ast::Image>(in_1);
	auto in_node_2 = std::make_shared<function_ast::Image>(in_2);
	kernel.inputs.push_back(in_node_1);
	kernel.inputs.push_back(in_node_2);
	auto out_node = std::make_shared<function_ast::Image>(out);
	kernel.output = out_node;

	switch(operation)
	{
	case function_ast::NodeType::Add:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node_1) + current_pixel(in_node_2));
		break;
	case function_ast::NodeType::Sub:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node_1) - current_pixel(in_node_2));
		break;
	case function_ast::NodeType::Mul:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node_1) * current_pixel(in_node_2));
		break;
	case function_ast::NodeType::Div:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node_1) / current_pixel(in_node_2));
		break;
	case function_ast::NodeType::BitwiseAnd:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node_1) & current_pixel(in_node_2));
		break;
	case function_ast::NodeType::BitwiseOr:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node_1) | current_pixel(in_node_2));
		break;
	case function_ast::NodeType::BitwiseXor:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node_1) ^ current_pixel(in_node_2));
		break;
	}
}

SaturateNode::SaturateNode()
{
    node_name = "Saturate";
}

std::vector<Object *> SaturateNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}

std::vector<Object *> SaturateNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::string SaturateNode::generateClassDefinition()
{
	std::string s = function_ast::generate(&kernel);
	return s;
}

std::string SaturateNode::generateNodeCall()
{
	std::string s = function_ast::generate_call(&kernel);
	return s;
}
void SaturateNode::build()
{
	auto in_node = std::make_shared<function_ast::Image>(in);
	kernel.inputs.push_back(in_node);
	auto out_node = std::make_shared<function_ast::Image>(out);
	kernel.output = out_node;

	auto temp = std::make_shared<function_ast::Variable>("temp", function_ast::Datatype::INT32);
	kernel.function << define(temp) << assign(temp, current_pixel(in_node));

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

	kernel.function << greater_if;
	kernel.function << greater_else;

	kernel.function << assign(target_pixel(out_node), temp);
}

ConvertDepthNode::ConvertDepthNode()
{
    node_name = "Depth Converter";
}

std::vector<Object *> ConvertDepthNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}

std::vector<Object *> ConvertDepthNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::string ConvertDepthNode::generateClassDefinition()
{
	std::string s = function_ast::generate(&kernel);
	return s;
}

std::string ConvertDepthNode::generateNodeCall()
{
	std::string s = function_ast::generate_call(&kernel);
	return s;
}

void ConvertDepthNode::build() {
	auto in_node = std::make_shared<function_ast::Image>(in);
	kernel.inputs.push_back(in_node);
	auto out_node = std::make_shared<function_ast::Image>(out);
	kernel.output = out_node;

    // Shift right
	auto temp = std::make_shared<function_ast::Variable>("value", function_ast::Datatype::INT16); // TODO: datatype
	auto shift_c = constant<>(shift->i32);
    kernel.function << define(temp) << assign(temp, current_pixel(in_node) >> shift_c);

	std::shared_ptr<function_ast::Node> min = constant<int>(0);
	std::shared_ptr<function_ast::Node> max = constant<int>(255);

    // max/min overflow
	auto less_if = IF(less(temp, min));
	     less_if->body << assign(temp, min);

	auto greater_if = IF(greater(temp, max));
	     greater_if->body << assign(temp, max);
	auto greater_else = ELSE();
	     greater_else->body << less_if;

	kernel.function << greater_if;
	kernel.function << greater_else;

	kernel.function << assign(target_pixel(out_node), temp);
}

VXSubtractNode::VXSubtractNode()
{
    node_name = "VX Subtraction Node";
}

std::vector<Object *> VXSubtractNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in_1);
    used_objects.emplace_back(in_2);
    return used_objects;
}

std::vector<Object *> VXSubtractNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::vector<Node*> VXSubtractNode::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&diff_node);
    if (policy == VX_CONVERT_POLICY_SATURATE)
        subnodes.push_back(&saturate_node);
    return subnodes;
}

std::string VXSubtractNode::generateClassDefinition()
{
    std::string s = diff_node.generateClassDefinition();
    if (policy == VX_CONVERT_POLICY_SATURATE)
        s += "\n" + saturate_node.generateClassDefinition();
    return s;
}

std::string VXSubtractNode::generateNodeCall()
{
    std::string s = diff_node.generateNodeCall();
    if (policy == VX_CONVERT_POLICY_SATURATE)
        s += "\n" + saturate_node.generateNodeCall();
    return s;
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
}

VXThresholdNode::VXThresholdNode()
{
    node_name = "VX Threshold";
}

std::vector<Object *> VXThresholdNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    used_objects.emplace_back(threshold);
    return used_objects;
}

std::vector<Object *> VXThresholdNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::string VXThresholdNode::generateClassDefinition()
{
	std::string s = function_ast::generate(&kernel);
	return s;
}

std::string VXThresholdNode::generateNodeCall()
{
	std::string s = function_ast::generate_call(&kernel);
	return s;
}

void VXThresholdNode::build()
{
	auto in_node = std::make_shared<function_ast::Image>(in);
	kernel.inputs.push_back(in_node);
	auto out_node = std::make_shared<function_ast::Image>(out);
	kernel.output = out_node;

    // buffer input image
	// auto in_d = std::make_shared<function_ast::Variable>("in_d", in_node->type); //TODO: how to gen in dtype? function_ast::Datatype::INT16);
    // kernel.function << define(in_d) << assign(in_d, current_pixel(in_node));

    // TODO: Even move the outer selection to kernels_point.cpp
    if (threshold->threshold_type == VX_THRESHOLD_TYPE_BINARY)
    {
	    auto if_less = IF(less(current_pixel(in_node), constant<>(threshold->value)));
             if_less->body << assign(target_pixel(out_node), constant<>(threshold->true_value));
	    auto else_less = ELSE();
	         else_less->body << assign(target_pixel(out_node), constant<>(threshold->true_value));
        kernel.function << if_less;
        kernel.function << else_less;
    }
    else
    {
	    auto if_outrange = IF(greater(current_pixel(in_node), constant<>(threshold->upper)) || less(current_pixel(in_node), constant<>(threshold->lower)));
	         if_outrange->body << assign(target_pixel(out_node), constant<>(threshold->false_value));
	    auto else_outrange = ELSE();
	         else_outrange->body << assign(target_pixel(out_node), constant<>(threshold->true_value));
        kernel.function << if_outrange;
        kernel.function << else_outrange;
    }
}

AbsDiffNode::AbsDiffNode()
{
    node_name = "Absolute Difference";
}

std::vector<Object *> AbsDiffNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in_1);
    used_objects.emplace_back(in_2);
    return used_objects;
}

std::vector<Object *> AbsDiffNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::vector<Node*> AbsDiffNode::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&diff_node);
    subnodes.push_back(&abs_node);
    if (saturate)
        subnodes.push_back(&saturate_node);
    return subnodes;
}

std::string AbsDiffNode::generateClassDefinition()
{
    std::string s = diff_node.generateClassDefinition();
    s += "\n" + abs_node.generateClassDefinition();
    if (saturate)
        s += "\n" + saturate_node.generateClassDefinition();
    return s;
}

std::string AbsDiffNode::generateNodeCall()
{
    std::string s = diff_node.generateNodeCall();
    s += "\n" + abs_node.generateNodeCall();
    if (saturate)
        s += "\n" + saturate_node.generateNodeCall();
    return s;
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
}

VXAddNode::VXAddNode()
{
    node_name = "VX Addition Node";
}

std::vector<Object *> VXAddNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in_1);
    used_objects.emplace_back(in_2);
    return used_objects;
}

std::vector<Object *> VXAddNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::vector<Node*> VXAddNode::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&add_node);
    if (policy == VX_CONVERT_POLICY_SATURATE)
        subnodes.push_back(&saturate_node);
    return subnodes;
}

std::string VXAddNode::generateClassDefinition()
{
    std::string s = add_node.generateClassDefinition();
    if (policy == VX_CONVERT_POLICY_SATURATE)
        s += "\n" + saturate_node.generateClassDefinition();
    return s;
}

std::string VXAddNode::generateNodeCall()
{
    std::string s = add_node.generateNodeCall();
    if (policy == VX_CONVERT_POLICY_SATURATE)
        s += "\n" + saturate_node.generateNodeCall();
    return s;
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
}

MagnitudeNode::MagnitudeNode()
{
    node_name = "VX Magnitude";
}

std::vector<Object *> MagnitudeNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in_1);
    used_objects.emplace_back(in_2);
    return used_objects;
}

std::vector<Object *> MagnitudeNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::vector<Node*> MagnitudeNode::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&grad_x_square_node);
    subnodes.push_back(&grad_y_square_node);
    subnodes.push_back(&add_node);
    subnodes.push_back(&sqrt_node);
    subnodes.push_back(&saturate_node);
    return subnodes;
}

std::string MagnitudeNode::generateClassDefinition()
{
    std::string s = grad_x_square_node.generateClassDefinition();
    s += "\n" + grad_y_square_node.generateClassDefinition();
    s += "\n" + add_node.generateClassDefinition();
    s += "\n" + sqrt_node.generateClassDefinition();
    s += "\n" + saturate_node.generateClassDefinition();
    return s;
}

std::string MagnitudeNode::generateNodeCall()
{
    std::string s = grad_x_square_node.generateNodeCall();
    s += "\n" + grad_y_square_node.generateNodeCall();
    s += "\n" + add_node.generateNodeCall();
    s += "\n" + sqrt_node.generateNodeCall();
    s += "\n" + saturate_node.generateNodeCall();
    return s;
}

void MagnitudeNode::build()
{
    grad_x_square_node.in = in_1;
    grad_x_square_image.reset(new Image(in_1->w, in_1->h, VX_DF_IMAGE_S32));
    grad_x_square_node.out = grad_x_square_image.get();

    grad_y_square_node.in = in_2;
    grad_y_square_image.reset(new Image(in_1->w, in_1->h, VX_DF_IMAGE_S32));
    grad_y_square_node.out = grad_y_square_image.get();

    add_node.in_1 = grad_x_square_image.get();
    add_node.in_2 = grad_y_square_image.get();
    add_image.reset(new Image(in_1->w, in_1->h, VX_DF_IMAGE_S32));
    add_node.out = add_image.get();

    sqrt_node.in = grad_x_square_image.get();
    sqrt_image.reset(new Image(in_1->w, in_2->h, VX_DF_IMAGE_S32));
    sqrt_node.out = sqrt_image.get();

    saturate_node.in = sqrt_image.get();
    saturate_node.out = out;

    grad_x_square_node.build();
    grad_y_square_node.build();
    add_node.build();
    sqrt_node.build();
    saturate_node.build();
}

PhaseNode::PhaseNode()
{
    node_name = "VX Phase";
}

std::vector<Object *> PhaseNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in_1);
    used_objects.emplace_back(in_2);
    return used_objects;
}

std::vector<Object *> PhaseNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::vector<Node*> PhaseNode::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&div_node);
    subnodes.push_back(&atan2_node);
    subnodes.push_back(&mapping_node);
    return subnodes;
}

std::string PhaseNode::generateClassDefinition()
{
    std::string s = div_node.generateClassDefinition();
    s += "\n" + atan2_node.generateClassDefinition();
    s += "\n" + mapping_node.generateClassDefinition();
    return s;
}

std::string PhaseNode::generateNodeCall()
{
    std::string s = div_node.generateNodeCall();
    s += "\n" + atan2_node.generateNodeCall();
    s += "\n" + mapping_node.generateNodeCall();
    return s;
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
}

VXMultiplyNode::VXMultiplyNode()
{
    node_name = "VX Multiplication";
}

std::vector<Object *> VXMultiplyNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in_1);
    used_objects.emplace_back(in_2);
    used_objects.emplace_back(scalar);
    return used_objects;
}

std::vector<Object *> VXMultiplyNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::vector<Node*> VXMultiplyNode::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&mul_node);
    subnodes.push_back(&mul_scalar_node);
    if (overflow_policy == VX_CONVERT_POLICY_SATURATE)
        subnodes.push_back(&saturate_node);
    return subnodes;
}

std::string VXMultiplyNode::generateClassDefinition()
{
    std::string s = mul_node.generateClassDefinition();
    s += "\n" + mul_scalar_node.generateClassDefinition();
    if (overflow_policy == VX_CONVERT_POLICY_SATURATE)
        s += "\n" + saturate_node.generateClassDefinition();
    return s;
}

std::string VXMultiplyNode::generateNodeCall()
{
    std::string s = mul_node.generateNodeCall();
    s += "\n" + mul_scalar_node.generateNodeCall();
    if (overflow_policy == VX_CONVERT_POLICY_SATURATE)
        s += "\n" + saturate_node.generateNodeCall();
    return s;
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
}

VXAccumulateNode::VXAccumulateNode()
{
    node_name = "VX Accumulation";
}

std::vector<Object *> VXAccumulateNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    used_objects.emplace_back(in_out);
    return used_objects;
}

std::vector<Object *> VXAccumulateNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in_out);
    return used_objects;
}

std::vector<Node*> VXAccumulateNode::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&add_node);
    subnodes.push_back(&saturate_node);
    return subnodes;
}

std::string VXAccumulateNode::generateClassDefinition()
{
    std::string s = add_node.generateClassDefinition();
    s += "\n" + saturate_node.generateClassDefinition();
    return s;
}

std::string VXAccumulateNode::generateNodeCall()
{
    std::string s = add_node.generateNodeCall();
    s += "\n" + saturate_node.generateNodeCall();
    return s;
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
}

VXAccumulateSquareNode::VXAccumulateSquareNode()
{
    node_name = "VX Squared Accumulation";
}

std::vector<Object *> VXAccumulateSquareNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    used_objects.emplace_back(in_out);
    used_objects.emplace_back(shift);
    return used_objects;
}

std::vector<Object *> VXAccumulateSquareNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in_out);
    return used_objects;
}

std::vector<Node*> VXAccumulateSquareNode::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&square_node);
    subnodes.push_back(&depth_node);
    subnodes.push_back(&add_node);
    subnodes.push_back(&saturate_node);
    return subnodes;
}

std::string VXAccumulateSquareNode::generateClassDefinition()
{
    std::string s = square_node.generateClassDefinition();
    s += "\n" + depth_node.generateClassDefinition();
    s += "\n" + add_node.generateClassDefinition();
    s += "\n" + saturate_node.generateClassDefinition();
    return s;
}

std::string VXAccumulateSquareNode::generateNodeCall()
{
    std::string s = add_node.generateNodeCall();
    s += "\n" + depth_node.generateNodeCall();
    s += "\n" + add_node.generateNodeCall();
    s += "\n" + saturate_node.generateNodeCall();
    return s;
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
}

VXAccumulateWeightedNode::VXAccumulateWeightedNode()
{
    node_name = "VX Weighted Accumulation";
}

std::vector<Object *> VXAccumulateWeightedNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    used_objects.emplace_back(in_out);
    used_objects.emplace_back(alpha);
    return used_objects;
}

std::vector<Object *> VXAccumulateWeightedNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in_out);
    return used_objects;
}

std::vector<Node*> VXAccumulateWeightedNode::get_subnodes()
{
    std::vector<Node*> subnodes;
    subnodes.push_back(&mul_scalar_left_node);
    subnodes.push_back(&mul_scalar_right_node);
    subnodes.push_back(&add_node);
    return subnodes;
}

std::string VXAccumulateWeightedNode::generateClassDefinition()
{
    std::string s = mul_scalar_left_node.generateClassDefinition();
    s += "\n" + mul_scalar_right_node.generateClassDefinition();
    s += "\n" + add_node.generateClassDefinition();
    return s;
}

std::string VXAccumulateWeightedNode::generateNodeCall()
{
    std::string s = mul_scalar_left_node.generateNodeCall();
    s += "\n" + mul_scalar_right_node.generateNodeCall();
    s += "\n" + add_node.generateNodeCall();
    return s;
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
}

VXChannelExtractNode::VXChannelExtractNode()
{
    node_name = "VX Channel Extraction";
}

std::vector<Object *> VXChannelExtractNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}

std::vector<Object *> VXChannelExtractNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::string VXChannelExtractNode::generateClassDefinition()
{
	std::string s = function_ast::generate(&kernel);
	return s;
}

std::string VXChannelExtractNode::generateNodeCall()
{
	std::string s = function_ast::generate_call(&kernel);
	return s;
}

void VXChannelExtractNode::build()
{
	auto in_node = std::make_shared<function_ast::Image>(in);
	kernel.inputs.push_back(in_node);

	auto out_node = std::make_shared<function_ast::Image>(out);
	kernel.output = out_node;

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

	kernel.function << assign(target_pixel(out_node), extract4(current_pixel(in_node), function_ast::Datatype::UCHAR4, channel_ast));
}

VXChannelCombineNode::VXChannelCombineNode()
{
    node_name = "VX Channel Combination";
}

std::vector<Object *> VXChannelCombineNode::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in_1);
    used_objects.emplace_back(in_2);
    used_objects.emplace_back(in_3);
    used_objects.emplace_back(in_4);
    return used_objects;
}

std::vector<Object *> VXChannelCombineNode::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

std::string VXChannelCombineNode::generateClassDefinition()
{
	std::string s = function_ast::generate(&kernel);
	return s;
}

std::string VXChannelCombineNode::generateNodeCall()
{
	std::string s = function_ast::generate_call(&kernel);
	return s;
}

void VXChannelCombineNode::build()
{
	auto in_node_1 = std::make_shared<function_ast::Image>(in_1);
	auto in_node_2 = std::make_shared<function_ast::Image>(in_2);
	auto in_node_3 = std::make_shared<function_ast::Image>(in_3);
	auto in_node_4 = std::make_shared<function_ast::Image>(in_4);
	kernel.inputs.push_back(in_node_1);
	kernel.inputs.push_back(in_node_2);
	kernel.inputs.push_back(in_node_3);
	kernel.inputs.push_back(in_node_4);

	auto out_node = std::make_shared<function_ast::Image>(out);
	kernel.output = out_node;

	kernel.function << assign(target_pixel(out_node), vect4(current_pixel(in_node_1), current_pixel(in_node_2), current_pixel(in_node_3), current_pixel(in_node_4), function_ast::Datatype::UCHAR4));
}

}
