#include "ast.hpp"
#include <fstream>
#include <string>

std::shared_ptr<ast4vx::Node> operator+(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
    return std::make_shared<ast4vx::Add>(a, b);
}

std::shared_ptr<ast4vx::Node> operator-(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
    return std::make_shared<ast4vx::Sub>(a, b);
}

std::shared_ptr<ast4vx::Node> operator*(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
    return std::make_shared<ast4vx::Mul>(a, b);
}

std::shared_ptr<ast4vx::Node> operator/(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
    return std::make_shared<ast4vx::Div>(a, b);
}

std::shared_ptr<ast4vx::Node> operator<<(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> shift)
{
    return std::make_shared<ast4vx::ShiftLeft>(a, shift);
}

std::shared_ptr<ast4vx::Statements> operator<<(std::shared_ptr<ast4vx::Statements> a, std::shared_ptr<ast4vx::Node> statement)
{
    (*a) << statement;
    return a;
}
std::shared_ptr<ast4vx::LocalToPixel> operator<<(std::shared_ptr<ast4vx::LocalToPixel> a, std::shared_ptr<ast4vx::Node> statement)
{
    (*a) << statement;
    return a;
}

std::shared_ptr<ast4vx::Reduction> operator<<(std::shared_ptr<ast4vx::Reduction> a, std::shared_ptr<ast4vx::Node> statement)
{
    (*a) << statement;
    return a;
}

std::shared_ptr<ast4vx::Node> operator>>(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> shift)
{
    return std::make_shared<ast4vx::ShiftRight>(a, shift);
}

std::shared_ptr<ast4vx::Node> exp(std::shared_ptr<ast4vx::Node> a)
{
    return std::make_shared<ast4vx::Exp>(a);
}

std::shared_ptr<ast4vx::Node> sqrt(std::shared_ptr<ast4vx::Node> a)
{
    return std::make_shared<ast4vx::Sqrt>(a);
}

std::shared_ptr<ast4vx::Node> atan2(std::shared_ptr<ast4vx::Node> a)
{
    return std::make_shared<ast4vx::Atan2>(a);
}

std::shared_ptr<ast4vx::Node> abs(std::shared_ptr<ast4vx::Node> a)
{
    return std::make_shared<ast4vx::Abs>(a);
}

std::shared_ptr<ast4vx::Node> assign(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
    return std::make_shared<ast4vx::Assignment>(a, b);
}

std::shared_ptr<ast4vx::Node> target_pixel(std::shared_ptr<ast4vx::Node> a)
{
    return std::make_shared<ast4vx::TargetPixel>(a);
}

std::shared_ptr<ast4vx::Node> current_pixel(std::shared_ptr<ast4vx::Node> a)
{
    return std::make_shared<ast4vx::CurrentPixelvalue>(a);
}

std::shared_ptr<ast4vx::Node> convert(std::shared_ptr<ast4vx::Node> a,
                                            ast4vx::Datatype type)
{
    return std::make_shared<ast4vx::Conversion>(a, type);
}

std::shared_ptr<ast4vx::Node> vect4(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b,
                                          std::shared_ptr<ast4vx::Node> c, std::shared_ptr<ast4vx::Node> d, ast4vx::Datatype type)
{
    return std::make_shared<ast4vx::Vect4>(a, b, c, d, type);
}

std::shared_ptr<ast4vx::Node> extract4(std::shared_ptr<ast4vx::Node> a, ast4vx::Datatype type, ast4vx::VectChannelType chn)
{
    return std::make_shared<ast4vx::Extract4>(a, type, chn);
}

std::shared_ptr<ast4vx::Node> define(std::shared_ptr<ast4vx::Node> n)
{
    return std::make_shared<ast4vx::VariableDefinition>(n);
}

std::shared_ptr<ast4vx::Node> operator&(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
	return std::make_shared<ast4vx::BitwiseAnd>(a, b);
}

std::shared_ptr<ast4vx::Node> operator&&(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
	return std::make_shared<ast4vx::And>(a, b);
}

std::shared_ptr<ast4vx::Node> operator|(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
	return std::make_shared<ast4vx::BitwiseOr>(a, b);
}

std::shared_ptr<ast4vx::Node> operator||(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
	return std::make_shared<ast4vx::Or>(a, b);
}

std::shared_ptr<ast4vx::Node> operator^(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
	return std::make_shared<ast4vx::BitwiseXor>(a, b);
}

std::shared_ptr<ast4vx::Node> operator!(std::shared_ptr<ast4vx::Node> a)
{
	return std::make_shared<ast4vx::Not>(a);
}

std::shared_ptr<ast4vx::Node> operator~(std::shared_ptr<ast4vx::Node> a)
{
	return std::make_shared<ast4vx::BitwiseNot>(a);
}

std::shared_ptr<ast4vx::If> IF(std::shared_ptr<ast4vx::Node> a)
{
	return std::make_shared<ast4vx::If>(a);
}

std::shared_ptr<ast4vx::Else> ELSE()
{
	return std::make_shared<ast4vx::Else>();
}

std::shared_ptr<ast4vx::Node> less(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
	return std::make_shared<ast4vx::Less>(a, b);
}

std::shared_ptr<ast4vx::Node> less_equal(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
	return std::make_shared<ast4vx::LessEquals>(a, b);
}

std::shared_ptr<ast4vx::Node> equal(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
	return std::make_shared<ast4vx::Equals>(a, b);
}

std::shared_ptr<ast4vx::Node> greater_equal(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
	return std::make_shared<ast4vx::GreaterEquals>(a, b);
}

std::shared_ptr<ast4vx::Node> greater(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
	return std::make_shared<ast4vx::Greater>(a, b);
}

std::shared_ptr<ast4vx::Node> unequal(std::shared_ptr<ast4vx::Node> a, std::shared_ptr<ast4vx::Node> b)
{
	return std::make_shared<ast4vx::Unequals>(a, b);
}





