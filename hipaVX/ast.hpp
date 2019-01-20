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

	And,
	Or,
	Xor,
	Not,

	BitwiseAnd,
	BitwiseOr,
	BitwiseXor,
	BitwiseNot,

    Sqrt,
    Square,
    Exp,
    Conversion,

    Constant,
    Variable,
    VariableDefinition,
    Assignment,
    TargetPixel,

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
    INT32,
    UINT32,
    FLOAT,
    UINT8,
    UINT16,
    INT16
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
class Square: public SimpleUnaryFunctionNode
{
public:
    Square()
    {
        type = NodeType::Square;
        subnodes.resize(1);
    }
    Square(std::shared_ptr<Node> n1)
    {
        type = NodeType::Square;
        subnodes = {n1};
    }
    virtual ~Square() = default;
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




std::string generate(SimpleBinaryNode *s);
std::string generate(SimpleUnaryFunctionNode *s);
std::string generate(Variable *s);
std::string generate(VariableDefinition *s);
std::string generate(Assignment *s);
std::string generate(TargetPixel *s);
std::string generate(Image *s);
std::string generate(ForEveryPixel *s);
std::string generate(ReductionOutput *s);
std::string generate(ReductionType *s);
std::string generate(IterateAroundPixel *s);
std::string generate(ReduceAroundPixel *s);
std::string generate(PixelvalueAtCurrentStencilPos *s);
std::string generate(StencilvalueAtCurrentStencilPos *s);
std::string generate(CurrentPixelvalue *s);
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


std::shared_ptr<function_ast::Node> operator+(std::shared_ptr<function_ast::Node> a,
                              std::shared_ptr<function_ast::Node> b);
std::shared_ptr<function_ast::Node> operator-(std::shared_ptr<function_ast::Node> a,
                              std::shared_ptr<function_ast::Node> b);
std::shared_ptr<function_ast::Node> operator*(std::shared_ptr<function_ast::Node> a,
                              std::shared_ptr<function_ast::Node> b);
std::shared_ptr<function_ast::Node> operator/(std::shared_ptr<function_ast::Node> a,
                              std::shared_ptr<function_ast::Node> b);
std::shared_ptr<function_ast::Node> operator&(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b);
std::shared_ptr<function_ast::Node> operator&&(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b);
std::shared_ptr<function_ast::Node> operator|(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b);
std::shared_ptr<function_ast::Node> operator||(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b);
std::shared_ptr<function_ast::Node> operator^(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b);
std::shared_ptr<function_ast::Node> operator!(std::shared_ptr<function_ast::Node> a);
std::shared_ptr<function_ast::Node> operator~(std::shared_ptr<function_ast::Node> a);

std::shared_ptr<function_ast::Node> square(std::shared_ptr<function_ast::Node> a);
std::shared_ptr<function_ast::Node> exp(std::shared_ptr<function_ast::Node> a);
std::shared_ptr<function_ast::Node> assign(std::shared_ptr<function_ast::Node> a,
                                           std::shared_ptr<function_ast::Node> b);
std::shared_ptr<function_ast::Node> target_pixel(std::shared_ptr<function_ast::Node> a);

std::shared_ptr<function_ast::Node> current_pixel(std::shared_ptr<function_ast::Node> a);
std::shared_ptr<function_ast::Node> convert(std::shared_ptr<function_ast::Node> a,
                                            function_ast::Datatype type);
template <typename T>
std::shared_ptr<function_ast::Node> constant(T t)
{
    return std::make_shared<function_ast::Constant<T>>(t);
}
std::shared_ptr<function_ast::Node> define(std::shared_ptr<function_ast::Node> n);


function_ast::Datatype convert_type(vx_df_image type);


#endif
