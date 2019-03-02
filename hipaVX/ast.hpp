#ifndef __HIPAVX_INTERNAL__
#define __HIPAVX_INTERNAL__

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

namespace HipaVX
{
class Image;
}

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

    Pregenerated,
    PixelAccessor,
    WindowAccessor
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
};

class PixelAccessor: public Node
{
public:
    int num;
    PixelAccessor()
    {
        type = NodeType::PixelAccessor;
    }
    PixelAccessor(int number)
        :num(number)
    {
        type = NodeType::PixelAccessor;
    }
};
class WindowAccesor: public Node
{
public:

    WindowAccesor()
    {
        type = NodeType::WindowAccessor;
    }
};

class SimpleBinaryNode: public Node
{
public:
    virtual ~SimpleBinaryNode() override = default;
};

class SimpleUnaryFunctionNode: public Node
{
public:
    virtual ~SimpleUnaryFunctionNode() override = default;
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
            subnodes.resize(4);
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

    virtual ~Vect4() = default;
};

class Extract4: public Node
{
public:
	Extract4()
	{
		type = NodeType::Extract4;
        subnodes.resize(1);
	}
	Extract4( std::shared_ptr<function_ast::Node> n1, Datatype dtype, VectChannelType channel) : from_dtype(dtype), channel(channel)
	{
		type = NodeType::Extract4;
		subnodes = {n1};
	}
    Datatype from_dtype;
    VectChannelType channel;

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
};

class Statements: public Node
{
public:
    std::vector<std::shared_ptr<PixelAccessor>> pixel_accessors;

    std::shared_ptr<PixelAccessor> pixel_accessor(unsigned int index)
    {
        if (index >= pixel_accessors.size())
            throw std::runtime_error("PixelAccessor* Statements::operator[](unsigned int index): Index out of bounds");
        return pixel_accessors[index];
    }

    Statements()
    {
        type = NodeType::Statements;
    }
    Statements(unsigned int number_mappings)
    {
		type = NodeType::Statements;
        pixel_accessors.resize(number_mappings);
        for(unsigned int i = 0; i < number_mappings; i++)
            pixel_accessors[i].reset(new PixelAccessor(i));
    }
    std::vector<std::shared_ptr<Node>> statements;
    virtual ~Statements() override = default;


    Statements& operator <<(std::shared_ptr<Node> n)
    {
        statements.push_back(n);
        return *this;
    }
    Statements& append(Statements &n)
    {
        //statements.push_back(n);
        statements.insert(std::end(statements), std::begin(n.statements), std::end(n.statements));
        return *this;
    }
    Statements& append(std::shared_ptr<Statements> &n)
    {
        //statements.push_back(n);
        statements.insert(std::end(statements), std::begin(n->statements), std::end(n->statements));
        return *this;
    }
    Statements& replicate(Statements n)
    {
        //statements.push_back(n);
        for (auto it = std::begin(n.statements); it != std::end(n.statements); it++){
            statements.emplace_back(*it);
        }
        return *this;
    }
};

class If: public Node
{
public:
	If()
	{
		type = NodeType::If;
        body = std::make_shared<Statements>();
	}
	If(std::shared_ptr<Node> c)
	{
		type = NodeType::If;
		condition = c;
        body = std::make_shared<Statements>();
	}
    std::shared_ptr<Node> condition;
    std::shared_ptr<Statements> body;
};

class Else: public Node
{
public:
    Else()
    {
        type = NodeType::Else;
        body = std::make_shared<Statements>();
	}
	std::shared_ptr<Statements> body;
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
};

class ForEveryPixel: public Node
{
public:
    ForEveryPixel()
    {
        type = NodeType::ForEveryPixel;
        function = std::make_shared<Statements>();
    }
    std::vector<std::shared_ptr<Node>> inputs;
    std::shared_ptr<Node> output;
	std::shared_ptr<Statements> function;
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

std::shared_ptr<function_ast::Statements> operator<<(std::shared_ptr<function_ast::Statements> a, std::shared_ptr<function_ast::Node> shift);

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


// TODO Own file?

template <class ReturnType, class ParameterType>
class ASTVisitor
{
public:
    virtual ReturnType visit(std::shared_ptr<function_ast::Node> n, ParameterType p) = 0;
    /*  switch(n->type)
        {
        case function_ast::NodeType::None:
            throw std::runtime_error("MapVisitor visited None");
        case function_ast::NodeType::Add:
        case function_ast::NodeType::Sub:
        case function_ast::NodeType::Mul:
        case function_ast::NodeType::Div:
        case function_ast::NodeType::ShiftLeft:
        case function_ast::NodeType::ShiftRight:
        case function_ast::NodeType::Less:
        case function_ast::NodeType::LessEquals:
        case function_ast::NodeType::Equals:
        case function_ast::NodeType::GreaterEquals:
        case function_ast::NodeType::Greater:
        case function_ast::NodeType::Unequals:
        case function_ast::NodeType::And:
        case function_ast::NodeType::Or:
        case function_ast::NodeType::Xor:
        case function_ast::NodeType::BitwiseAnd:
        case function_ast::NodeType::BitwiseOr:
        case function_ast::NodeType::BitwiseXor:
        case function_ast::NodeType::BitwiseNot:
        case function_ast::NodeType::Not:
        case function_ast::NodeType::Sqrt:
        case function_ast::NodeType::Exp:
        case function_ast::NodeType::Conversion:
        case function_ast::NodeType::Abs:
        case function_ast::NodeType::Atan2:
        case function_ast::NodeType::Constant:
        case function_ast::NodeType::Vect4:
        case function_ast::NodeType::Extract4:
        case function_ast::NodeType::Variable:
        case function_ast::NodeType::VariableDefinition:
        case function_ast::NodeType::Assignment:
        case function_ast::NodeType::TargetPixel:
        case function_ast::NodeType::If:
        case function_ast::NodeType::Else:
        case function_ast::NodeType::Image:
        case function_ast::NodeType::ForEveryPixel:
        case function_ast::NodeType::CurrentPixelvalue:
        case function_ast::NodeType::Stencil:
        case function_ast::NodeType::IterateAroundPixel:
        case function_ast::NodeType::ReductionOutput:
        case function_ast::NodeType::ReduceAroundPixel:
        case function_ast::NodeType::PixelvalueAtCurrentStencilPos:
        case function_ast::NodeType::StencilvalueAtCurrentStencilPos:
        case function_ast::NodeType::Statements:
        case function_ast::NodeType::Pregenerated:
        case function_ast::NodeType::PixelAccessor:
        case function_ast::NodeType::WindowAccessor:
        default:
            break;
        }*/
};
#endif
