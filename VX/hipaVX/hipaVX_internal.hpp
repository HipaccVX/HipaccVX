#ifndef __HIPAVX_INTERNAL__
#define __HIPAVX_INTERNAL__

#include "../vx.h"

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
enum class NodeType
{
	None,
	Add,
	Sub,
	Mul,
	Div,

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
	virtual ~SimpleBinaryNode() = default;
	virtual std::string generate_source() override;
};
class SimpleUnaryFunctionNode: public Node
{
public:
	virtual ~SimpleUnaryFunctionNode() = default;
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

namespace HipaVX
{

class FileinputImage: public Image
{
public:
	FileinputImage(vx_uint32 width, vx_uint32 height, vx_df_image color, std::string filename);
	virtual ~FileinputImage() = default;
	std::string file;
};

class WriteImageNode: public Node
{
public:
	WriteImageNode();
	virtual ~WriteImageNode() override = default;
	Image *in;
	std::string out_file;
	virtual std::vector<Object*> get_inputs() override;
	virtual std::vector<Object*> get_outputs() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
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

	virtual std::vector<Object*> get_inputs() override;
	virtual std::vector<Object*> get_outputs() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};


class Dilate: public Node
{
public:
	Dilate();
	virtual ~Dilate() override = default;

	Image *in;
	Image *out;

	virtual std::vector<Object*> get_inputs() override;
	virtual std::vector<Object*> get_outputs() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};
class Erode: public Node
{
public:
	Erode();
	virtual ~Erode() override = default;

	Image *in;
	Image *out;

	virtual std::vector<Object*> get_inputs() override;
	virtual std::vector<Object*> get_outputs() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};


class HipaccNode: public Node
{
public:
	HipaccNode();
	std::string filename;
	Image *out;
	std::vector<vx_reference> parameters;



	virtual ~HipaccNode() override = default;

	std::string kernel_name;

	virtual std::vector<Object*> get_inputs() override;
	virtual std::vector<Object*> get_outputs() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};


class SimplePoint: public Node
{
public:
	virtual ~SimplePoint() override = default;

	Image *in_1;
	Image *in_2;
	Image *out;
	std::string operation;

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
		operation = "+";
		node_name = "Point Addition";
	}
	virtual ~SimplePointAdd() override = default;
};
class SimplePointSub: public SimplePoint
{
public:
	SimplePointSub()
	{
		operation = "-";
		node_name = "Point Subtraction";
	}
	virtual ~SimplePointSub() override = default;
};
class SimplePointMul: public SimplePoint
{
public:
	SimplePointMul()
	{
		operation = "*";
		node_name = "Point Multiplication";
	}
	virtual ~SimplePointMul() override = default;
};
class SimplePointDiv: public SimplePoint
{
public:
	SimplePointDiv()
	{
		operation = "/";
		node_name = "Point Division";
	}
	virtual ~SimplePointDiv() override = default;
};
class AndNode: public SimplePoint
{
public:
	AndNode()
	{
		operation = "&";
		node_name = "And";
	}
	virtual ~AndNode() override = default;
};
class XorNode: public SimplePoint
{
public:
	XorNode()
	{
		operation = "^";
		node_name = "Xor";
	}
	virtual ~XorNode() override = default;
};
class OrNode: public SimplePoint
{
public:
	OrNode()
	{
		operation = "|";
		node_name = "or";
	}
	virtual ~OrNode() override = default;
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

template<typename T>
class Matrix: public Object
{
public:
	Matrix()
	{
		type = VX_TYPE_HIPAVX_MATRIX;
	}

	std::vector<T> mask;
	int dim[2]; //dim[0] = x, dim[1] = y

	void from_VX_Matrix(VX_Matrix *m);
};

template <typename T>
class LinearMask: public Node
{
public:
	LinearMask();
	virtual ~LinearMask() override = default;
	Matrix<T> matrix;
	std::unique_ptr<Scalar> normalization;

	Image *in;
	Image *out;

	bool use_image_datatype_for_sum = true;
	vx_df_image sum_datatype;

	virtual std::vector<Object*> get_inputs() override;
	virtual std::vector<Object*> get_outputs() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};

class Sobel3x3Node: public Node
{
public:
	Sobel3x3Node();
	virtual ~Sobel3x3Node() override = default;

	Image *in;
	Image *out_x;
	Image *out_y;


    std::shared_ptr<function_ast::Stencil> stencil_x;
    std::shared_ptr<function_ast::Stencil> stencil_y;

    function_ast::ForEveryPixel kernel_x;
    function_ast::ForEveryPixel kernel_y;

	virtual std::vector<Object*> get_inputs() override;
	virtual std::vector<Object*> get_outputs() override;
	virtual std::vector<Node*> get_subnodes() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class BoxFilter: public LinearMask<int>
{
public:
	BoxFilter()
	{
		this->matrix.mask = {1, 1, 1, 1, 1, 1, 1, 1, 1};
		this->matrix.dim[0] = this->matrix.dim[1] = 3;
		float norm = 1.0f / 9;
		Scalar* norm_scalar = new Scalar(VX_TYPE_FLOAT32, &norm);
		this->normalization.reset(norm_scalar);
		this->use_image_datatype_for_sum = false;
		this->sum_datatype = VX_DF_IMAGE_S16;
		this->node_name = "Box Filter 3x3";
	}
	virtual ~BoxFilter() override = default;
};
class GaussianFilter: public LinearMask<int>
{
public:
	GaussianFilter()
	{
		this->matrix.mask = {1, 2, 1, 2, 4, 2, 1, 2, 1};
		this->matrix.dim[0] = this->matrix.dim[1] = 3;
		float norm = 1.0f / 16;
		Scalar* norm_scalar = new Scalar(VX_TYPE_FLOAT32, &norm);
		this->normalization.reset(norm_scalar);
		this->use_image_datatype_for_sum = false;
		this->sum_datatype = VX_DF_IMAGE_S16;
		this->node_name = "Gaussian Filter 3x3";
	}
	virtual ~GaussianFilter() override = default;
};


class Add3_3: public Node
{
public:
	Add3_3();
	virtual ~Add3_3() override = default;

	LinearMask<int> add;

	Image *in;
	Image *out;

	virtual std::vector<Object*> get_inputs() override;
	virtual std::vector<Object*> get_outputs() override;
	virtual std::vector<Node*> get_subnodes() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class HarrisCorners: public Node
{
public:
	HarrisCorners(Image *in)
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
	virtual ~HarrisCorners() override = default;

	Sobel3x3Node sobel;
	SimplePointScalarMul<float> sobel_x_norm;//Gx
	SimplePointScalarMul<float> sobel_y_norm;//Gy
	SimplePointMul sobel_x_square;//Gx²
	SimplePointMul sobel_y_square;//Gx²

	SimplePointMul sobel_x_y;//Gx*Gy

	Add3_3 gx_square_A;
	Add3_3 gy_square_A;
	Add3_3 gx_gy_A;

	SimplePointAdd trace_add;

	SimplePointMul gx_A_gy_A; //gx_A² * gy_A²
	SimplePointMul gx_gy_A_square; //gx_gy_A * gx_gy_A
	SimplePointSub det_kernel;

	SimplePointMul trace_A_square_kernel;

	SimplePointScalarMul<float> trace_A_square_k_kernel;

	SimplePointSub Mc_kernel;




	Image *in;
	Scalar *strength_thresh;
	Scalar *min_distance;
	Scalar *sensitivity;
	vx_int32 gradient_size;
	vx_int32 block_size;
	Array *corners;
	Scalar *num_corners;




	Image sob_x;
	Image sob_y;

	Image Gx;
	Image Gy;

	Image square_Gx;
	Image square_Gy;

	Image square_Gx_sum;
	Image square_Gy_sum;

	//Mask A
	Image trace_A;


	Image det_A_minuend;

	Image det_A_mul_Gx_Gy;
	Image det_A_mul_Gx_Gy_sum;
	Image det_A_subtrahend;

	Image det_A;

	Image trace_A_square;
	Image trace_A_square_k;
	Image Mc;




	Image Vc;


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

class VXConvolveNode: public Node
{
public:
	VXConvolveNode();
	virtual ~VXConvolveNode() override = default;
	Image *in;
	Image *out;
	Convolution *convolution;

	LinearMask<short> lin_mask_node;
	std::unique_ptr<Image> lin_mask_image;

	SaturateNode saturate_node;


	virtual std::vector<Object*> get_inputs() override;
	virtual std::vector<Object*> get_outputs() override;
	virtual std::vector<Node*> get_subnodes() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
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

class VXBilateralFilterNode: public Node
{
public:
	VXBilateralFilterNode();
	virtual ~VXBilateralFilterNode() override = default;

	Image *in;
	Image *out;
	vx_int32 diameter;
	vx_float32 sigmaSpace;
	vx_float32 sigmaValues;

	function_ast::ForEveryPixel kernel;

	virtual std::vector<Object*> get_inputs() override;
	virtual std::vector<Object*> get_outputs() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};

class AnotherBilateralFilterNode: public Node
{
public:
	AnotherBilateralFilterNode();
	virtual ~AnotherBilateralFilterNode() override = default;

	Image *in;
	Image *out;

	vx_int32 sigma_r;

    std::shared_ptr<function_ast::Stencil> stencil;
    std::shared_ptr<function_ast::ForEveryPixel> kernel;

	virtual std::vector<Object*> get_inputs() override;
	virtual std::vector<Object*> get_outputs() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

}

namespace generator
{
enum class Type
{
	Definition = 0,
	Call
};

std::string node_generator(HipaVX::WriteImageNode* n, Type t);
std::string node_generator(HipaVX::ConvertDepthNode* n, Type t);

std::string node_generator(HipaVX::NotNode* n, Type t);

std::string node_generator(HipaVX::Dilate* n, Type t);
std::string node_generator(HipaVX::Erode* n, Type t);



std::string node_generator(HipaVX::VXChannelExtractNode* n, Type t);
std::string node_generator(HipaVX::VXChannelCombineNode* n, Type t);


template <typename T>
std::string node_generator(HipaVX::LinearMask<T>* n, Type t);
std::string node_generator(HipaVX::SimplePoint* n, Type t);
template <typename T>
std::string node_generator(HipaVX::SimplePointScalar<T>* n, Type t);
std::string node_generator(HipaVX::HarrisCorners* n, Type t);

std::string node_generator(HipaVX::SaturateNode* n, Type t);
std::string node_generator(HipaVX::UnaryFunctionNode* n, Type t);
template <typename T>
std::string node_generator(HipaVX::ConditionalAssignmentNode<T>* n, Type t);

std::string node_generator(HipaVX::HipaccNode *n, Type t);
std::string node_generator(HipaVX::VXScaleNode *n, Type t);



}







namespace HipaVX
{
template<typename T>
void Matrix<T>::from_VX_Matrix(VX_Matrix *m)
{
	dim[0] = m->rows;
	dim[1] = m->columns;

	T* data = (T*) m->mat.data();

	mask.clear();
	for(unsigned int i = 0; i < dim[0] * dim[1]; i++)
	{
		mask.push_back(data[i]);
	}
}




template <typename T>
LinearMask<T>::LinearMask()
{
	node_name = "Linear Mask";
}
template <typename T>
std::vector<Object *> LinearMask<T>::get_inputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(in);
	return used_objects;
}
template <typename T>
std::vector<Object *> LinearMask<T>::get_outputs()
{
	std::vector<Object*> used_objects;
	used_objects.emplace_back(out);
	return used_objects;
}
template <typename T>
std::string LinearMask<T>::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
template <typename T>
std::string LinearMask<T>::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}


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




#endif
