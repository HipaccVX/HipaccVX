#include "cpp_gen.hpp"

// TODO: use tuples
string generate_image_name(HipaVX::Image *image)
{
    return string("Image_") + std::to_string(image->my_id);
}

namespace generator
{



string node_generator(HipaVX::WriteImageNode* n, Type t)
{
    if (t == Type::Definition)
    {
        return "";
    }
    else if (t == Type::Call)
    {
        string s = "\t@@@IMAGE_DATATYPE@@@ *data_@@@IMAGE@@@_@@@ID@@@ = @@@IMAGE@@@.data.data();\n";
        s       += "\tsave_data(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_CHANNELS@@@, @@@CONVERSION@@@ data_@@@IMAGE@@@_@@@ID@@@, \"@@@FILENAME@@@\");\n";
        s       += "\n";


        s = use_template(s, "ID", n->my_id);
        s = use_template(s, "IMAGE", generate_image_name(n->in));
        s = use_template(s, "IMAGE_DATATYPE", VX_DF_IMAGE_to_cpp[n->in->col]);

        s = use_template(s, "IMAGE_WIDTH", n->in->w);
        s = use_template(s, "IMAGE_HEIGHT", n->in->h);
        if (n->in->col == VX_DF_IMAGE_RGBX)
        {
            s = use_template(s, "IMAGE_CHANNELS", 4);
            s = use_template(s, "CONVERSION", "(unsigned char*)");
        }
        else
        {
            s = use_template(s, "IMAGE_CHANNELS", 1); //TODO
            s = use_template(s, "CONVERSION", "");
        }
        s = use_template(s, "FILENAME", n->out_file);

        return s;
    }
    return "SOMETHING IS WRONG";
}


string node_generator(HipaVX::HipaccNode* n, Type t)
{
    return "string node_generator(HipaVX::HipaccNode* n, Type t) not supported for CppGen";
}

}

namespace function_ast
{

// !!! get rid of this use tuples
std::string generate_image_name(Image *i)
{
    //std::string hipavx_part = string("Image_") + std::to_string(i->my_id); // same with hipacc_gen.hpp
    std::string hipavx_part = ::generate_image_name(i->image);
    return hipavx_part;
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
        datatype = "unsigned char";
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

std::string generate(ForEveryPixel *s)
{
    return "";
}
}

// move this to domVX_main.cpp
std::vector<HipaVX::Image*> get_all_images(HipaVX::Graph *g)
{
    std::vector<HipaVX::Image*> images;
    std::vector<HipaVX::Node*> nodes = g->graph;

    while(!nodes.empty())
    {
        auto node = nodes[nodes.size() - 1];
        nodes.pop_back();

        for(auto ref: node->inputs)
        {
            if (ref->type == VX_TYPE_IMAGE)
                images.push_back((HipaVX::Image*) ref);
        }
        for(auto ref: node->outputs)
        {
            if (ref->type == VX_TYPE_IMAGE)
                images.push_back((HipaVX::Image*) ref);
        }

        auto subnodes = node->subnodes;
        nodes.insert(nodes.end(), subnodes.begin(), subnodes.end());
    }

    std::sort(images.begin(), images.end());
    images.erase(std::unique(images.begin(), images.end()), images.end());

    return images;
}

std::string generate_source_recursive(std::vector<HipaVX::Node*> nodes, const generator::Type t)
{
    if (t == generator::Type::Definition)
        return "";

    string sources = "";
    for (auto node: nodes)
    {
        if (dynamic_cast<HipaVX::WriteImageNode*>(node) != nullptr)
        {
            if (t == generator::Type::Call)
                sources += generator::node_generator(dynamic_cast<HipaVX::WriteImageNode*>(node), t);
            continue;
        }
        auto subnodes = node->subnodes;
        if (subnodes.empty()) //we are at a fundamental node
        {
            //sources += function_ast::generate_call(&node->kernel) + '\n';
            CPPVisitor v;
            sources += v.visit(node->kernel, 0) + '\n';
        }
        else
        {
            sources += generate_source_recursive(subnodes, t) + '\n';
        }
    }
    return sources;
}

void process_graph(HipaVX::Graph *graph)
{
    string main = read_file(hipaVX_folder + "/templates/cpp_main.templ");

    string images;
    const string image_decl_template = "\tmatrix<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@);\n";

    auto used_images = get_all_images(graph);
    for(const auto image: used_images)
    {
        string image_name = ::generate_image_name(image);

        string temp = image_decl_template;

        // BAD HACKs
        const HipaVX::FileinputImage* fim = dynamic_cast<const HipaVX::FileinputImage*>(image);
        const HipaVX::Array* arr = dynamic_cast<const HipaVX::Array*>(image);
        if(fim)
        {
            string fim_decl_template;
            if (fim->col == VX_DF_IMAGE_RGBX)
            {
                fim_decl_template = "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = (@@@DATATYPE@@@*) load_data<unsigned char>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, 4, \"@@@FILENAME@@@\");\n"
                                    "\tmatrix<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
            }
            else
            {
                fim_decl_template = "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = load_data<@@@DATATYPE@@@>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, 1, \"@@@FILENAME@@@\");\n"
                                    "\tmatrix<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
            }

            temp = fim_decl_template;
            temp = use_template(temp, "FILENAME", fim->file);
        }
        else if(arr)
        {
            continue;//Currently arr is only used in Harris corner
            // There is a bug where reads of images after writes are not transpiled correctly
            // Therefore i write to this array when constructing -> constructor has to be called later
        }

        temp = use_template(temp, "DATATYPE", VX_DF_IMAGE_to_cpp[image->col]);
        temp = use_template(temp, "IMAGE_NAME", image_name);
        temp = use_template(temp, "IMAGE_WIDTH", image->w);
        temp = use_template(temp, "IMAGE_HEIGHT", image->h);

        images += temp;
    }

    main = use_template(main, "IMAGE_DEFINITIONS", images);

    string node_definitions = generate_source_recursive(graph->graph, generator::Type::Definition);

    main = use_template(main, "KERNEL_DEFINITIONS", node_definitions);

    string node_calls = generate_source_recursive(graph->graph, generator::Type::Call);

    main = use_template(main, "KERNEL_CALLS", node_calls);

    _write_to_file("main.hipaVX.cpp", main);
}


std::string CPPVisitor::visit(std::shared_ptr<function_ast::Node> n, int i)
{
    switch(n->type)
    {
    case function_ast::NodeType::None:
        throw std::runtime_error("CPPVisitor visited None");
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
    {
        auto s = std::dynamic_pointer_cast<function_ast::SimpleBinaryNode>(n);
        std::string op;

        switch(s->type)
        {
        case function_ast::NodeType::Add:
            op = "+";
            break;
        case function_ast::NodeType::Sub:
            op = "-";
            break;
        case function_ast::NodeType::Mul:
            op = "*";
            break;
        case function_ast::NodeType::Div:
            op = "/";
            break;
        case function_ast::NodeType::ShiftLeft:
            op = "<<";
            break;
        case function_ast::NodeType::ShiftRight:
            op = ">>";
            break;
        case function_ast::NodeType::And:
            op = "&&";
            break;
        case function_ast::NodeType::Or:
            op = "||";
            break;
        case function_ast::NodeType::Xor:
            throw std::runtime_error("std::string generate(SimpleBinaryNode *s): No logical XOR available");
        case function_ast::NodeType::BitwiseAnd:
            op = "&";
            break;
        case function_ast::NodeType::BitwiseOr:
            op = "|";
            break;
        case function_ast::NodeType::BitwiseXor:
            op = "^";
            break;
        case function_ast::NodeType::Less:
            op = "<";
            break;
        case function_ast::NodeType::LessEquals:
            op = "<=";
            break;
        case function_ast::NodeType::Equals:
            op = "==";
            break;
        case function_ast::NodeType::GreaterEquals:
            op = ">=";
            break;
        case function_ast::NodeType::Greater:
            op = ">";
            break;
        case function_ast::NodeType::Unequals:
            op = "!=";
            break;
        }

        auto left = this->visit(s->subnodes[0], 0);
        auto right = this->visit(s->subnodes[1], 0);

        return "(" + left + " " + op + " " + right + ")";
    }

    case function_ast::NodeType::BitwiseNot:
    case function_ast::NodeType::Not:
    case function_ast::NodeType::Sqrt:
    case function_ast::NodeType::Exp:
    case function_ast::NodeType::Conversion:
    case function_ast::NodeType::Abs:
    case function_ast::NodeType::Atan2:
    {
        auto s = std::dynamic_pointer_cast<function_ast::SimpleUnaryFunctionNode>(n);
        std::string func;

        switch(s->type)
        {
        case function_ast::NodeType::Sqrt:
            func = "sqrt";
            break;
        case function_ast::NodeType::Exp:
            func = "exp";
            break;
        case function_ast::NodeType::Atan2:
            func = "atan2";
            break;
        case function_ast::NodeType::Abs:
            func = "abs";
            break;
        case function_ast::NodeType::Not:
            func = "!";
            break;
        case function_ast::NodeType::BitwiseNot:
            func = "~";
            break;
        case function_ast::NodeType::Conversion:
            func = "(" + to_string(std::dynamic_pointer_cast<function_ast::Conversion>(s)->to) + ")";
            break;
        }

        auto argument = this->visit(s->subnodes[0], 0);

        return func + "(" + argument + ")";
    }

    case function_ast::NodeType::Constant:
    {
        if (auto c = dynamic_cast<function_ast::Constant<float>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        else if (auto c = dynamic_cast<function_ast::Constant<unsigned char>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        else if (auto c = dynamic_cast<function_ast::Constant<unsigned int>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        else if (auto c = dynamic_cast<function_ast::Constant<int>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        return "CPP Generate: Constant type fail";
    }

    case function_ast::NodeType::Vect4:
    {
        auto s = std::dynamic_pointer_cast<function_ast::Vect4>(n);
        std::string func = "";

        switch(s->to_dtype)
        {
        case function_ast::Datatype::UCHAR4:
            func = "uchar4";
            break;
        case function_ast::Datatype::UINT4:
            func = "uint4";
            break;
        default:
            std::cerr << "AST: Vect4 is called with an unsupported datatype: " << std::endl;
            exit(1);
            break;
        }

        auto argument = this->visit(s->subnodes[0], 0) + ", "+
                this->visit(s->subnodes[1], 0) + ", "+
                this->visit(s->subnodes[2], 0) + ", "+
                this->visit(s->subnodes[3], 0);
        return func + "("+ argument + ")";
    }

    case function_ast::NodeType::Extract4:
    {
        auto s = std::dynamic_pointer_cast<function_ast::Extract4>(n);
        string channel;
        switch(s->channel)
        {
        case function_ast::VectChannelType::CHANNEL0:
            channel = "0";
            break;
        case function_ast::VectChannelType::CHANNEL1:
            channel = "1";
            break;
        case function_ast::VectChannelType::CHANNEL2:
            channel = "2";
            break;
        case function_ast::VectChannelType::CHANNEL3:
            channel = "3";
            break;
        default:
            throw std::runtime_error("AST: Vect4 is called with an unsupported datatype");
        }

        auto argument = this->visit(s->subnodes[0], 0);
        return argument + ".arr[" + channel + "]";
    }

    case function_ast::NodeType::Variable:
    {
        return std::dynamic_pointer_cast<function_ast::Variable>(n)->name;
    }

    case function_ast::NodeType::VariableDefinition:
    {
        auto s = std::dynamic_pointer_cast<function_ast::VariableDefinition>(n);
        std::string datatype = to_string(std::dynamic_pointer_cast<function_ast::Variable>(s->subnodes[0])->datatype);
        return datatype + " " + this->visit(s->subnodes[0], 0);
    }

    case function_ast::NodeType::Assignment:
    {
        auto s = std::dynamic_pointer_cast<function_ast::Assignment>(n);
        if (s->subnodes[0]->type == function_ast::NodeType::ReductionOutput)
        {
            auto right = this->visit(s->subnodes[1], 0);
            return "return " + right + ";";
        }
        else
        {
            auto left = this->visit(s->subnodes[0], 0);
            auto right = this->visit(s->subnodes[1], 0);

            return left + " = " + right;
        }
    }

    case function_ast::NodeType::TargetPixel:
    {
        auto s = std::dynamic_pointer_cast<function_ast::TargetPixel>(n);
        std::string name = generate_image_name((function_ast::Image*) s->subnodes[0].get());
        return name + ".data[xy_matrix_flat]";
    }

    case function_ast::NodeType::If:
    {
        auto s = std::dynamic_pointer_cast<function_ast::If>(n);
        std::string to_return;

        to_return += "if (" + this->visit(s->condition, 0) + ")\n";
        to_return += "{\n";
        to_return += this->visit(s->body, 0);
        to_return += "}\n";

        return to_return;
    }

    case function_ast::NodeType::Else:
    {
        auto s = std::dynamic_pointer_cast<function_ast::Else>(n);
        std::string to_return;

        to_return += "else\n";
        to_return += "{\n";
        to_return += this->visit(s->body, 0);
        to_return += "}\n";

        return to_return;
    }

    case function_ast::NodeType::Image:
    {
        return generate_image_name(std::dynamic_pointer_cast<function_ast::Image>(n).get());
    }

    case function_ast::NodeType::ForEveryPixel:
    {
        auto fep = std::dynamic_pointer_cast<function_ast::ForEveryPixel>(n);
        function_ast::Image *output = (function_ast::Image*) fep->output.get();

        std::string definitions = "";

        for(auto node: fep->inputs)
        {
            if (node->type == function_ast::NodeType::Stencil)
            {
                auto s = dynamic_cast<function_ast::Stencil*>(node.get());

                string name;
                string type;
                std::string constructor;

                name = s->name;
                type = "matrix<" + to_string(s->datatype) + ">";
                constructor = "(" + std::to_string(s->dim[0]) + ", " + std::to_string(s->dim[1]) + ", {" ;
                for (int i = 0; i < s->mask.size(); i++)
                {
                    constructor += s->mask[i];
                    if (i != s->mask.size() - 1)
                        constructor += ", ";
                }
                constructor += "});";
                definitions += type + " " + name + constructor + "\n";
            }
        }

        std::string code = this->visit(fep->function, i);




        string call = read_file(hipaVX_folder + "/templates/cpp_rasterscan.templ");

        call = use_template(call, "HEIGHT", generate_image_name(output->image) + ".h");
        call = use_template(call, "WIDTH", generate_image_name(output->image) + ".w");
        call = use_template(call, "DEFINITIONS", definitions);
        call = use_template(call, "CODE", code);
        return call;
    }

    case function_ast::NodeType::CurrentPixelvalue:
    {
        auto s = std::dynamic_pointer_cast<function_ast::CurrentPixelvalue>(n);
        std::string name = generate_image_name((function_ast::Image*) s->subnodes[0].get());

        // Legacy, only needed to allow current HipaVX implementation
        if (current_mapping == nullptr)
        {
            return name + ".data[xy_matrix_flat]";
        }
        else
        {
            if (current_output_pixel_index == "")
            {
                return name + ".data[pixel_index]";
            }
            else
            {
                return name + ".data[" + current_output_pixel_index + "]";
            }
        }
    }

    case function_ast::NodeType::Stencil:
    {
        return "Stencil generate todo";
    }

    case function_ast::NodeType::IterateAroundPixel:
    {
        return "IterateAroundPixel generate todo";
    }

    case function_ast::NodeType::ReductionOutput:
    {
        return "ReductionOutput should not generate";
    }

    case function_ast::NodeType::ReduceAroundPixel:
    {
        auto s = std::dynamic_pointer_cast<function_ast::ReduceAroundPixel>(n);
        auto stencil = std::dynamic_pointer_cast<function_ast::Stencil>(s->subnodes[1]);
        std::string reduction = "@std::string generate(ReduceAroundPixel *s)";

        switch(s->reduction_type)
        {
        case function_ast::ReduceAroundPixel::Type::SUM:
            reduction = "a + b";
            break;
        case function_ast::ReduceAroundPixel::Type::MIN:
            reduction = "(a > b) ? b : a";
            break;
        case function_ast::ReduceAroundPixel::Type::MAX:
            reduction = "(a > b) ? a : b";
            break;
        }

        std::string t = "local_reduce<@@@REDUCE_DATATYPE@@@>(@@@IMAGE_NAME@@@, y_matrix, x_matrix, @@@STENCIL_NAME@@@, [&] (short s, int value) -> @@@REDUCE_DATATYPE@@@ {\n"
                        "@@@BODY@@@\n"
                        "},\n"
                        "[](@@@REDUCE_DATATYPE@@@ a, @@@REDUCE_DATATYPE@@@ b)\n"
                        "{\n"
                        "    return @@@REDUCTION@@@;\n"
                        "})\n";
        t = use_template(t, "REDUCE_DATATYPE", to_string(s->datatype));
        t = use_template(t, "STENCIL_NAME", stencil->name);
        t = use_template(t, "REDUCTION", reduction);
        t = use_template(t, "IMAGE_NAME", generate_image_name(std::dynamic_pointer_cast<function_ast::Image>(s->subnodes[0]).get()));
        t = use_template(t, "BODY", this->visit(s->subnodes[2], i));

        return t;
    }

    case function_ast::NodeType::PixelvalueAtCurrentStencilPos:
    {
        return "value";
    }

    case function_ast::NodeType::StencilvalueAtCurrentStencilPos:
    {
        return "s";
    }

    case function_ast::NodeType::Statements:
    {
        auto s = std::dynamic_pointer_cast<function_ast::Statements>(n);
        std::string to_return;

        for(auto statement: s->statements)
        {
            to_return += this->visit(statement, i);
            if (statement->type != function_ast::NodeType::If && statement->type != function_ast::NodeType::Else)
                to_return += ";\n";
        }

        return to_return;
    }

    case function_ast::NodeType::Pregenerated:
        break;
    case function_ast::NodeType::PixelAccessor:
    {
        auto s = std::dynamic_pointer_cast<function_ast::PixelAccessor>(n);
        if (current_mapping == nullptr)
            return "PixelAccessor_" + std::to_string(s->num);
        else
        {
            auto image = std::make_shared<function_ast::Image>();
            image->image = current_mapping->pixel_mappings.at(s->num);
            auto pixel = std::make_shared<function_ast::CurrentPixelvalue>();
            pixel->subnodes[0] = image;
            return visit(pixel, i+1);
        }
    }

    case function_ast::NodeType::WindowAccessor:
        break;
    }
    throw std::runtime_error("CPP Generate: reached end of switch case");
}


std::string CPPVisitor::visit(std::shared_ptr<DomVX::AbstractionNode> n, int i)
{
    switch(n->type)
    {
    case DomVX::AbstractionType::ForEveryPixelTest:
    {
        auto s = std::dynamic_pointer_cast<DomVX::ForEveryPixelTest>(n);

        std::string flat_index_name = "xy_matrix_flat_" + std::to_string(s->id);
        std::string templ =
R"END(for(int y_matrix_@@@ID@@@ = 0; y_matrix_@@@ID@@@ < @@@HEIGHT@@@; y_matrix_@@@ID@@@++)
{
    for (int x_matrix_@@@ID@@@ = 0; x_matrix_@@@ID@@@ < @@@WIDTH@@@; x_matrix_@@@ID@@@++)
    {
        int @@@FLAT_INDEX_NAME@@@ = y_matrix_@@@ID@@@ * @@@WIDTH@@@ + x_matrix_@@@ID@@@;
        @@@CODE@@@
    }
})END";
        templ = use_template(templ, "HEIGHT", s->out->h);
        templ = use_template(templ, "WIDTH", s->out->w);
        templ = use_template(templ, "ID", s->id);
        templ = use_template(templ, "FLAT_INDEX_NAME", flat_index_name);

        current_output_pixel_index = flat_index_name;
        templ = use_template(templ, "CODE", visit(s->call, i+1));
        current_output_pixel_index = "";

        return templ;
    }
    case DomVX::AbstractionType::MapTest:
    {
        auto s = std::dynamic_pointer_cast<DomVX::MapTest>(n);

        current_mapping = s;
        std::string str = visit(s->get_statements(), 0);
        current_mapping = nullptr;
        return str;
    }
    }
    return "";
}

