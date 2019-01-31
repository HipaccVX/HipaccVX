#include "ast_gen.hpp"

using std::string;

string generate_image_name(HipaVX::Image *image);

namespace function_ast
{

std::string generate_image_name(Image *i)
{
	//std::string hipavx_part = string("Image_") + std::to_string(i->my_id); // same with hipacc_gen.hpp
    std::string hipavx_part = ::generate_image_name(i->image);
	return hipavx_part + "_" + std::to_string(i->id);
}

std::string to_string(Datatype d)
{
    std::string datatype = "Unsupported Datatype";
    switch(d)
    {
    case Datatype::FLOAT:
        datatype = "float";
        break;
    case Datatype::UINT8:
        datatype = "uchar";
        break;
    case Datatype::INT16:
        datatype = "short";
        break;
    case Datatype::INT32:
        datatype = "int";
        break;
    }
    return datatype;
}

std::string generate(SimpleBinaryNode *s)
{
    std::string op;

    switch(s->type)
    {
    case NodeType::Add:
        op = "+";
        break;
    case NodeType::Sub:
        op = "-";
        break;
    case NodeType::Mul:
        op = "*";
        break;
    case NodeType::Div:
        op = "/";
        break;
    case NodeType::ShiftLeft:
        op = "<<";
        break;
    case NodeType::ShiftRight:
        op = ">>";
        break;
	case NodeType::And:
		op = "&&";
		break;
	case NodeType::Or:
		op = "||";
		break;
	case NodeType::Xor:
		throw std::runtime_error("std::string generate(SimpleBinaryNode *s): No logical XOR available");
	case NodeType::BitwiseAnd:
		op = "&";
		break;
	case NodeType::BitwiseOr:
		op = "|";
        break;
    case NodeType::BitwiseXor:
        op = "^";
        break;
    case NodeType::Less:
        op = "<";
        break;
    case NodeType::LessEquals:
        op = "<=";
        break;
    case NodeType::Equals:
        op = "==";
        break;
    case NodeType::GreaterEquals:
        op = ">=";
        break;
    case NodeType::Greater:
		op = ">";
        break;
    case NodeType::Unequals:
        op = "!=";
        break;
    }

    auto left = s->subnodes[0]->generate_source();
    auto right = s->subnodes[1]->generate_source();

    return "(" + left + " " + op + " " + right + ")";
}

std::string generate(SimpleUnaryFunctionNode *s)
{
    std::string func;

    switch(s->type)
    {
    case NodeType::Sqrt:
        func = "sqrt";
        break;
    case NodeType::Exp:
        func = "exp";
		break;
    case NodeType::Atan2:
        func = "atan2";
		break;
    case NodeType::Abs:
        func = "abs";
		break;
	case NodeType::Not:
		func = "!";
		break;
	case NodeType::BitwiseNot:
		func = "~";
		break;
    case NodeType::Conversion:
        func = "(" + to_string(dynamic_cast<Conversion*>(s)->to) + ")";
        break;
    }

    auto argument = s->subnodes[0]->generate_source();

    return func + "(" + argument + ")";
}

std::string generate(Vect4 *s)
{
    std::string func = "";

    switch(s->to_dtype)
    {
    case Datatype::UCHAR4:
        func = "make_uchar4";
        break;
    case Datatype::UINT4:
        func = "make_uint4";
        break;
    default:
        std::cerr << "AST: Vect4 is called with an unsupported datatype: " << std::endl;
        exit(1);
        break;
    }

    auto argument = s->subnodes[0]->generate_source() + ", "+
                    s->subnodes[1]->generate_source() + ", "+
                    s->subnodes[2]->generate_source() + ", "+
                    s->subnodes[3]->generate_source();
    return func + "("+ argument + ")";
}

std::string generate(Extract4 *s)
{
    string channel;
    switch(s->channel)
    {
    case function_ast::VectChannelType::CHANNEL0:
        channel = "x";
        break;
    case function_ast::VectChannelType::CHANNEL1:
        channel = "y";
        break;
    case function_ast::VectChannelType::CHANNEL2:
        channel = "z";
        break;
    case function_ast::VectChannelType::CHANNEL3:
        channel = "w";
        break;
    default:
        throw std::runtime_error("AST: Vect4 is called with an unsupported datatype");
    }

    std::string func = "";
    switch(s->from_dtype)
    {
    case Datatype::UCHAR4:
        func = "convert_uchar4";
        break;
    case Datatype::UINT4:
        func = "convert_uint4";
        break;
    default:
        std::cerr << "AST: Extract4 is called with an unsupported datatype: " << std::endl;
        exit(1);
        break;
    }

    auto argument = s->subnodes[0]->generate_source();
    return func + "("+ argument + ")." + channel;
}

std::string generate(Variable *s)
{
    return s->name;
}

std::string generate(VariableDefinition *s)
{
    std::string datatype = to_string(std::dynamic_pointer_cast<Variable>(s->subnodes[0])->datatype);
    return datatype + " " + s->subnodes[0]->generate_source();
}

std::string generate(Assignment *s)
{
    if (s->subnodes[0]->type == NodeType::ReductionOutput)
    {
        auto right = s->subnodes[1]->generate_source();
        return "return " + right + ";";
    }
    else
    {
        auto left = s->subnodes[0]->generate_source();
        auto right = s->subnodes[1]->generate_source();

        return left + "=" + right;
    }
}

std::string generate(TargetPixel *s)
{
    return "output()";
}

std::string generate(Statements *s)
{
    std::string to_return;

    for(auto statement: s->statements)
    {
		to_return += statement->generate_source();
		if (statement->type != NodeType::If && statement->type != NodeType::Else)
			to_return += ";\n";
    }

    return to_return;
}

std::string generate(If *s)
{
    std::string to_return;

    to_return += "if (" + s->condition->generate_source() + ")\n";
    to_return += "{\n";
    to_return += s->body.generate_source();
    to_return += "}\n";

    return to_return;
}

std::string generate(Else *s)
{
    std::string to_return;

    to_return += "else\n";
    to_return += "{\n";
    to_return += s->body.generate_source();
    to_return += "}\n";

    return to_return;
}

std::string generate(CurrentPixelvalue *s)
{
	return generate_image_name(std::dynamic_pointer_cast<Image>(s->subnodes[0]).get()) + "()";
}

std::string generate(Image *s)
{
	return generate_image_name(s);
}

std::string generate(Stencil *s)
{
    return "Stencil generate todo";
}

std::string generate(ReductionOutput *s)
{
    return "ReductionOutput should new generate";
}

std::string generate(ReduceAroundPixel *s)
{
    auto stencil = std::dynamic_pointer_cast<Stencil>(s->subnodes[1]);
    std::string reduction = "";
    switch(s->reduction_type)
    {
    case ReduceAroundPixel::Type::SUM:
        reduction = "SUM";
        break;
    case ReduceAroundPixel::Type::MIN:
        reduction = "MIN";
        break;
    case ReduceAroundPixel::Type::MAX:
        reduction = "MAX";
        break;
    }

    std::string t = "reduce(@@@DOM_NAME@@@, Reduce::@@@REDUCTION@@@, [&] () -> @@@REDUCE_DATATYPE@@@ {\n"
            "@@@BODY@@@\n"
            "})";
    t = use_template(t, "REDUCE_DATATYPE", to_string(s->datatype));
    t = use_template(t, "DOM_NAME", stencil->name);
    t = use_template(t, "REDUCTION", reduction);
    t = use_template(t, "BODY", s->subnodes[2]->generate_source());
    return t;
}

std::string generate(IterateAroundPixel *s)
{
    auto stencil = std::dynamic_pointer_cast<Stencil>(s->subnodes[1]);
    std::string t = "iterate(@@@DOM_NAME@@@, [&] () -> void {\n"
            "@@@BODY@@@\n"
            "})";
    t = use_template(t, "DOM_NAME", stencil->name);
    t = use_template(t, "BODY", s->subnodes[2]->generate_source());
    return t;
}

std::string generate(PixelvalueAtCurrentStencilPos *s)
{
    std::shared_ptr<Stencil>stencil;
    std::shared_ptr<Image> image;
    IterateAroundPixel* iterate;
    ReduceAroundPixel* reduce;
    if ((iterate = dynamic_cast<IterateAroundPixel*>(s->parent)))
    {
        stencil = std::dynamic_pointer_cast<Stencil>(iterate->subnodes[1]);
        image = std::dynamic_pointer_cast<Image>(iterate->subnodes[0]);
    }
    else if ((reduce = dynamic_cast<ReduceAroundPixel*>(s->parent)))
    {
        stencil = std::dynamic_pointer_cast<Stencil>(reduce->subnodes[1]);
        image = std::dynamic_pointer_cast<Image>(reduce->subnodes[0]);
    }

    return image->generate_source() + "(" + stencil->name + ")";
}

std::string generate(StencilvalueAtCurrentStencilPos *s)
{
    std::shared_ptr<Stencil> stencil;
    IterateAroundPixel* iterate;
    ReduceAroundPixel* reduce;
    if ((iterate = dynamic_cast<IterateAroundPixel*>(s->parent)))
    {
        stencil = std::dynamic_pointer_cast<Stencil>(iterate->subnodes[1]);
    }
    else if ((reduce = dynamic_cast<ReduceAroundPixel*>(s->parent)))
    {
        stencil = std::dynamic_pointer_cast<Stencil>(reduce->subnodes[1]);
    }

    return stencil->name + "_mask" + "(" + stencil->name + ")";
}
}
