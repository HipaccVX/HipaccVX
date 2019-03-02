#include "ast.hpp"
#include <fstream>
#include <string>

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

std::shared_ptr<function_ast::Node> operator<<(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> shift)
{
    return std::make_shared<function_ast::ShiftLeft>(a, shift);
}

std::shared_ptr<function_ast::Statements> operator<<(std::shared_ptr<function_ast::Statements> a, std::shared_ptr<function_ast::Node> statement)
{
    (*a) << statement;
    return a;
}

std::shared_ptr<function_ast::Node> operator>>(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> shift)
{
    return std::make_shared<function_ast::ShiftRight>(a, shift);
}

std::shared_ptr<function_ast::Node> exp(std::shared_ptr<function_ast::Node> a)
{
    return std::make_shared<function_ast::Exp>(a);
}

std::shared_ptr<function_ast::Node> sqrt(std::shared_ptr<function_ast::Node> a)
{
    return std::make_shared<function_ast::Sqrt>(a);
}

std::shared_ptr<function_ast::Node> atan2(std::shared_ptr<function_ast::Node> a)
{
    return std::make_shared<function_ast::Atan2>(a);
}

std::shared_ptr<function_ast::Node> abs(std::shared_ptr<function_ast::Node> a)
{
    return std::make_shared<function_ast::Abs>(a);
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

std::shared_ptr<function_ast::Node> vect4(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b,
                                          std::shared_ptr<function_ast::Node> c, std::shared_ptr<function_ast::Node> d, function_ast::Datatype type)
{
    return std::make_shared<function_ast::Vect4>(a, b, c, d, type);
}

std::shared_ptr<function_ast::Node> extract4(std::shared_ptr<function_ast::Node> a, function_ast::Datatype type, function_ast::VectChannelType chn)
{
    return std::make_shared<function_ast::Extract4>(a, type, chn);
}

std::shared_ptr<function_ast::Node> define(std::shared_ptr<function_ast::Node> n)
{
    return std::make_shared<function_ast::VariableDefinition>(n);
}

std::shared_ptr<function_ast::Node> operator&(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::BitwiseAnd>(a, b);
}

std::shared_ptr<function_ast::Node> operator&&(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::And>(a, b);
}

std::shared_ptr<function_ast::Node> operator|(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
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

std::shared_ptr<function_ast::Node> less(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::Less>(a, b);
}

std::shared_ptr<function_ast::Node> less_equal(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::LessEquals>(a, b);
}

std::shared_ptr<function_ast::Node> equal(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::Equals>(a, b);
}

std::shared_ptr<function_ast::Node> greater_equal(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::GreaterEquals>(a, b);
}

std::shared_ptr<function_ast::Node> greater(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::Greater>(a, b);
}

std::shared_ptr<function_ast::Node> unequal(std::shared_ptr<function_ast::Node> a, std::shared_ptr<function_ast::Node> b)
{
	return std::make_shared<function_ast::Unequals>(a, b);
}


