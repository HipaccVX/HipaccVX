// TODO: Would be nice to briefly comment on AST build

#include "../VX/vx.h"
#include "ast.hpp"
#include "domVX_types.hpp"

#pragma once

// namespace HipaVX
// {
// class SimplePoint;
// template <typename T> class SimplePointScalar;
// class ImageComparision;
// template <typename T> class ConditionalAssignmentNode;
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
// class UnaryFunctionNode;
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
// }

namespace HipaVX
{

class SimplePoint: public Node
{
public:
    virtual ~SimplePoint() override = default;

    // 2 inputs, 1 output, 1 operation
    Image *in_1;
    Image *in_2;
	Image *out;

	function_ast::NodeType operation;
	function_ast::ForEveryPixel kernel;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
	virtual void build() override;
};

template <typename T>
class SimplePointScalar: public Node
{
public:
    virtual ~SimplePointScalar() override = default;

    Image *in;
    T scalar;
    Image *out;
    std::string operation;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class ImageComparision
{
public:
    std::string comp_op;
    std::string value;
    bool image_is_first_operand = true;
};

template <typename T>
class ConditionalAssignmentNode: public Node
{
public:
    ConditionalAssignmentNode();
    virtual ~ConditionalAssignmentNode() override = default;

    Image *in;
    Image *out;

    std::vector<ImageComparision> comparision;
    std::vector<std::string> chaining_operators;
    T true_value;
    T false_value;

    bool use_image_datatype_for_sum = true;
    vx_df_image sum_datatype;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class SimplePointMul: public SimplePoint
{
public:
    SimplePointMul()
    {
		operation = function_ast::NodeType::Mul;
        node_name = "Point Multiplication";
    }
    virtual ~SimplePointMul() override = default;
};

class SquareNode: public Node
{
public:
    SquareNode();
    virtual ~SquareNode() override = default;
    Image *in;
    Image *out;

    SimplePointMul mul_node;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

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
};

class SimplePointAdd: public SimplePoint
{
public:
    SimplePointAdd()
    {
		operation = function_ast::NodeType::Add;
        node_name = "Point Addition";
    }
    virtual ~SimplePointAdd() override = default;
};

class SimplePointSub: public SimplePoint
{
public:
    SimplePointSub()
    {
		operation = function_ast::NodeType::Sub;
        node_name = "Point Subtraction";
    }
    virtual ~SimplePointSub() override = default;
};

class SimplePointDiv: public SimplePoint
{
public:
    SimplePointDiv()
    {
		operation = function_ast::NodeType::Div;
        node_name = "Point Division";
    }
    virtual ~SimplePointDiv() override = default;
};

class AndNode: public SimplePoint
{
public:
    AndNode()
    {
		operation = function_ast::NodeType::BitwiseAnd;
        node_name = "And";
    }
    virtual ~AndNode() override = default;
};

class XorNode: public SimplePoint
{
public:
    XorNode()
    {
		operation = function_ast::NodeType::BitwiseXor;
        node_name = "Xor";
    }
    virtual ~XorNode() override = default;
};

class OrNode: public SimplePoint
{
public:
    OrNode()
    {
		operation = function_ast::NodeType::BitwiseOr;
        node_name = "or";
    }
    virtual ~OrNode() override = default;
};

template <typename T>
class SimplePointScalarAdd: public SimplePointScalar<T>
{
public:
    SimplePointScalarAdd()
    {
        this->operation = "+";
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
        this->operation = "-";
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
        this->operation = "*";
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
        this->operation = "/";
        this->node_name = "Scalar Division";
    }
    virtual ~SimplePointScalarDiv() override = default;
};

class ConvertDepthNode: public Node
{
public:
    ConvertDepthNode();
    virtual ~ConvertDepthNode() override = default;
    Image *in;
    Image *out;

    vx_enum policy;
    Scalar *shift;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class NotNode: public Node
{
public:
    NotNode();
    virtual ~NotNode() override = default;
    Image *in;
    Image *out;

	function_ast::ForEveryPixel kernel;

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

class VXThresholdNode: public Node
{
public:
    VXThresholdNode();
    virtual ~VXThresholdNode() override = default;

    Image *in;
    Image *out;
    Threshold *threshold;

    ConditionalAssignmentNode<int> comparision_node;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class UnaryFunctionNode: public Node
{
public:
    UnaryFunctionNode();
    virtual ~UnaryFunctionNode() override = default;
    Image *in;
    Image *out;

    std::string function;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class SqrtNode: public Node
{
public:
    SqrtNode();
    virtual ~SqrtNode() override = default;
    Image *in;
    Image *out;

    UnaryFunctionNode function_node;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class AbsNode: public Node
{
public:
    AbsNode();
    virtual ~AbsNode() override = default;
    Image *in;
    Image *out;

    UnaryFunctionNode function_node;

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

class Atan2Node: public Node
{
public:
    Atan2Node();
    virtual ~Atan2Node() override = default;
    Image *in;
    Image *out;

    UnaryFunctionNode function_node;

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

class VXChannelExtractNode: public Node
{
public:
    VXChannelExtractNode();
    virtual ~VXChannelExtractNode() override = default;
    Image *in;
    vx_channel_e channel;
    Image *out;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
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
};

class VXScaleNode: public Node
{
public:
    VXScaleNode();
    virtual ~VXScaleNode() override = default;

    Image *in;
    Image *out;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};
}


namespace generator
{
enum class Type
{
    Definition = 0,
    Call
};

// TODO: remove all except HipaccNode, WriteImageNode
std::string node_generator(HipaVX::ConvertDepthNode* n, Type t);

std::string node_generator(HipaVX::VXChannelExtractNode* n, Type t);
std::string node_generator(HipaVX::VXChannelCombineNode* n, Type t);
template <typename T>
std::string node_generator(HipaVX::SimplePointScalar<T>* n, Type t);

std::string node_generator(HipaVX::SaturateNode* n, Type t);
std::string node_generator(HipaVX::UnaryFunctionNode* n, Type t);
template <typename T>
std::string node_generator(HipaVX::ConditionalAssignmentNode<T>* n, Type t);

std::string node_generator(HipaVX::VXScaleNode *n, Type t);

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
    return generator::node_generator(this, generator::Type::Definition);
}
template <typename T>
std::string SimplePointScalar<T>::generateNodeCall()
{
    return generator::node_generator(this, generator::Type::Call);
}

template <typename T>
ConditionalAssignmentNode<T>::ConditionalAssignmentNode()
{
    node_name = "Conditional Assignment";
}

template <typename T>
std::vector<Object *> ConditionalAssignmentNode<T>::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}

template <typename T>
std::vector<Object *> ConditionalAssignmentNode<T>::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}

template <typename T>
std::string ConditionalAssignmentNode<T>::generateClassDefinition()
{
    return generator::node_generator(this, generator::Type::Definition);
}

template <typename T>
std::string ConditionalAssignmentNode<T>::generateNodeCall()
{
    return generator::node_generator(this, generator::Type::Call);
}

}
