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

namespace ast4vx
{

// TODO: Consider different enum types for abstractions and scalar operations
/**
 * @brief The different types of AST Nodes
 */
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
    VariableDeclaration,
    Assignment,

    If,
    Else,

    Statements,
    LocalToPixel,
    PixelToPixel,

    VariableAccessor,
    PixelAccessor,
    WindowAccessor,
    WindowDescriptor,
    WindowAccessorPosition,
    WindowOperation,
    Reduction,
    MaskPixelToPixel,
    MaskAccessor,
};

/**
 * @brief The Datatype enum.
 *
 * It lists all currently supported datatypes in the AST
 */
enum class Datatype
{
    UCHAR,
    INT32,
    UINT32,
    FLOAT,
    UINT8,
    INT8,
    UINT16,
    INT16,

    UCHAR4,
    UINT4,
};

/**
 * @brief The VectChannelType enum describes the channels for a vectorlike datatype
 */
enum class VectChannelType
{
    CHANNEL0,
    CHANNEL1,
    CHANNEL2,
    CHANNEL3
};


/**
 * @brief The fundamental AST Node class
 */
class Node
{
public:
    int id; /**< This gets set when constructing a new Node Instance. This is read only */
    Node()
    {
        static int next_id = 0;
        id = next_id++;
    }
    NodeType type = NodeType::None; /**< Denotes which type this AST Node is */
    std::vector<std::shared_ptr<Node>> subnodes; /**< Many of the AST Nodes parameters and subnodes are stored in here */

    /**
     * @brief operator []
     * @return A reference to the std::shared_ptr<Node> at the corresponding index
     * @throws Throws a std::runtime_error when the index is out of bounds
     */
    std::shared_ptr<Node>& operator[](unsigned int index)
    {
        if (index >= subnodes.size())
            throw std::runtime_error("Node::operator[]: index out of bounds");
        return subnodes[index];
    }
    /**
     * @brief operator []
     * @return A copy of the std::shared_ptr<Node> at the corresponding index
     * @throws Throws a std::runtime_error when the index is out of bounds
     */
    std::shared_ptr<Node> operator[](unsigned int index) const
    {
        if (index >= subnodes.size())
            throw std::runtime_error("Node::operator[]: index out of bounds");
        return subnodes[index];
    }

    virtual ~Node() = default;
};

/**
 * @brief The VariableAccessor class
 *
 * It is a placeholder node, which gets linked in the Abstraction Layers to the actual values and instances
 */
class VariableAccessor: public Node
{
public:
    int num;
    VariableAccessor()
    {
        type = NodeType::VariableAccessor;
    }
    VariableAccessor(int number)
        :num(number)
    {
        type = NodeType::VariableAccessor;
    }
};

/**
 * @brief The PixelAccessor class
 *
 * It is a placeholder node, which gets linked in the Abstraction Layers to the actual values and instances
 */
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

/**
 * @brief The SimpleBinaryNode class
 *
 * It represents all simple binary operations like addition, subtraction, etc..\n
 * The left argument has to be stored in subnodes[0] and the right argument in subnodes[1].
 * The actual operation is deduced from the Nodes type
 */
class SimpleBinaryNode: public Node
{
public:
    virtual ~SimpleBinaryNode() override = default;
};

/**
 * @brief Addition
 */
class Add: public SimpleBinaryNode
{
public:
    Add()
    {
        type = NodeType::Add;
        subnodes.resize(2);
    }
    Add(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::Add;
        subnodes = {n1, n2};
    }

    virtual ~Add() = default;
};

/**
 * @brief Subtraction
 */
class Sub: public SimpleBinaryNode
{
public:
    Sub()
    {
        type = NodeType::Sub;
        subnodes.resize(2);
    }
    Sub(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::Sub;
        subnodes = {n1, n2};
    }

    virtual ~Sub() = default;
};

/**
 * @brief Multiplication
 */
class Mul: public SimpleBinaryNode
{
public:
    Mul()
    {
        type = NodeType::Mul;
        subnodes.resize(2);
    }
    Mul(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::Mul;
        subnodes = {n1, n2};
    }

    virtual ~Mul() = default;
};

/**
 * @brief Division
 */
class Div: public SimpleBinaryNode
{
public:
    Div()
    {
        type = NodeType::Div;
        subnodes.resize(2);
    }
    Div(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::Div;
        subnodes = {n1, n2};
    }

    virtual ~Div() = default;
};

/**
 * @brief Left shifting
 */
class ShiftLeft: public SimpleBinaryNode
{
public:
    ShiftLeft()
    {
        type = NodeType::ShiftLeft;
        subnodes.resize(2);
    }
    ShiftLeft(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::ShiftLeft;
        subnodes = {n1, n2};
    }

    virtual ~ShiftLeft() = default;
};

/**
 * @brief Right shifting
 */
class ShiftRight: public SimpleBinaryNode
{
public:
    ShiftRight()
    {
        type = NodeType::ShiftRight;
        subnodes.resize(2);
    }
    ShiftRight(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::ShiftRight;
        subnodes = {n1, n2};
    }

    virtual ~ShiftRight() = default;
};

/**
 * @brief Logical AND operation
 */
class And: public SimpleBinaryNode
{
public:
	And()
	{
		type = NodeType::And;
		subnodes.resize(2);
	}
    And(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
	{
		type = NodeType::And;
		subnodes = {n1, n2};
	}

	virtual ~And() = default;
};

/**
 * @brief Logical OR operation
 */
class Or: public SimpleBinaryNode
{
public:
	Or()
	{
		type = NodeType::Or;
		subnodes.resize(2);
	}
    Or(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
	{
		type = NodeType::Or;
		subnodes = {n1, n2};
	}

	virtual ~Or() = default;
};

/**
 * @brief Logical XOR operation
 */
class Xor: public SimpleBinaryNode
{
public:
	Xor()
	{
		type = NodeType::Xor;
		subnodes.resize(2);
	}
    Xor(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
	{
		type = NodeType::Xor;
		subnodes = {n1, n2};
	}

	virtual ~Xor() = default;
};

/**
 * @brief Bitwise AND operation
 */
class BitwiseAnd: public SimpleBinaryNode
{
public:
    BitwiseAnd()
    {
        type = NodeType::BitwiseAnd;
        subnodes.resize(2);
    }
    BitwiseAnd(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::BitwiseAnd;
        subnodes = {n1, n2};
    }

    virtual ~BitwiseAnd() = default;
};

/**
 * @brief Bitwise Or operation
 */
class BitwiseOr: public SimpleBinaryNode
{
public:
    BitwiseOr()
    {
        type = NodeType::BitwiseOr;
        subnodes.resize(2);
    }
    BitwiseOr(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::BitwiseOr;
        subnodes = {n1, n2};
    }

    virtual ~BitwiseOr() = default;
};

/**
 * @brief Bitwise XOR operation
 */
class BitwiseXor: public SimpleBinaryNode
{
public:
    BitwiseXor()
    {
        type = NodeType::BitwiseXor;
        subnodes.resize(2);
    }
    BitwiseXor(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::BitwiseXor;
        subnodes = {n1, n2};
    }

    virtual ~BitwiseXor() = default;
};

/**
 * @brief Less comparision
 */
class Less: public SimpleBinaryNode
{
public:
    Less()
    {
        type = NodeType::Less;
        subnodes.resize(2);
    }
    Less(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::Less;
        subnodes = {n1, n2};
    }

    virtual ~Less() = default;
};

/**
 * @brief Less equals comparision
 */
class LessEquals: public SimpleBinaryNode
{
public:
    LessEquals()
    {
        type = NodeType::LessEquals;
        subnodes.resize(2);
    }
    LessEquals(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::LessEquals;
        subnodes = {n1, n2};
    }

    virtual ~LessEquals() = default;
};

/**
 * @brief Equals comparision
 */
class Equals: public SimpleBinaryNode
{
public:
    Equals()
    {
        type = NodeType::Equals;
        subnodes.resize(2);
    }
    Equals(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::Equals;
        subnodes = {n1, n2};
    }

    virtual ~Equals() = default;
};

/**
 * @brief Greater equals comparision
 */
class GreaterEquals: public SimpleBinaryNode
{
public:
    GreaterEquals()
    {
        type = NodeType::GreaterEquals;
        subnodes.resize(2);
    }
    GreaterEquals(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::GreaterEquals;
        subnodes = {n1, n2};
    }

    virtual ~GreaterEquals() = default;
};

/**
 * @brief Greater comparision
 */
class Greater: public SimpleBinaryNode
{
public:
    Greater()
    {
        type = NodeType::Greater;
        subnodes.resize(2);
    }
    Greater(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::Greater;
        subnodes = {n1, n2};
    }

    virtual ~Greater() = default;
};

/**
 * @brief Unequals comparision
 */
class Unequals: public SimpleBinaryNode
{
public:
    Unequals()
    {
        type = NodeType::Unequals;
        subnodes.resize(2);
    }
    Unequals(std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2)
    {
        type = NodeType::Unequals;
        subnodes = {n1, n2};
    }

    virtual ~Unequals() = default;
};

/**
 * @brief The SimpleUnaryFunctionNode class
 *
 * It represents all simple unary functions like not, sqrt, etc..\n
 * The argument has to be stored in subnodes[0].
 * The actual operation/function is deduced from the Nodes type
 */
class SimpleUnaryFunctionNode: public Node
{
public:
    virtual ~SimpleUnaryFunctionNode() override = default;
};

/**
 * @brief Logical not operation
 */
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

/**
 * @brief Bitwise equals comparision
 */
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

/**
 * @brief Squareroot operation
 */
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

/**
 * @brief Absolute value operation
 */
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

/**
 * @todo Is this correct? Atan2 needs 2 parameters?
 */
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

/**
 * @brief Takes the value to the power of e
 */
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

/**
 * @brief Represents a conversion operation
 *
 * The conversion argument has to be stored in subnods[0], as all other SimpleUnaryFunctioNode
 * Its destination type has to be written into the member variable Conversion::to
 */
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

/**
 * @brief The Vect4 class creates a vector with 4 entries
 *
 * The entries are in the Node::subnodes member variable and the datatype is set by Vect4::to_dtype
 */
class Vect4: public Node
{
public:
    Vect4()
    {
            type = NodeType::Vect4;
            subnodes.resize(4);
    }
    Vect4(
    std::shared_ptr<ast4vx::Node> n1, std::shared_ptr<ast4vx::Node> n2,
    std::shared_ptr<ast4vx::Node> n3, std::shared_ptr<ast4vx::Node> n4,
    Datatype dtype
    ) : to_dtype(dtype)
    {
            type = NodeType::Vect4;
            subnodes = {n1, n2, n3, n4};
    }
    Datatype to_dtype;

    virtual ~Vect4() = default;
};

/**
 * @brief The Extract4 class extracts a single channel/entry
 *
 * The source Vect4 is stored in subnodes[0] and the channel is in Extract4::channel
 */
class Extract4: public Node
{
public:
	Extract4()
	{
		type = NodeType::Extract4;
        subnodes.resize(1);
	}
    Extract4( std::shared_ptr<ast4vx::Node> n1, VectChannelType channel)
        :channel(channel)
	{
		type = NodeType::Extract4;
		subnodes = {n1};
    }
    VectChannelType channel;

	virtual ~Extract4() = default;
};

/**
 * @brief Represents a Constant in the AST
 *
 * The actual value of the constant is stored inside Constant::value
 */
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

/**
 * @brief The Variable class represents a conventional variable.
 *
 * The Variables name is stored in Variable::name and its Datatype is in Variable::datatype
 */
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

/**
 * @brief Defined a Variable in the AST
 *
 * It represents the point of time when a variable gets declared.
 * The corresponding Variable has to be stored in Node::subnodes[0]
 */
class VariableDeclaration: public Node
{
public:
    VariableDeclaration()
    {
        type = NodeType::VariableDeclaration;
        subnodes.resize(1);
    }
    VariableDeclaration(std::shared_ptr<Node> n1)
    {
        type = NodeType::VariableDeclaration;
        subnodes = {n1};
    }
};

/**
 * @brief The Assignment assigns a right hand expression to a left hand value.
 *
 * The left hand values has to be stored in Node::subnodes[0] and the right hand value expression in Node::subnodes[1]
 * @todo Use SimpleBinaryNode here aswell?
 */
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

/**
 * @brief Abstract accessor class for a window
 *
 * It is a placeholder node, which gets linked in the Abstraction Layers to the actual values and instances
 */
class WindowAccessor: public Node, public std::enable_shared_from_this<WindowAccessor>
{
public:
    /**
     * @brief Abstract accessor class for a position in a window
     *
     * It is a placeholder node, which gets linked in the Abstraction Layers to the actual values and instances
     */
    class Position: public Node
    {
    public:
        unsigned int x, y;
        std::weak_ptr<WindowAccessor> parent;
    private:
        Position(std::shared_ptr<WindowAccessor> parent)
            :parent(parent)
        {
            type = NodeType::WindowAccessorPosition;
        }
        Position(unsigned int x, unsigned int y, std::shared_ptr<WindowAccessor> parent)
            :x(x), y(y), parent(parent)
        {
            type = NodeType::WindowAccessorPosition;
        }
        friend class WindowAccessor;
    };
    std::vector<std::shared_ptr<Position>> accessors;

    int num;
    WindowAccessor()
    {
        type = NodeType::WindowAccessor;
    }
    WindowAccessor(int number)
        :num(number)
    {
        type = NodeType::WindowAccessor;
    }

    /**
     * @brief pixel_at Creates a placeholder node at x,y if it does not exist already and returns it
     * @param x X Coordinate (0-indexed)
     * @param y Y Coordinate (0-indexed)
     * @return A placeholder node for the corresponding pixel at the current coordinate in the window
     */
    std::shared_ptr<Position> pixel_at(unsigned int x, unsigned int y)
    {
        auto it = std::find_if(accessors.begin(), accessors.end(), [x, y] (std::shared_ptr<Position> m)
        {
            return m->x == x && m->y == y;
        });
        if (it != accessors.end())
            return *it;

        auto m = std::shared_ptr<Position>(new Position(x, y, shared_from_this()));
        accessors.emplace_back(m);
        return m;
    }
};

/**
 * @brief Packs AST Nodes together into many statements, which work on a pixel basis
 *
 * This is one of the main nodes of the AST.\n
 * It provides different accessors for the AST and saves statements, which can be any AST Node.\n
 * A Statement therefore should be a self contained AST Operation. This can be imagined as a line in C or C++.
 *
 * @todo Rename to PixelToPixel?
 */
class Statements: public Node
{
public:
    std::vector<std::shared_ptr<PixelAccessor>> out_pixel_mappings;
    std::vector<std::shared_ptr<PixelAccessor>> in_pixel_mappings;

    std::vector<std::shared_ptr<VariableAccessor>> out_variable_mappings;
    std::vector<std::shared_ptr<VariableAccessor>> in_variable_mappings;

    std::vector<std::shared_ptr<Node>> statements;
public:
    /**
     * @brief d_in Accessing a (d)ata (in)put pixel mapping
     * @param index input index of the data input pixel mapping
     * @return A std::shared_ptr<PixelAccessor>
     * @throws std::runtime_error when index is out of bounds
     */
    std::shared_ptr<PixelAccessor> d_in(unsigned int index)
    {
        if (index >= in_pixel_mappings.size())
            throw std::runtime_error("std::shared_ptr<PixelAccessor> Statements::d_in(unsigned int index): Index out of bounds");
        return in_pixel_mappings[index];
    }
    /**
     * @brief d_out Accessing a (d)ata (out)put pixel mapping
     * @param index output index of the data output pixel mapping
     * @return A std::shared_ptr<PixelAccessor>
     * @throws std::runtime_error when index is out of bounds
     */
    std::shared_ptr<PixelAccessor> d_out(unsigned int index)
    {
        if (index >= out_pixel_mappings.size())
            throw std::runtime_error("std::shared_ptr<PixelAccessor> Statements::d_in(unsigned int index): Index out of bounds");
        return out_pixel_mappings[index];
    }

    /**
     * @brief v_in Accessing a (v)ariable (in)put mapping
     * @param index input index of the variable input mapping
     * @return A std::shared_ptr<VariableAccessor>
     * @throws std::runtime_error when index is out of bounds
     */
    std::shared_ptr<VariableAccessor> v_in(unsigned int index)
    {
        if (index >= in_variable_mappings.size())
            throw std::runtime_error("std::shared_ptr<VariableAccessor> Statements::v_in(unsigned int index): Index out of bounds");
        return in_variable_mappings[index];
    }
    /**
     * @brief v_out Accessing a (v)ariable (out)put mapping
     * @param index output index of the variable output mapping
     * @return A std::shared_ptr<VariableAccessor>
     * @throws std::runtime_error when index is out of bounds
     */
    std::shared_ptr<VariableAccessor> v_out(unsigned int index)
    {
        if (index >= out_variable_mappings.size())
            throw std::runtime_error("std::shared_ptr<VariableAccessor> Statements::v_in(unsigned int index): Index out of bounds");
        return out_variable_mappings[index];
    }

    Statements()
    {
        type = NodeType::Statements;
    }
    /**
     * @param out Amount of data output pixel accessors
     * @param in Amount of data input pixel accessors
     */
    Statements(unsigned int out, unsigned int in)
    {
        type = NodeType::Statements;
        set_data_inout_count(out, in);
    }
    virtual ~Statements() override = default;

    /**
     * @brief Sets amount of data in/out pixel accessors
     * @param out Amount of data output pixel accessors
     * @param in Amount of data input pixel accessors
     */
    void set_data_inout_count(unsigned int out, unsigned int in)
    {
        out_pixel_mappings.resize(out);
        in_pixel_mappings.resize(in);

        unsigned int i = 0;
        for(; i < out; i++)
            out_pixel_mappings[i].reset(new PixelAccessor(i));

        for(; i < out + in; i++)
            in_pixel_mappings[i-out].reset(new PixelAccessor(i));
    }
    /**
     * @brief Sets amount of variable in/out accessors
     * @param out Amount of variable accessors
     * @param in Amount of variable accessors
     */
    void set_variable_inout_count(unsigned int out, unsigned int in)
    {
        out_variable_mappings.resize(out);
        in_variable_mappings.resize(in);

        unsigned int i = 0;
        for(; i < out; i++)
            out_variable_mappings[i].reset(new VariableAccessor(i));

        for(; i < out + in; i++)
            in_variable_mappings[i-out].reset(new VariableAccessor(i));
    }

    /**
     * @brief Adds a statement at the end of the statements vector.
     * @param n The top level AST Node, which should be treated as a statement
     * @return Reference to *this in order allow chaining of the operator
     */
    Statements& operator <<(std::shared_ptr<Node> n)
    {
        statements.push_back(n);
        return *this;
    }
    /**
     * @todo Akif, do we need this?
     */
    Statements& append(Statements &n)
    {
        //statements.push_back(n);
        statements.insert(std::end(statements), std::begin(n.statements), std::end(n.statements));
        return *this;
    }
    /**
     * @todo Akif, do we need this?
     */
    Statements& append(std::shared_ptr<Statements> &n)
    {
        //statements.push_back(n);
        statements.insert(std::end(statements), std::begin(n->statements), std::end(n->statements));
        return *this;
    }
    /**
     * @todo Akif, do we need this?
     */
    Statements& replicate(Statements n)
    {
        //statements.push_back(n);
        for (auto it = std::begin(n.statements); it != std::end(n.statements); it++){
            statements.emplace_back(*it);
        }
        return *this;
    }
};

/**
 * @brief Packs AST Nodes together into many statements, which work on a local to pixel basis
 *
 * It provides different accessors for the AST and saves statements, which can be any AST Node.\n
 * A Statement therefore should be a self contained AST Operation. This can be imagined as a line in C or C++.
 * Its difference to Statements is that it has input windows and output data pixel accessors
 *
 */
class LocalToPixel: public Statements
{
public:
    std::vector<std::shared_ptr<WindowAccessor>> windows;
    /**
     * @param num_output_images Amount of data output pixel accessors
     * @param num_windows Amount of input window accessors
     */
    LocalToPixel(unsigned int num_output_images, unsigned int num_windows)
        :Statements(num_output_images, 0)
    {
        type = NodeType::LocalToPixel;
        windows.resize(num_windows);
        for(unsigned int i = 0; i < windows.size(); i++)
            windows[i].reset(new WindowAccessor(i));
    }

    /**
     * @brief w_in Accessing a (w)indow (in)put mapping
     * @param index input index of the window input mapping
     * @return A std::shared_ptr<WindowAccessor>
     * @throws std::runtime_error when index is out of bounds
     */
    std::shared_ptr<WindowAccessor> w_in(unsigned int index)
    {
        if (index >= windows.size())
            throw std::runtime_error("LocalToPixel::w_in(unsigned int index): Index out of bounds");
        return windows[index];
    }
};

/**
 * @brief Abstract accessor class for a Mask
 *
 * It is a placeholder node, which gets linked in the Abstraction Layers to the actual values and instances
 */
class MaskAccessor: public Node
{
public:
    int num;
    MaskAccessor()
    {
        type = NodeType::MaskAccessor;
    }
    MaskAccessor(int number)
        :num(number)
    {
        type = NodeType::MaskAccessor;
    }
};

/**
 * @brief Packs AST Nodes together into many statements, which work on a pixel to pixel basis with mask values
 *
 * It provides different accessors for the AST and saves statements, which can be any AST Node.\n
 * A Statement therefore should be a self contained AST Operation. This can be imagined as a line in C or C++.
 * Its difference to Statements is that it has input masks additionally
 *
 */
class MaskPixelToPixel: public Statements
{
public:
    std::vector<std::shared_ptr<MaskAccessor>> masks;

    /**
     * @param num_output_images Amount of data output pixel accessors
     * @param num_input_images Amount of data input pixel accessors
     * @param num_masks Amount of input mask accessors
     */
    MaskPixelToPixel(unsigned int num_output_images, unsigned int num_input_images, unsigned int num_masks)
        :Statements(num_output_images, num_input_images)
    {
        type = NodeType::MaskPixelToPixel;
        masks.resize(num_masks);
        for(unsigned int i = 0; i < masks.size(); i++)
            masks[i].reset(new MaskAccessor(i));
    }

    /**
     * @brief m_in Accessing a (m)ask (in)put mapping, it corresponds to the correct current mask value of the mask
     * @param index input index of the mask input mapping
     * @return A std::shared_ptr<MaskAccessor>
     * @throws std::runtime_error when index is out of bounds
     */
    std::shared_ptr<MaskAccessor> m_in(unsigned int index)
    {
        if (index >= masks.size())
            throw std::runtime_error("MaskPixelToPixel::m_in(unsigned int index): Index out of bounds");
        return masks[index];
    }
};

/**
 * @brief Packs AST Nodes together into many statements, which abstracts a reduction operation
 *
 * It provides three accessors: Reduction::left(), Reduction::right() and Reduction::out()
 */
class Reduction: public Statements
{
public:
    std::shared_ptr<Node> initial;

    /**
     * @brief Constructs a new Reduction operation with the inial value of c
     * @param c Inital constant value
     */
    template <class T>
    Reduction(Constant<T> c = Constant<T>())
        :Statements(1,2)
    {
        type = NodeType::Reduction;
        initial = std::make_shared<Constant<T>>(c);
    }

    std::shared_ptr<PixelAccessor> left()
    {
        return d_in(0);
    }
    std::shared_ptr<PixelAccessor> right()
    {
        return d_in(1);
    }
    std::shared_ptr<PixelAccessor> out()
    {
        return d_out(0);
    }
};

/**
 * @brief Placeholder in the AST for a Window with a specific datatype
 *
 * It is a placeholder node, which gets linked in the Abstraction Layers to the actual values and instances
 *
 */
class WindowDescriptor: public Node
{
public:
    unsigned int width, height;
    Datatype output_datatype;

    WindowDescriptor(unsigned int x, unsigned int y, Datatype datatype = Datatype::INT32)
    {
        if (y <= 0 || x <= 0)
            throw std::runtime_error("WindowDescriptor::WindowDescriptor domain size must be > 0");

        type = NodeType::WindowDescriptor;

        this->output_datatype = datatype;
        width = x;
        height = y;
    }
};

/**
 * @brief Describes an operation on a Window
 *
 * A WindowOperation can only be in one WindowOperation::State\n
 */
class WindowOperation: public Node
{
private:
    bool explicit_x_y;
public:
    enum class State
    {
        None,
        At,
        Forall,
        ToPixel,
        Reduce,
    };

    State current_state = State::None;
    std::shared_ptr<WindowDescriptor> output;
    std::shared_ptr<LocalToPixel> ltp_statement;
    std::shared_ptr<Statements> forall_statement;
    std::shared_ptr<Reduction> reduction_statement;

    std::vector<std::shared_ptr<WindowDescriptor>> window_inputs;
public:
    std::vector<std::vector<std::shared_ptr<Statements>>> statements;

    /**
     * @brief Constructor with an explicit domain size
     * @param x domain width
     * @param y domain height
     */
    WindowOperation(unsigned int x, unsigned int y)
    {
        if (y <= 0 || x <= 0)
            throw std::runtime_error("WindowOperation::WindowOperation: domain size must be > 0");

        type = NodeType::WindowOperation;
        statements.resize(y);
        for(unsigned int i = 0; i < statements.size(); i++)
        {
            statements[i].resize(x);
        }
        explicit_x_y = true;
    }
    /**
     * @brief Constructor without an explicit domain size
     *
     * The domain size will get inferred from WindowOperation::set_window_inputs
     */
    WindowOperation()
    {
        type = NodeType::WindowOperation;
        explicit_x_y = false;
    }

    /**
     * @brief Sets the WindowDescriptors as inputs to this Operation
     * @param in The whole input list for this WindowOperation
     */
    void set_window_inputs(std::initializer_list<std::shared_ptr<WindowDescriptor>> in)
    {
        window_inputs.clear();
        for(auto wd: in)
            window_inputs.emplace_back(wd);

        if (!explicit_x_y)
        {
            if (window_inputs.size() == 0)
                throw std::runtime_error("WindowOperation::set_window_inputs(): If the WindowOperation size is not explicitly set, at least one input window must be given");

            statements.resize(window_inputs[0]->height);
            for(unsigned int i = 0; i < statements.size(); i++)
            {
                statements[i].resize(window_inputs[0]->width);
            }
        }
    }

    /**
     * @brief Computes a PixelToPixel Statement at a specific given coordinate in the domain
     * @param x X Coordinate in the domain (0-indexed)
     * @param y Y Coordinate in the domain (0-indexed)
     * @param s The PixelToPixel Statement that should be computed at the given coordinates
     * @throws std::runtime_error will be thrown when this WindowOperation is already in another state. See brief description of this class
     * @throws std::runtime_error will be thrown when the domain size is not known yet. This happens when the size is not set explicitly and WindowOperation::set_window_inputs() haven't been called yet with at least one WindowDescriptor
     * @throws std::runtime_error will be thrown when the coordinates are out of bounds
     * @throws std::runtime_error will be thrown when a PixelToPixel Statement already is bound to that coordinate
     */
    void compute_at(unsigned int x, unsigned int y, std::shared_ptr<Statements> s)
    {
        if (current_state != State::None && current_state != State::At)
            throw std::runtime_error("WindowOperation::compute_at(): Window already in another state");
        if (!explicit_x_y && statements.size() == 0)
            throw std::runtime_error("WindowOperation::compute_at(): If the WindowOperation size is not explicitly set, set_window_inputs(...) needs to get called before at(...)");
        if (y >= statements.size() || x >= statements[x].size())
            throw std::runtime_error("WindowOperation::compute_at(): Index out of bounds");
        if (statements[y][x] != nullptr)
            throw std::runtime_error("WindowOperation::compute_at(): Coordinate already written to");

        current_state = State::At;
        statements[y][x] = s;
    }

    /**
     * @brief Computes a PixelToPixel Statement at all in the domain
     * @param s The PixelToPixel Statement that should be computed for the whole domain
     * @throws std::runtime_error will be thrown when this WindowOperation is already in another state. See brief description of this class
     * @throws std::runtime_error will be thrown when a PixelToPixel Statement already is bound for this WindowOperation
     */
    void forall(std::shared_ptr<Statements> s)
    {
        if (current_state != State::None && current_state != State::Forall)
            throw std::runtime_error("WindowOperation::forall(): Window already in another state");
        if (forall_statement != nullptr)
            throw std::runtime_error("WindowOperation::forall(): Already written");

        current_state = State::Forall;
        forall_statement = s;
    }

    /**
     * @brief Computes a reduction statement for the values in the domain
     * @param s The Reduction Statement that computes the final value
     * @throws std::runtime_error will be thrown when this WindowOperation is already in another state. See brief description of this class
     * @throws std::runtime_error will be thrown when a Reduction Statement already is bound for this WindowOperation
     */
    void reduce(std::shared_ptr<Reduction> s)
    {
        if (current_state != State::None && current_state != State::Reduce)
            throw std::runtime_error("WindowOperation::reduce(): Window already in another state");
        if (reduction_statement != nullptr)
            throw std::runtime_error("WindowOperation::reduce(): Already written");

        current_state = State::Reduce;
        reduction_statement = s;
    }

    /**
     * @brief Computes a custom reduction statement via the LocalToPixel AST Node
     * @param s The LocalToPixel Statement that computes the final value
     * @throws std::runtime_error will be thrown when this WindowOperation is already in another state. See brief description of this class
     * @throws std::runtime_error will be thrown when a LocalToPixel Statement already is bound for this WindowOperation
     */
    void to_pixel(std::shared_ptr<LocalToPixel> s)
    {
        if (current_state != State::None && current_state != State::ToPixel)
            throw std::runtime_error("WindowOperation::to_pixel(): Window already in another state");
        if (ltp_statement != nullptr)
            throw std::runtime_error("WindowOperation::to_pixel(): Already written");

        current_state = State::ToPixel;
        ltp_statement = s;
    }

    /**
     * @brief Gets the output window of this WindowOperation if one exists
     * @throws std::runtime_error will be thrown when this WindowOperation is not in the WindowOperation::State::At or WindowOperation::State::Forall state, since no output window would exist
     */
    std::shared_ptr<WindowDescriptor> get_window_output()
    {
        if (current_state != State::At && current_state != State::Forall)
            throw std::runtime_error("WindowOperation::get_window_output(): Window not in the right state");
        if (output.get() == nullptr)
            output = std::make_shared<WindowDescriptor>(statements[0].size(), statements.size());

        return output;
    }
};

/**
 * @brief The if Node in the AST
 *
 * The conditional top AST Node has to be written to If::condition and the body in If::body
 */
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

/**
 * @brief The else Node in the AST
 *
 * The body has to be writtin to Else::body
 */
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

}

/**
 * @brief Creates a new ast4vx::Add Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> operator+(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::Sub Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> operator-(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::Mul Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> operator*(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::Div Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> operator/(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::And Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> operator&(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::BitwiseAnd Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> operator&&(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::Or Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> operator|(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::BitwiseOr Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> operator||(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::BitwiseXor Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> operator^(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::Not Node with \p a as the parameter and returns it
 */
std::shared_ptr<ast4vx::Node> operator!(std::shared_ptr<ast4vx::Node> a);

/**
 * @brief Creates a new ast4vx::BitwiseNot Node with \p a as the parameter and returns it
 */
std::shared_ptr<ast4vx::Node> operator~(std::shared_ptr<ast4vx::Node> a);

/**
 * @brief Creates a new ast4vx::ShiftLeft Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> operator<<(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> shift);

/**
 * @brief Adds the \p statement to the statements of \p a
 */
std::shared_ptr<ast4vx::Statements> operator<<(std::shared_ptr<ast4vx::Statements> a, std::shared_ptr<ast4vx::Node> statement);

/**
 * @brief Adds the \p statement to the statements of \p a
 */
std::shared_ptr<ast4vx::LocalToPixel> operator<<(std::shared_ptr<ast4vx::LocalToPixel> a, std::shared_ptr<ast4vx::Node> statement);

/**
 * @brief Adds the \p statement to the statements of \p a
 */
std::shared_ptr<ast4vx::Reduction> operator<<(std::shared_ptr<ast4vx::Reduction> a, std::shared_ptr<ast4vx::Node> statement);

/**
 * @brief Adds the \p statement to the statements of \p a
 */
std::shared_ptr<ast4vx::MaskPixelToPixel> operator<<(std::shared_ptr<ast4vx::MaskPixelToPixel> a, std::shared_ptr<ast4vx::Node> statement);

/**
 * @brief Adds the \p statement to the statements of \p a
 */
std::shared_ptr<ast4vx::If> operator<<(std::shared_ptr<ast4vx::If> a, std::shared_ptr<ast4vx::Node> statement);

/**
 * @brief Adds the \p statement to the statements of \p a
 */
std::shared_ptr<ast4vx::Else> operator<<(std::shared_ptr<ast4vx::Else> a, std::shared_ptr<ast4vx::Node> statement);

/**
 * @brief Creates a new ast4vx::ShiftRight Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> operator>>(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> shift);

/**
 * @brief Creates a new ast4vx::Exp Node with \p a as the parameter and returns it
 */
std::shared_ptr<ast4vx::Node> exp(std::shared_ptr<ast4vx::Node> a);

/**
 * @brief Creates a new ast4vx::Sqrt Node with \p a as the parameter and returns it
 */
std::shared_ptr<ast4vx::Node> sqrt(std::shared_ptr<ast4vx::Node> a);

/**
 * @brief Creates a new ast4vx::Atan2 Node with \p a as the parameter and returns it
 */
std::shared_ptr<ast4vx::Node> atan2(std::shared_ptr<ast4vx::Node> a);

/**
 * @brief Creates a new ast4vx::Abs Node with \p a as the parameter and returns it
 */
std::shared_ptr<ast4vx::Node> abs(std::shared_ptr<ast4vx::Node> a);

/**
 * @brief Creates a new ast4vx::Assign Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> assign(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::Conversion Node with \p a and \p type as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> convert(std::shared_ptr<ast4vx::Node> a, ast4vx::Datatype type);

/**
 * @brief Creates a new ast4vx::Constant Node with \p t as the type and parameter and returns it
 */
template <typename T>
std::shared_ptr<ast4vx::Node> constant(T t)
{
    return std::make_shared<ast4vx::Constant<T>>(t);
}

/**
 * @brief Creates a new ast4vx::VariableDefinition Node with \p a as the parameter and returns it
 */
std::shared_ptr<ast4vx::Node> define(std::shared_ptr<ast4vx::Node> a);

/**
 * @brief Creates a new ast4vx::If Node with \p a as the parameter and returns it
 */
std::shared_ptr<ast4vx::If> IF(std::shared_ptr<ast4vx::Node> a);

/**
 * @brief Creates a new ast4vx::Else Node and returns it
 */
std::shared_ptr<ast4vx::Else> ELSE();

/**
 * @brief Creates a new ast4vx::Less Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> less(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::LessEquals Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> less_equal(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::Equals Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> equal(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::GreaterEquals Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> greater_equal(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::Greater Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> greater(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::Unequals Node with \p a and \p b as the parameters and returns it
 */
std::shared_ptr<ast4vx::Node> unequal(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b);

/**
 * @brief Creates a new ast4vx::Vect4 Node with \p a, \p b, \p c, \p d  as the parameters, \p as the type and returns it
 */
std::shared_ptr<ast4vx::Node> vect4(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b,
                                          std::shared_ptr<ast4vx::Node> c, std::shared_ptr<ast4vx::Node> d, ast4vx::Datatype type );

/**
 * @brief Creates a new ast4vx::Extract4 Node with \p a as the parameter, \p e as the ast4vx::VectChannelType and returns it
 */
std::shared_ptr<ast4vx::Node> extract4(std::shared_ptr<ast4vx::Node> a, ast4vx::VectChannelType e);

/**
 * @brief Creates a new ast4vx::WindowOperation
 * @param in_win The input ast4vx::WindowDescriptor
 * @param reduction_function The ast4vx::Reduction function of this ast4vx::WindowOperation
 * @return The new ast4vx::WindowOperation
 */
std::shared_ptr<ast4vx::WindowOperation> reduce(std::shared_ptr<ast4vx::WindowDescriptor> in_win,
                                                std::shared_ptr<ast4vx::Reduction> reduction_function);
/**
 * @brief Creates a new ast4vx::WindowOperation
 * @param predecessor The predecessor ast4vx::WindowOperation, whichs output should be the input of this ast4vx::WindowOperation
 * @param reduction_function The ast4vx::Reduction function of this ast4vx::WindowOperation
 * @return The new ast4vx::WindowOperation
 */
std::shared_ptr<ast4vx::WindowOperation> reduce(std::shared_ptr<ast4vx::WindowOperation> predecessor,
                                                std::shared_ptr<ast4vx::Reduction> reduction_function);
/**
 * @brief Creates a new ast4vx::WindowOperation
 * @param in_win An std::initializer_list of ast4vx::WindowDescriptor that are the inputs of this reduction operation. Be aware that a reducing ast4vx::WindowOperation only supports one ast4vx::WindowDescriptor
 * @param reduction_function The ast4vx::Reduction function of this ast4vx::WindowOperation
 * @return The new ast4vx::WindowOperation
 */
std::shared_ptr<ast4vx::WindowOperation> reduce(std::initializer_list<std::shared_ptr<ast4vx::WindowDescriptor>> in_win,
                                                std::shared_ptr<ast4vx::Reduction> reduction_function);

/**
 * @brief Creates a new ast4vx::WindowOperation
 * @param in_win The input ast4vx::WindowDescriptor
 * @param forall_function The ast4vx::Statements function of this ast4vx::WindowOperation that should be applied to the whole domain
 * @return The new ast4vx::WindowOperation
 */
std::shared_ptr<ast4vx::WindowOperation> forall(std::shared_ptr<ast4vx::WindowDescriptor> in_win,
                                                std::shared_ptr<ast4vx::Statements> forall_function);
/**
 * @brief Creates a new ast4vx::WindowOperation
 * @param predecessor The predecessor ast4vx::WindowOperation, whichs output should be the input of this ast4vx::WindowOperation
 * @param forall_function The ast4vx::Statements function of this ast4vx::WindowOperation that should be applied to the whole domain
 * @return The new ast4vx::WindowOperation
 */
std::shared_ptr<ast4vx::WindowOperation> forall(std::shared_ptr<ast4vx::WindowOperation> predecessor,
                                                std::shared_ptr<ast4vx::Statements> forall_function);
/**
 * @brief Creates a new ast4vx::WindowOperation
 * @param in_win An std::initializer_list of ast4vx::WindowDescriptor that are the inputs of this reduction operation. Be aware that a forall ast4vx::WindowOperation only supports one ast4vx::WindowDescriptor
 * @param forall_function The ast4vx::Statements function of this ast4vx::WindowOperation that should be applied to the whole domain
 * @return The new ast4vx::WindowOperation
 */
std::shared_ptr<ast4vx::WindowOperation> forall(std::initializer_list<std::shared_ptr<ast4vx::WindowDescriptor>> in_win,
                                                std::shared_ptr<ast4vx::Statements> forall_function);

/**
 * @brief Returns a new PixelToPixel instance and initializes it
 * @param d_out Number of output pixel accessors
 * @param d_in Number of input pixel accessors
 * @return A newly created PixelToPixel instance
 */
std::shared_ptr<ast4vx::Statements> create_p2p(unsigned int d_out, unsigned int d_in);

/**
 * @brief Returns a new MaskPixelToPixel instance and initializes it
 * @param d_out Number of output pixel accessors
 * @param d_in Number of input pixel accessors
 * @param m_in Number of input mask accessors
 * @return A newly created MaskPixelToPixel instance
 */
std::shared_ptr<ast4vx::MaskPixelToPixel> create_p2p_mask(unsigned int d_out, unsigned int d_in, unsigned int m_in);

/**
 * @brief Returns a new LocalToPixel instance and initializes it
 * @param d_out Number of output pixel accessors
 * @param w_in Number of input window accessors
 * @return A newly created LocalToPixel instance
 */
std::shared_ptr<ast4vx::LocalToPixel> create_l2p(unsigned int d_out, unsigned int w_in);

/**
 * @brief Returns a new WindowDescriptor instance and initializes it
 * @param w Width of the window
 * @param h Height of the window
 * @return A newly created WindowDescriptor instance
 */
std::shared_ptr<ast4vx::WindowDescriptor> create_window_desc(unsigned int w, unsigned int h);

/**
 * @brief Returns a new WindowOperation instance and initializes it
 * @param w Width of the window
 * @param h Height of the window
 * @return A newly created WindowOperation instance
 */
std::shared_ptr<ast4vx::WindowOperation> create_window_op(unsigned int w, unsigned int h);

/**
 * @brief Returns a new WindowOperation instance and initializes it
 * @return A newly created WindowOperation instance
 */
std::shared_ptr<ast4vx::WindowOperation> create_window_op();

/**
 * @brief Returns a new Reduction instance and initializes it
 * @param c Intial value of this reduction instance
 * @return A newly created Reduction instance
 */
template <class T>
std::shared_ptr<ast4vx::Reduction> create_reduction(ast4vx::Constant<T> c = ast4vx::Constant<T>())
{
    return std::make_shared<ast4vx::Reduction>(c);
}



/**
 * @brief Abstract Visitor class for ast4vx::Node
 */
template <class ReturnType, class ParameterType>
class ASTVisitor
{
public:
    virtual ReturnType visit(std::shared_ptr<ast4vx::Node> n, ParameterType p) = 0;
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
