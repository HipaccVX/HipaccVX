#ifndef __HIPAVX_INTERNAL__
#define __HIPAVX_INTERNAL__

#include "../VX/vx.h"
#include "domVX_types.hpp"

#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <memory>
#include <string>
#include <algorithm>
#include <memory>

namespace function_ast
{

// TODO: Consider different enum types for abstractions and scalar operations
enum class NodeType
{
    None,
    Add,
    Sub,
    Mul,
    Div,
    ShiftLeft,
    ShiftRight,

    Less,
    LessEquals,
    Equals,
    GreaterEquals,
    Greater,
    Unequals,

	And,
	Or,
	Xor,
	Not,

	BitwiseAnd,
	BitwiseOr,
	BitwiseXor,
	BitwiseNot,

    Vect4,
    Extract4,

    Sqrt,
    Exp,
    Conversion,
    Abs,
    Atan2,

    Constant,
    Variable,
    VariableDefinition,
    Assignment,
    TargetPixel,

    If,
    Else,

    Image,
    ForEveryPixel,
    CurrentPixelvalue,

    Stencil,
    IterateAroundPixel,
    ReductionOutput,
    ReduceAroundPixel,
    PixelvalueAtCurrentStencilPos,
    StencilvalueAtCurrentStencilPos,

    Statements,
};

enum class Datatype
{
    UCHAR,
    INT32,
    UINT32,
    FLOAT,
    UINT8,
    UINT16,
    INT16,

    UCHAR4,
    UINT4,
};

enum class VectChannelType 
{
    CHANNEL0,
    CHANNEL1,
    CHANNEL2,
    CHANNEL3
};


enum class ReductionType
{
    SUM,
    MIN,
    MAX
};

// TODO: Rename to AST Node
class Node
{
public:
    int id;
    Node()
    {
        static int next_id = 0;
        id = next_id++;
    }
    NodeType type = NodeType::None;
    std::vector<std::shared_ptr<Node>> subnodes;
    std::shared_ptr<Node>& operator[](unsigned int index)
    {
        return subnodes[index];
    }
    std::shared_ptr<Node> operator[](unsigned int index) const
    {
        return subnodes[index];
    }

    virtual ~Node() = default;
    virtual std::string generate_source() = 0;
};

class SimpleBinaryNode: public Node
{
public:
	virtual ~SimpleBinaryNode() override = default;
    virtual std::string generate_source() override;
};

class SimpleUnaryFunctionNode: public Node
{
public:
	virtual ~SimpleUnaryFunctionNode() override = default;
    virtual std::string generate_source() override;
};

class Add: public SimpleBinaryNode
{
public:
    Add()
    {
        type = NodeType::Add;
        subnodes.resize(2);
    }
    Add(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::Add;
        subnodes = {n1, n2};
    }

    virtual ~Add() = default;
};

class Sub: public SimpleBinaryNode
{
public:
    Sub()
    {
        type = NodeType::Sub;
        subnodes.resize(2);
    }
    Sub(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::Sub;
        subnodes = {n1, n2};
    }

    virtual ~Sub() = default;
};

class Mul: public SimpleBinaryNode
{
public:
    Mul()
    {
        type = NodeType::Mul;
        subnodes.resize(2);
    }
    Mul(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::Mul;
        subnodes = {n1, n2};
    }

    virtual ~Mul() = default;
};

class Div: public SimpleBinaryNode
{
public:
    Div()
    {
        type = NodeType::Div;
        subnodes.resize(2);
    }
    Div(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::Div;
        subnodes = {n1, n2};
    }

    virtual ~Div() = default;
};

class ShiftLeft: public SimpleBinaryNode
{
public:
    ShiftLeft()
    {
        type = NodeType::ShiftLeft;
        subnodes.resize(2);
    }
    ShiftLeft(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::ShiftLeft;
        subnodes = {n1, n2};
    }

    virtual ~ShiftLeft() = default;
};

class ShiftRight: public SimpleBinaryNode
{
public:
    ShiftRight()
    {
        type = NodeType::ShiftRight;
        subnodes.resize(2);
    }
    ShiftRight(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::ShiftRight;
        subnodes = {n1, n2};
    }

    virtual ~ShiftRight() = default;
};

class And: public SimpleBinaryNode
{
public:
	And()
	{
		type = NodeType::And;
		subnodes.resize(2);
	}
	And(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
	{
		type = NodeType::And;
		subnodes = {n1, n2};
	}

	virtual ~And() = default;
};

class Or: public SimpleBinaryNode
{
public:
	Or()
	{
		type = NodeType::Or;
		subnodes.resize(2);
	}
	Or(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
	{
		type = NodeType::Or;
		subnodes = {n1, n2};
	}

	virtual ~Or() = default;
};

class Xor: public SimpleBinaryNode
{
public:
	Xor()
	{
		type = NodeType::Xor;
		subnodes.resize(2);
	}
	Xor(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
	{
		type = NodeType::Xor;
		subnodes = {n1, n2};
	}

	virtual ~Xor() = default;
};

class BitwiseAnd: public SimpleBinaryNode
{
public:
    BitwiseAnd()
    {
        type = NodeType::BitwiseAnd;
        subnodes.resize(2);
    }
    BitwiseAnd(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::BitwiseAnd;
        subnodes = {n1, n2};
    }

    virtual ~BitwiseAnd() = default;
};

class Less: public SimpleBinaryNode
{
public:
    Less()
    {
        type = NodeType::Less;
        subnodes.resize(2);
    }
    Less(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::Less;
        subnodes = {n1, n2};
    }

    virtual ~Less() = default;
};

class LessEquals: public SimpleBinaryNode
{
public:
    LessEquals()
    {
        type = NodeType::LessEquals;
        subnodes.resize(2);
    }
    LessEquals(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::LessEquals;
        subnodes = {n1, n2};
    }

    virtual ~LessEquals() = default;
};

class Equals: public SimpleBinaryNode
{
public:
    Equals()
    {
        type = NodeType::Equals;
        subnodes.resize(2);
    }
    Equals(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::Equals;
        subnodes = {n1, n2};
    }

    virtual ~Equals() = default;
};

class GreaterEquals: public SimpleBinaryNode
{
public:
    GreaterEquals()
    {
        type = NodeType::GreaterEquals;
        subnodes.resize(2);
    }
    GreaterEquals(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::GreaterEquals;
        subnodes = {n1, n2};
    }

    virtual ~GreaterEquals() = default;
};

class Greater: public SimpleBinaryNode
{
public:
    Greater()
    {
        type = NodeType::Greater;
        subnodes.resize(2);
    }
    Greater(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::Greater;
        subnodes = {n1, n2};
    }

    virtual ~Greater() = default;
};

class Unequals: public SimpleBinaryNode
{
public:
    Unequals()
    {
        type = NodeType::Unequals;
        subnodes.resize(2);
    }
    Unequals(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
    {
        type = NodeType::Unequals;
        subnodes = {n1, n2};
    }

    virtual ~Unequals() = default;
};

class BitwiseOr: public SimpleBinaryNode
{
public:
	BitwiseOr()
	{
		type = NodeType::BitwiseOr;
		subnodes.resize(2);
	}
	BitwiseOr(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
	{
		type = NodeType::BitwiseOr;
		subnodes = {n1, n2};
	}

	virtual ~BitwiseOr() = default;
};

class BitwiseXor: public SimpleBinaryNode
{
public:
	BitwiseXor()
	{
		type = NodeType::BitwiseXor;
		subnodes.resize(2);
	}
	BitwiseXor(std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2)
	{
		type = NodeType::BitwiseXor;
		subnodes = {n1, n2};
	}

	virtual ~BitwiseXor() = default;
};

class Not: public SimpleUnaryFunctionNode
{
public:
	Not()
	{
		type = NodeType::Not;
		subnodes.resize(1);
	}
	Not(std::shared_ptr<Node> n1)
	{
		type = NodeType::Not;
		subnodes = {n1};
	}
	virtual ~Not() = default;
};

class BitwiseNot: public SimpleUnaryFunctionNode
{
public:
	BitwiseNot()
	{
		type = NodeType::BitwiseNot;
		subnodes.resize(1);
	}
	BitwiseNot(std::shared_ptr<Node> n1)
	{
		type = NodeType::BitwiseNot;
		subnodes = {n1};
	}
	virtual ~BitwiseNot() = default;
};

class Sqrt: public SimpleUnaryFunctionNode
{
public:
    Sqrt()
    {
        type = NodeType::Sqrt;
        subnodes.resize(1);
    }
    Sqrt(std::shared_ptr<Node> n1)
    {
        type = NodeType::Sqrt;
        subnodes = {n1};
    }
    virtual ~Sqrt() = default;
};

class Abs: public SimpleUnaryFunctionNode
{
public:
    Abs()
    {
        type = NodeType::Abs;
        subnodes.resize(1);
    }
    Abs(std::shared_ptr<Node> n1)
    {
        type = NodeType::Abs;
        subnodes = {n1};
    }
    virtual ~Abs() = default;
};

class Atan2: public SimpleUnaryFunctionNode
{
public:
    Atan2()
    {
        type = NodeType::Atan2;
        subnodes.resize(1);
    }
    Atan2(std::shared_ptr<Node> n1)
    {
        type = NodeType::Atan2;
        subnodes = {n1};
    }
    virtual ~Atan2() = default;
};

class Exp: public SimpleUnaryFunctionNode
{
public:
    Exp()
    {
        type = NodeType::Exp;
        subnodes.resize(1);
    }
    Exp(std::shared_ptr<Node> n1)
    {
        type = NodeType::Exp;
        subnodes = {n1};
    }
    virtual ~Exp() = default;
};

class Conversion: public SimpleUnaryFunctionNode
{
public:
    Conversion()
    {
        type = NodeType::Conversion;
        subnodes.resize(1);
    }
    Conversion(std::shared_ptr<Node> n1)
    {
        type = NodeType::Conversion;
        subnodes = {n1};
    }
    Conversion(std::shared_ptr<Node> n1, Datatype d)
        :to(d)
    {
        type = NodeType::Conversion;
        subnodes = {n1};
    }
    Datatype to;
    virtual ~Conversion() = default;
};

class Vect4: public Node
{
public:
	Vect4()
	{
		type = NodeType::Vect4;
		subnodes.resize(5);
	}
	Vect4(
        std::shared_ptr<function_ast::Node> n1, std::shared_ptr<function_ast::Node> n2,
        std::shared_ptr<function_ast::Node> n3, std::shared_ptr<function_ast::Node> n4,
        Datatype dtype
        ) : to_dtype(dtype)
	{
		type = NodeType::Vect4;
		subnodes = {n1, n2, n3, n4};
	}
    Datatype to_dtype;
    virtual std::string generate_source() override;

	virtual ~Vect4() = default;
};

class Extract4: public Node
{
public:
	Extract4()
	{
		type = NodeType::Extract4;
		subnodes.resize(2);
	}
	Extract4( std::shared_ptr<function_ast::Node> n1, Datatype dtype, VectChannelType channel) : from_dtype(dtype), channel(channel)
	{
		type = NodeType::Extract4;
		subnodes = {n1};
	}
    Datatype from_dtype;
    VectChannelType channel;
    virtual std::string generate_source() override;

	virtual ~Extract4() = default;
};

class PixelvalueAtCurrentStencilPos: public Node
{
public:
    PixelvalueAtCurrentStencilPos()
    {
        type = NodeType::PixelvalueAtCurrentStencilPos;
    }
    PixelvalueAtCurrentStencilPos(Node *n1)
    {
        type = NodeType::PixelvalueAtCurrentStencilPos;
        parent = n1;
    }
    Node *parent;
    virtual std::string generate_source() override;
};

class StencilvalueAtCurrentStencilPos: public Node
{
public:
    StencilvalueAtCurrentStencilPos()
    {
        type = NodeType::StencilvalueAtCurrentStencilPos;
        subnodes.resize(1);
    }
    StencilvalueAtCurrentStencilPos(Node *n1)
    {
        type = NodeType::StencilvalueAtCurrentStencilPos;
        parent = n1;
    }
    Node *parent;
    virtual std::string generate_source() override;
};

class ReductionOutput: public Node
{
public:
    ReductionOutput()
    {
        type = NodeType::ReductionOutput;
        subnodes.resize(1);
    }
    ReductionOutput(Node *n1)
    {
        type = NodeType::ReductionOutput;
        parent = n1;
    }
    Node *parent;
    virtual std::string generate_source() override;
};

class IterateAroundPixel: public Node
{
public:
    IterateAroundPixel()
        :pixel_value(new PixelvalueAtCurrentStencilPos(this)),
          stencil_value(new StencilvalueAtCurrentStencilPos(this))
    {
        type = NodeType::IterateAroundPixel;
        subnodes.resize(3);
    }
    IterateAroundPixel(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2, std::shared_ptr<Node> n3)
        :pixel_value(new PixelvalueAtCurrentStencilPos(this)),
          stencil_value(new StencilvalueAtCurrentStencilPos(this))
    {
        type = NodeType::IterateAroundPixel;
        subnodes = {n1, n2, n3};
    }
    std::shared_ptr<PixelvalueAtCurrentStencilPos> pixel_value;
    std::shared_ptr<StencilvalueAtCurrentStencilPos> stencil_value;

    virtual std::string generate_source() override;
};

class ReduceAroundPixel: public Node
{
public:
    enum class Type
    {
        SUM,
        MIN,
        MAX
    };
    ReduceAroundPixel()
        :pixel_value(new PixelvalueAtCurrentStencilPos(this)),
          stencil_value(new StencilvalueAtCurrentStencilPos(this)),
          reduction_output(new ReductionOutput())
    {
        type = NodeType::ReduceAroundPixel;
        subnodes.resize(3);
    }
    ReduceAroundPixel(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2, std::shared_ptr<Node>n3, Type reduction_type)
        :pixel_value(new PixelvalueAtCurrentStencilPos(this)),
          stencil_value(new StencilvalueAtCurrentStencilPos(this)),
          reduction_output(new ReductionOutput()),
          reduction_type(reduction_type)
    {
        type = NodeType::ReduceAroundPixel;
        subnodes = {n1, n2, n3};
    }
    std::shared_ptr<PixelvalueAtCurrentStencilPos> pixel_value;
    std::shared_ptr<StencilvalueAtCurrentStencilPos> stencil_value;
    std::shared_ptr<ReductionOutput> reduction_output;
    Type reduction_type;
    Datatype datatype;

    virtual std::string generate_source() override;
};

template<typename T>
class Constant: public Node
{
public:
    Constant()
    {
        type = NodeType::Constant;
    }
    Constant(T value)
    {
        type = NodeType::Constant;
        this->value = value;
    }
    T value;
    virtual std::string generate_source() override;
};

class Variable: public Node
{
public:
    Variable()
    {
        type = NodeType::Variable;
    }
    Variable(std::string name, Datatype datatype)
    {
        type = NodeType::Variable;
        this->name = name;
        this->datatype = datatype;
    }
    Datatype datatype;
    std::string name;
    virtual std::string generate_source() override;
};

class VariableDefinition: public Node
{
public:
    VariableDefinition()
    {
        type = NodeType::VariableDefinition;
        subnodes.resize(1);
    }
    VariableDefinition(std::shared_ptr<Node> n1)
    {
        type = NodeType::VariableDefinition;
        subnodes = {n1};
    }
    virtual std::string generate_source() override;
};

class Assignment: public Node
{
public:
    Assignment()
    {
        type = NodeType::Assignment;
        subnodes.resize(2);
    }
    Assignment(std::shared_ptr<Node> n1, std::shared_ptr<Node>n2)
    {
        type = NodeType::Assignment;
        subnodes = {n1, n2};
    }
    virtual std::string generate_source() override;
};

class TargetPixel: public Node
{
public:
    TargetPixel()
    {
        type = NodeType::TargetPixel;
        subnodes.resize(1);
    }
    TargetPixel(std::shared_ptr<Node> n1)
    {
        type = NodeType::TargetPixel;
        subnodes = {n1};
    }
    virtual std::string generate_source() override;
};

class Image: public Node
{
public:
    Image()
    {
        type = NodeType::Image;
    }
    Image(HipaVX::Image *image)
    {
        type = NodeType::Image;
        this->image = image;
    }
    HipaVX::Image *image;
    virtual std::string generate_source() override;
};

class Statements: public Node
{
public:
    Statements()
    {
        type = NodeType::Statements;
    }
    std::vector<std::shared_ptr<Node>> statements;
    virtual ~Statements() override = default;
    virtual std::string generate_source() override;
    Statements& operator <<(std::shared_ptr<Node> n)
    {
        statements.push_back(n);
        return *this;
    }
};

class If: public Node
{
public:
	If()
	{
		type = NodeType::If;
	}
	If(std::shared_ptr<Node> c)
	{
		type = NodeType::If;
		condition = c;
	}
    std::shared_ptr<Node> condition;
    Statements body;

    virtual std::string generate_source() override;
};

class Else: public Node
{
public:
    Else()
    {
        type = NodeType::Else;
    }
    Statements body;
    virtual std::string generate_source() override;
};

class CurrentPixelvalue: public Node
{
public:
    CurrentPixelvalue()
    {
        type = NodeType::CurrentPixelvalue;
        subnodes.resize(1);
    }
    CurrentPixelvalue(std::shared_ptr<Node> n1)
    {
        type = NodeType::CurrentPixelvalue;
        subnodes = {n1};
    }
    virtual std::string generate_source() override;
};

class ForEveryPixel: public Node
{
public:
    ForEveryPixel()
    {
        type = NodeType::ForEveryPixel;
    }
    std::vector<std::shared_ptr<Node>> inputs;
    std::shared_ptr<Node> output;
    Statements function;
    virtual std::string generate_source() override;
};

class Stencil: public Node
{
public:
    Stencil()
    {
        type = NodeType::Stencil;
    }
    std::string name;
    std::vector<std::string> mask;
    int dim[2]; //dim[0] = x, dim[1] = y
    Datatype datatype;
    virtual std::string generate_source() override;

    template<typename T>
    static std::vector<std::string> from_t(std::vector<T> v)
    {
        std::vector<std::string> to_return;
        to_return.reserve(v.size());
        for(auto e: v)
            to_return.emplace_back(std::to_string(e));
        return to_return;
    }
};



// TODO: move to ast_gen.hpp
std::string generate(SimpleBinaryNode *s);
std::string generate(SimpleUnaryFunctionNode *s);
std::string generate(Variable *s);
std::string generate(VariableDefinition *s);
std::string generate(Assignment *s);
std::string generate(TargetPixel *s);
std::string generate(Image *s);
std::string generate(Vect4 *s);
std::string generate(Extract4 *s);
std::string generate(ForEveryPixel *s);
std::string generate(ReductionOutput *s);
std::string generate(ReductionType *s);
std::string generate(IterateAroundPixel *s);
std::string generate(ReduceAroundPixel *s);
std::string generate(PixelvalueAtCurrentStencilPos *s);
std::string generate(StencilvalueAtCurrentStencilPos *s);
std::string generate(CurrentPixelvalue *s);
std::string generate(If *s);
std::string generate(Else *s);
std::string generate(Stencil *s);
std::string generate(Statements *s);
std::string generate_call(ForEveryPixel *s);

// TODO: Better name?
template<typename T>
std::string generate(Constant<T> *s)
{
    return std::to_string(s->value);
}

//Templated Node method definitions
template<typename T>
std::string Constant<T>::generate_source()
{
    return generate(this);
}

}


std::shared_ptr<function_ast::Node> operator+(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> operator-(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> operator*(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> operator/(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> operator&(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> operator&&(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> operator|(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> operator||(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> operator^(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> operator!(std::shared_ptr<function_ast::Node> a);

std::shared_ptr<function_ast::Node> operator~(std::shared_ptr<function_ast::Node> a);

std::shared_ptr<function_ast::Node> operator<<(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> shift);

std::shared_ptr<function_ast::Node> operator>>(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> shift);

std::shared_ptr<function_ast::Node> square(std::shared_ptr<function_ast::Node> a);

std::shared_ptr<function_ast::Node> exp(std::shared_ptr<function_ast::Node> a);

std::shared_ptr<function_ast::Node> sqrt(std::shared_ptr<function_ast::Node> a);

std::shared_ptr<function_ast::Node> atan2(std::shared_ptr<function_ast::Node> a);

std::shared_ptr<function_ast::Node> abs(std::shared_ptr<function_ast::Node> a);

std::shared_ptr<function_ast::Node> assign(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> target_pixel(std::shared_ptr<function_ast::Node> a);

std::shared_ptr<function_ast::Node> current_pixel(std::shared_ptr<function_ast::Node> a);

std::shared_ptr<function_ast::Node> convert(std::shared_ptr<function_ast::Node> a, function_ast::Datatype type);

template <typename T>
std::shared_ptr<function_ast::Node> constant(T t)
{
    return std::make_shared<function_ast::Constant<T>>(t);
}

std::shared_ptr<function_ast::Node> define(std::shared_ptr<function_ast::Node> n);

std::shared_ptr<function_ast::If> IF(std::shared_ptr<function_ast::Node> n);

std::shared_ptr<function_ast::Else> ELSE();

std::shared_ptr<function_ast::Node> less(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> less_equal(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> equal(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> greater_equal(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> greater(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

std::shared_ptr<function_ast::Node> vect4(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b, 
                                          std::shared_ptr<function_ast::Node> c, std::shared_ptr<function_ast::Node> d, function_ast::Datatype type );

std::shared_ptr<function_ast::Node> extract4(std::shared_ptr<function_ast::Node> a, function_ast::Datatype type, function_ast::VectChannelType e);

std::shared_ptr<function_ast::Node> unequal(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b);

function_ast::Datatype convert_type(vx_df_image type);

#endif
