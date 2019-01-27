// TODO: Would be nice to briefly comment on AST build

#include "../VX/vx.h"
#include "ast.hpp"
#include "domVX_types.hpp"

#pragma once


// TODO: IMPORTANT: Do not have multiple node classes

// namespace HipaVX
// {
// class SimplePointBinary;
// template <typename T> class SimplePointScalar;
// class ImageComparision;
// class SimplePointMul;
// class SquareNode;
// class SaturateNode;
// class SimplePointAdd;
// class SimplePointSub;
// class SimplePointDiv;
// class AndNode;
// class XorNode;
// class OrNode;
// template <typename T> class SimplePointScalarAdd;
// template <typename T> class SimplePointScalarSub;
// template <typename T> class SimplePointScalarMul;
// template <typename T> class SimplePointScalarDiv;
// class ConvertDepthNode;
// class NotNode;
// class VXSubtractNode;
// class VXThresholdNode;
// class SimplePointUnary;
// class SqrtNode;
// class AbsNode;
// class AbsDiffNode;
// class VXAddNode;
// class MagnitudeNode;
// class Atan2Node;
// class PhaseNode;
// class VXMultiplyNode;
// class VXAccumulateNode;
// class VXAccumulateSquareNode;
// class VXAccumulateWeightedNode;
// class VXChannelExtractNode;
// class VXChannelCombineNode;
// class VXScaleNode;
// class VXCopy
// }

namespace HipaVX
{

// ----------- Single Operation Unary (1to1) Point Operators ----------------
class SimplePointUnary: public Node
{
public:
    virtual ~SimplePointUnary() override = default;

    // 1 input, 1 output, 1 operation
    Image *in;
	Image *out;

    function_ast::NodeType operation;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class SqrtNode: public SimplePointUnary 
{
public:
    SqrtNode()
    {
		operation = function_ast::NodeType::Sqrt;
        node_name = "Sqrt";
    }
    virtual ~SqrtNode() override = default;
};

class Atan2Node: public SimplePointUnary 
{
public:
    Atan2Node()
    {
		operation = function_ast::NodeType::Atan2;
        node_name = "Atan2";
    }
    virtual ~Atan2Node() override = default;
};

class AbsNode: public SimplePointUnary 
{
public:
    AbsNode()
    {
		operation = function_ast::NodeType::Abs;
        node_name = "Abs";
    }
    virtual ~AbsNode() override = default;
};

class NotNode: public SimplePointUnary
{
public:
    NotNode()
    {
		operation = function_ast::NodeType::Not;
        node_name = "Not";
    }
    virtual ~NotNode() override = default;
};

class SquareNode: public SimplePointUnary
{
public:
    SquareNode()
    {
		operation = function_ast::NodeType::Mul;
        node_name = "Square";
    }
    virtual ~SquareNode() override = default;
};

// ----------- Single Operation Binary (2to1) Point Operators ----------------
class SimplePointBinary: public Node
{
public:
    virtual ~SimplePointBinary() override = default;

    // 2 inputs, 1 output, 1 operation
    Image *in_1;
    Image *in_2;
	Image *out;

    function_ast::NodeType operation;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class SimplePointMul: public SimplePointBinary
{
public:
    SimplePointMul()
    {
		operation = function_ast::NodeType::Mul;
        node_name = "Point Multiplication";
    }
    virtual ~SimplePointMul() override = default;
};

class SimplePointAdd: public SimplePointBinary
{
public:
    SimplePointAdd()
    {
		operation = function_ast::NodeType::Add;
        node_name = "Point Addition";
    }
    virtual ~SimplePointAdd() override = default;
};

class SimplePointSub: public SimplePointBinary
{
public:
    SimplePointSub()
    {
		operation = function_ast::NodeType::Sub;
        node_name = "Point Subtraction";
    }
    virtual ~SimplePointSub() override = default;
};

class SimplePointDiv: public SimplePointBinary
{
public:
    SimplePointDiv()
    {
		operation = function_ast::NodeType::Div;
        node_name = "Point Division";
    }
    virtual ~SimplePointDiv() override = default;
};

class AndNode: public SimplePointBinary
{
public:
    AndNode()
    {
		operation = function_ast::NodeType::BitwiseAnd;
        node_name = "And";
    }
    virtual ~AndNode() override = default;
};

class XorNode: public SimplePointBinary
{
public:
    XorNode()
    {
		operation = function_ast::NodeType::BitwiseXor;
        node_name = "Xor";
    }
    virtual ~XorNode() override = default;
};

class OrNode: public SimplePointBinary
{
public:
    OrNode()
    {
		operation = function_ast::NodeType::BitwiseOr;
        node_name = "or";
    }
    virtual ~OrNode() override = default;
};

// ----------- Binary Point Operators with a constant ( {1,c}to1) ------------
template <typename T>
class SimplePointScalar: public Node
{
public:
    virtual ~SimplePointScalar() override = default;

    Image *in;
    T scalar;
	Image *out;

    function_ast::NodeType operation;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
	virtual void build() override;
};

template <typename T>
class SimplePointScalarAdd: public SimplePointScalar<T>
{
public:
    SimplePointScalarAdd()
    {
		this->operation = function_ast::NodeType::Add;
        this->node_name = "Scalar Addition";
    }
    virtual ~SimplePointScalarAdd() override = default;
};

template <typename T>
class SimplePointScalarSub: public SimplePointScalar<T>
{
public:
    SimplePointScalarSub()
	{
		this->operation = function_ast::NodeType::Sub;
        this->node_name = "Scalar Subtraction";
    }
    virtual ~SimplePointScalarSub() override = default;
};

template <typename T>
class SimplePointScalarMul: public SimplePointScalar<T>
{
public:
    SimplePointScalarMul()
	{
		this->operation = function_ast::NodeType::Mul;
        this->node_name = "Scalar Multiplication";
    }
    virtual ~SimplePointScalarMul() override = default;
};

template <typename T>
class SimplePointScalarDiv: public SimplePointScalar<T>
{
public:
    SimplePointScalarDiv()
	{
		this->operation = function_ast::NodeType::Div;
        this->node_name = "Scalar Division";
    }
    virtual ~SimplePointScalarDiv() override = default;
};

template <typename T>
class SimplePointScalarShiftRight: public SimplePointScalar<T>
{
public:
    SimplePointScalarShiftRight()
	{
		this->operation = function_ast::NodeType::ShiftRight;
        this->node_name = "Scalar Shift Right";
    }
    virtual ~SimplePointScalarShiftRight() override = default;
};

template <typename T>
class SimplePointScalarShiftLeft: public SimplePointScalar<T>
{
public:
    SimplePointScalarShiftLeft()
	{
		this->operation = function_ast::NodeType::ShiftLeft;
        this->node_name = "Scalar Shift Left";
    }
    virtual ~SimplePointScalarShiftLeft() override = default;
};

// ------------------------------------------------------------------------
// TODO: Are these really necessary? Can we find a more orthogonal set?
//       Multiple node classes should be written as a VX graph at a higher abstraction layer

class SaturateNode: public Node
{
public:
    SaturateNode();
    virtual ~SaturateNode() override = default;
    Image *in;
    Image *out;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class VXThresholdNode: public Node
{
public:
    VXThresholdNode();
    virtual ~VXThresholdNode() override = default;

    Image *in;
    Image *out;
    Threshold *threshold;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    //virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXChannelExtractNode: public Node
{
public:
    VXChannelExtractNode();
    virtual ~VXChannelExtractNode() override = default;

    Image *in;
    Image *out;
    vx_channel_e channel_vx;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXChannelCombineNode: public Node
{
public:
    VXChannelCombineNode();
    virtual ~VXChannelCombineNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *in_3;
    Image *in_4;

    Image *out;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

// TODO: Multi-Nodes
class ConvertDepthNode: public Node
{
public:
    ConvertDepthNode();
    virtual ~ConvertDepthNode() override = default;
    Image *in;
    Image *out;
    vx_enum policy;
    Scalar *shift;

    SimplePointScalarShiftRight<unsigned> shift_node; // TODO: ?? how do we know datatype

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXSubtractNode: public Node
{
public:
    VXSubtractNode();
    virtual ~VXSubtractNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *out;

    SimplePointSub diff_node;
    std::unique_ptr<Image> diff_image;

    SaturateNode saturate_node;

    vx_enum policy;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class AbsDiffNode: public Node
{
public:
    AbsDiffNode();
    virtual ~AbsDiffNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *out;

    SimplePointSub diff_node;
    std::unique_ptr<Image> diff_image;

    AbsNode abs_node;
    std::unique_ptr<Image> abs_image;

    SaturateNode saturate_node;

    bool saturate;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXAddNode: public Node
{
public:
    VXAddNode();
    virtual ~VXAddNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *out;

    SimplePointAdd add_node;
    std::unique_ptr<Image> add_image;

    SaturateNode saturate_node;

    vx_enum policy;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class MagnitudeNode: public Node
{
public:
    MagnitudeNode();
    virtual ~MagnitudeNode() override = default;

    Image *in_1;
    Image *in_2;

    Image *out;

    SquareNode grad_x_square_node;
    std::unique_ptr<Image> grad_x_square_image;
    SquareNode grad_y_square_node;
    std::unique_ptr<Image> grad_y_square_image;

    SimplePointAdd add_node;
    std::unique_ptr<Image> add_image;

    SqrtNode sqrt_node;
    std::unique_ptr<Image> sqrt_image;

    SaturateNode saturate_node;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class PhaseNode: public Node
{
public:
    PhaseNode();
    virtual ~PhaseNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *out;

    SimplePointDiv div_node;
    std::unique_ptr<Image> div_image;

    Atan2Node atan2_node;
    std::unique_ptr<Image> atan2_image;

    SimplePointScalarMul<float> mapping_node;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXMultiplyNode: public Node
{
public:
    VXMultiplyNode();
    virtual ~VXMultiplyNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *out;
    Scalar *scalar;

    vx_enum overflow_policy;
    vx_enum rounding_policy;

    SimplePointMul mul_node;
    std::unique_ptr<Image> mul_image;

    SimplePointScalarMul<float> mul_scalar_node;
    std::unique_ptr<Image> mul_scalar_image;

    SaturateNode saturate_node;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXAccumulateNode: public Node
{
public:
    VXAccumulateNode();
    virtual ~VXAccumulateNode() override = default;
    Image *in;
    Image *in_out;

    SimplePointAdd add_node;
    std::unique_ptr<Image> add_image;

    SaturateNode saturate_node;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXAccumulateSquareNode: public Node
{
public:
    VXAccumulateSquareNode();
    virtual ~VXAccumulateSquareNode() override = default;
    Image *in;
    Image *in_out;
    Scalar *shift;

    SquareNode square_node;
    std::unique_ptr<Image> square_image;

    ConvertDepthNode depth_node;
    std::unique_ptr<Scalar> depth_scalar;
    std::unique_ptr<Image> depth_image;

    SimplePointAdd add_node;
    std::unique_ptr<Image> add_image;

    SaturateNode saturate_node;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXAccumulateWeightedNode: public Node
{
public:
    VXAccumulateWeightedNode();
    virtual ~VXAccumulateWeightedNode() override = default;
    Image *in;
    Image *in_out;
    Scalar *alpha;

    SimplePointScalarMul<float> mul_scalar_left_node;
    std::unique_ptr<Image> mul_scalar_left_image;

    SimplePointScalarMul<float> mul_scalar_right_node;
    std::unique_ptr<Image> mul_scalar_right_image;

    SimplePointAdd add_node;
    std::unique_ptr<Image> add_image;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXCopy: public Node
{
public:
    VXCopy();
    virtual ~VXCopy() override = default;

    // 2 inputs, 1 output, 1 operation
    Image *in;
    Image *out;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};
}

namespace HipaVX
{
template <typename T>
std::vector<Object *> SimplePointScalar<T>::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}
template <typename T>
std::vector<Object *> SimplePointScalar<T>::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}
template <typename T>
std::string SimplePointScalar<T>::generateClassDefinition()
{
	std::string s = function_ast::generate(&kernel);
	return s;
}
template <typename T>
std::string SimplePointScalar<T>::generateNodeCall()
{
	std::string s = function_ast::generate_call(&kernel);
	return s;
}
template <typename T>
void SimplePointScalar<T>::build()
{
	auto in_node = std::make_shared<function_ast::Image>(in);

	kernel.inputs.push_back(in_node);
	auto out_node = std::make_shared<function_ast::Image>(out);
	kernel.output = out_node;

	auto c = constant<>(scalar);

	switch(operation)
	{
	case function_ast::NodeType::Add:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node) + c);
		break;
	case function_ast::NodeType::Sub:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node) - c);
		break;
	case function_ast::NodeType::Mul:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node) * c);
		break;
	case function_ast::NodeType::Div:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node) / c);
		break;
	case function_ast::NodeType::ShiftRight:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node) >> c);
		break;
	case function_ast::NodeType::ShiftLeft:
		kernel.function << assign(target_pixel(out_node), current_pixel(in_node) << c);
		break;
	}
}
}
