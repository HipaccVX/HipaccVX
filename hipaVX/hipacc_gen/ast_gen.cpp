#include "../gen_template.hpp"
#include "hipacc_gen.hpp"
#include "node_gen.hpp"

#include <iostream>
#include <cstdlib>

#include <fstream>
#include <string>
#include <sstream>
#include <map>

using std::string;

namespace function_ast
{

std::string generate_image_name(Image *i)
{
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

    return "(" + left + op + right + ")";
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
	case NodeType::Not:
		func = "!";
		break;
	case NodeType::BitwiseNot:
		func = "~";
		break;
    case NodeType::Conversion:
        func = "(" + to_string(dynamic_cast<Conversion*>(s)->to) + ")";
        break;
    case NodeType::Square:
    {
        auto mul = new Mul();
        mul->subnodes[0] = s->subnodes[0];
        mul->subnodes[1] = s->subnodes[0];
        return mul->generate_source();
    }break;
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

std::string generate(ForEveryPixel *s)
{
    string member_variables;
    string constructor_parameters;
    string constructor_init_list;
    string add_accessor;

    string tabs = "\t";

    std::vector<Variable*> variables;

    for(auto node: s->inputs)
    {
        string name;
        string type;
        if (node->type == NodeType::Variable)
        {
            name = std::dynamic_pointer_cast<Variable>(node)->generate_source();
            type = to_string(std::dynamic_pointer_cast<Variable>(node)->datatype);
        }
        if (node->type == NodeType::Image)
        {
            auto i = std::dynamic_pointer_cast<Image>(node);
            name = i->generate_source();
            type = "Accessor<" + VX_DF_IMAGE_to_hipacc[i->image->col] + ">&";
        }
        if (node->type == NodeType::Stencil)
        {
            auto s = std::dynamic_pointer_cast<Stencil>(node);
            name = s->name;
            type = "Domain&";

            member_variables += tabs + type + " " + name + ";\n";
            constructor_parameters += ", " + type + " " + name;
            constructor_init_list += ", " + name + "(" + name + ")";

            name = name + "_mask";
            type = "Mask<" + to_string(s->datatype) + ">&";
        }

        member_variables += tabs + type + " " + name + ";\n";
        constructor_parameters += ", " + type + " " + name;
        constructor_init_list += ", " + name + "(" + name + ")";


        if (node->type == NodeType::Image)
        {
            add_accessor += tabs + '\t' + "add_accessor(&" + name + ");\n";
        }
    }

    if (s->output->type != NodeType::Image)
        throw std::runtime_error("std::string generate(ForEveryPixel *s)");

    string kernel = generate(&s->function);

    string def = read_file(hipaVX_folder + "/kernels/general_kernel_definition");
    def = use_template(def, "KERNEL_NAME", "Kernel");
    def = use_template(def, "VARIABLES", member_variables);
    def = use_template(def, "VARIABLES_CONSTRUCTOR_PARAMS", constructor_parameters);
    def = use_template(def, "VARIABLES_CONSTRUCTOR_INIT_LIST", constructor_init_list);
    def = use_template(def, "ADD_ACCESSOR", add_accessor);
    def = use_template(def, "MISC_CONSTRUCTOR", "");
    def = use_template(def, "KERNEL", kernel);
    def = use_template(def, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[std::dynamic_pointer_cast<Image>(s->output)->image->col]);
    def = use_template(def, "ID", std::to_string(s->id));

    return def;
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

std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_accessor(HipaVX::Image *image, function_ast::Stencil *stencil)
{
    std::vector<Kernelcall_Variable*> to_return_call_parameters;
    std::vector<Kernelcall_Variable*> to_return;

    Kernelcall_Mask *mask = new Kernelcall_Mask;
	string mask_name = "mask_" + ::generate_image_name(image);
    mask->set_real_name(mask_name);
    mask->datatype = to_string(stencil->datatype);
    mask->len_dims[0] = stencil->dim[0];
    mask->len_dims[1] = stencil->dim[1];
    mask->flat_mask = stencil->mask;

    Kernelcall_Domain *domain = new Kernelcall_Domain();
	string domain_name = "domain_" + ::generate_image_name(image);
    domain->set_real_name(domain_name);
    domain->argument = mask;

    Kernelcall_BoundaryCondition_from_Dom *boundary = new Kernelcall_BoundaryCondition_from_Dom();
	string boundary_name = "boundary_" + ::generate_image_name(image);
    boundary->set_real_name(boundary_name);
    boundary->datatype = VX_DF_IMAGE_to_hipacc[image->col];
	boundary->image = ::generate_image_name(image);
    boundary->bc = "@@@BOUNDARY_CONDITION@@@";
    boundary->argument = domain;

    Kernelcall_Accessor *accessor = new Kernelcall_Accessor();
	string accessor_name = "accessor_" + ::generate_image_name(image);
    accessor->set_real_name(accessor_name);
    accessor->datatype = VX_DF_IMAGE_to_hipacc[image->col];
    accessor->argument = boundary;


    to_return_call_parameters.push_back(accessor);
    to_return_call_parameters.push_back(domain);
    to_return_call_parameters.push_back(mask);

    to_return.push_back(mask);
    to_return.push_back(domain);
    to_return.push_back(boundary);
    to_return.push_back(accessor);

    return {to_return_call_parameters,to_return};
}

std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_accessor(Image *image)
{
	std::vector<Kernelcall_Variable*> to_return_call_parameters;
	std::vector<Kernelcall_Variable*> to_return;

	Kernelcall_BoundaryCondition_from_WH *boundary = new Kernelcall_BoundaryCondition_from_WH();
	string boundary_name = "boundary_" + generate_image_name(image);
	boundary->set_real_name(boundary_name);
	boundary->datatype = VX_DF_IMAGE_to_hipacc[image->image->col];
	boundary->image = ::generate_image_name(image->image);
	boundary->bc = "@@@BOUNDARY_CONDITION@@@";
	boundary->width = std::to_string(image->image->w);
	boundary->height = std::to_string(image->image->h);

	Kernelcall_Accessor *accessor = new Kernelcall_Accessor();
	string accessor_name = "accessor_" + generate_image_name(image);
	accessor->set_real_name(accessor_name);
	accessor->datatype = VX_DF_IMAGE_to_hipacc[image->image->col];
	accessor->argument = boundary;

	to_return_call_parameters.push_back(accessor);

	to_return.push_back(boundary);
	to_return.push_back(accessor);

	return {to_return_call_parameters,to_return};
}

std::string generate_call(ForEveryPixel *fep)
{
    config_struct_call___ hipacc_call;
    std::vector<Kernelcall_Variable*> kernel_parameters;

    std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> tuple;

    HipaVX::Image *image = std::dynamic_pointer_cast<Image>(fep->output)->image;
    tuple = generator::generate_iterationspace(image);
    kernel_parameters.insert(kernel_parameters.end(), std::get<0>(tuple).begin(), std::get<0>(tuple).end());
    hipacc_call.kcv.insert(hipacc_call.kcv.end(), std::get<1>(tuple).begin(), std::get<1>(tuple).end());

    for(unsigned int i = 0; i < fep->inputs.size(); i++)
    {
        auto node = fep->inputs[i];
        switch(node->type)
        {
        /*case secret::NodeType::Variable:
            tuple = generator::generate_scalar((HipaVX::Scalar*) n->parameters[i]);
            break;*/
        case function_ast::NodeType::Image:
        {
            auto image = std::dynamic_pointer_cast<Image>(node);
            HipaVX::Image *HVX_image = image->image;
            if (i+1 < fep->inputs.size() && fep->inputs[i+1]->type == function_ast::NodeType::Stencil)
            {
                auto stencil = std::dynamic_pointer_cast<Stencil>(fep->inputs[i+1]);
                tuple = generate_accessor(HVX_image, stencil.get());
                i++;
            }
			else
			{
				tuple = generate_accessor(image.get());
			}
            break;
        }
        default:
            throw std::runtime_error("std::string generate_call(ForEveryPixel *fep):\n\tdefault in switchcase");
        }

        kernel_parameters.insert(kernel_parameters.end(), std::get<0>(tuple).begin(), std::get<0>(tuple).end());
        hipacc_call.kcv.insert(hipacc_call.kcv.end(), std::get<1>(tuple).begin(), std::get<1>(tuple).end());
    }

    hipacc_call.kcv.push_back(generator::generate_kernelcall("Kernel", kernel_parameters));
    string s = generator::kernelcall_builder(hipacc_call.kcv);

    s = use_template(s, "ID", fep->id);
    s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

    return s;
}

}
