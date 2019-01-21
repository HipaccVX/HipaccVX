#include "ast.hpp"
#include <fstream>
#include <string>

function_ast::Datatype convert_type(vx_df_image type)
{
	switch(type)
	{
	case VX_DF_IMAGE_U8:
		return function_ast::Datatype::UINT8;
	case VX_DF_IMAGE_S16:
		return function_ast::Datatype::INT16;
	case VX_DF_IMAGE_U16:
		return function_ast::Datatype::UINT16;
	case VX_DF_IMAGE_S32:
		return function_ast::Datatype::INT32;
	case VX_DF_IMAGE_U32:
		return function_ast::Datatype::UINT32;
	case VX_TYPE_FLOAT32:
		return function_ast::Datatype::FLOAT;
	}
	throw std::runtime_error("Unknown type in function_ast::Datatype convert");
}

namespace function_ast
{
std::string SimpleBinaryNode::generate_source()
{
    return generate(this);
}
std::string SimpleUnaryFunctionNode::generate_source()
{
    return generate(this);
}
std::string Variable::generate_source()
{
    return generate(this);
}
std::string VariableDefinition::generate_source()
{
    return generate(this);
}
std::string Assignment::generate_source()
{
    return generate(this);
}
std::string TargetPixel::generate_source()
{
    return generate(this);
}
std::string Image::generate_source()
{
    return generate(this);
}
std::string ForEveryPixel::generate_source()
{
    return generate(this);
}
std::string IterateAroundPixel::generate_source()
{
    return generate(this);
}
std::string PixelvalueAtCurrentStencilPos::generate_source()
{
    return generate(this);
}
std::string StencilvalueAtCurrentStencilPos::generate_source()
{
    return generate(this);
}
std::string ReduceAroundPixel::generate_source()
{
    return generate(this);
}
std::string ReductionOutput::generate_source()
{
    return generate(this);
}
std::string Statements::generate_source()
{
    return generate(this);
}
std::string If::generate_source()
{
    return generate(this);
}
std::string Else::generate_source()
{
    return generate(this);
}
std::string CurrentPixelvalue::generate_source()
{
    return generate(this);
}
std::string Stencil::generate_source()
{
    return generate(this);
}

}

std::shared_ptr<function_ast::Node> operator+(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
    return std::make_shared<function_ast::Add>(a, b);
}

std::shared_ptr<function_ast::Node> operator-(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
    return std::make_shared<function_ast::Sub>(a, b);
}

std::shared_ptr<function_ast::Node> operator*(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
    return std::make_shared<function_ast::Mul>(a, b);
}

std::shared_ptr<function_ast::Node> operator/(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
    return std::make_shared<function_ast::Div>(a, b);
}

std::shared_ptr<function_ast::Node> square(std::shared_ptr<function_ast::Node> a)
{
    return std::make_shared<function_ast::Square>(a);
}

std::shared_ptr<function_ast::Node> exp(std::shared_ptr<function_ast::Node> a)
{
    return std::make_shared<function_ast::Exp>(a);
}

std::shared_ptr<function_ast::Node> assign(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
    return std::make_shared<function_ast::Assignment>(a, b);
}

std::shared_ptr<function_ast::Node> target_pixel(std::shared_ptr<function_ast::Node> a)
{
    return std::make_shared<function_ast::TargetPixel>(a);
}

std::shared_ptr<function_ast::Node> current_pixel(std::shared_ptr<function_ast::Node> a)
{
    return std::make_shared<function_ast::CurrentPixelvalue>(a);
}

std::shared_ptr<function_ast::Node> convert(std::shared_ptr<function_ast::Node> a,
                                            function_ast::Datatype type)
{
    return std::make_shared<function_ast::Conversion>(a, type);
}

std::shared_ptr<function_ast::Node> define(std::shared_ptr<function_ast::Node> n)
{
    return std::make_shared<function_ast::VariableDefinition>(n);
}

std::shared_ptr<function_ast::Node> operator&(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::BitwiseAnd>(a, b);
}

std::shared_ptr<function_ast::Node> operator&&(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::And>(a, b);
}

std::shared_ptr<function_ast::Node> operator|(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::BitwiseOr>(a, b);
}

std::shared_ptr<function_ast::Node> operator||(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::Or>(a, b);
}

std::shared_ptr<function_ast::Node> operator^(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::BitwiseXor>(a, b);
}

std::shared_ptr<function_ast::Node> operator!(std::shared_ptr<function_ast::Node> a)
{
	return std::make_shared<function_ast::Not>(a);
}

std::shared_ptr<function_ast::Node> operator~(std::shared_ptr<function_ast::Node> a)
{
	return std::make_shared<function_ast::BitwiseNot>(a);
}
std::shared_ptr<function_ast::If> IF(std::shared_ptr<function_ast::Node> a)
{
	return std::make_shared<function_ast::If>(a);
}
std::shared_ptr<function_ast::Else> ELSE()
{
	return std::make_shared<function_ast::Else>();
}
std::shared_ptr<function_ast::Node> less(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::Less>(a, b);
}
std::shared_ptr<function_ast::Node> less_equal(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::LessEquals>(a, b);
}
std::shared_ptr<function_ast::Node> equal(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::Equals>(a, b);
}
std::shared_ptr<function_ast::Node> greater_equal(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::GreaterEquals>(a, b);
}
std::shared_ptr<function_ast::Node> greater(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::Greater>(a, b);
}
std::shared_ptr<function_ast::Node> unequal(std::shared_ptr<function_ast::Node> a,
							  std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::Unequals>(a, b);
}


