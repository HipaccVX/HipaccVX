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

namespace ast4vx
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
    case Datatype::INT8:
        datatype = "char";
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


std::string CPPVisitor::visit(std::shared_ptr<ast4vx::Node> n, int i)
{
    switch(n->type)
    {
    case ast4vx::NodeType::None:
        throw std::runtime_error("CPPVisitor visited None");
    case ast4vx::NodeType::Add:
    case ast4vx::NodeType::Sub:
    case ast4vx::NodeType::Mul:
    case ast4vx::NodeType::Div:
    case ast4vx::NodeType::ShiftLeft:
    case ast4vx::NodeType::ShiftRight:
    case ast4vx::NodeType::Less:
    case ast4vx::NodeType::LessEquals:
    case ast4vx::NodeType::Equals:
    case ast4vx::NodeType::GreaterEquals:
    case ast4vx::NodeType::Greater:
    case ast4vx::NodeType::Unequals:
    case ast4vx::NodeType::And:
    case ast4vx::NodeType::Or:
    case ast4vx::NodeType::Xor:
    case ast4vx::NodeType::BitwiseAnd:
    case ast4vx::NodeType::BitwiseOr:
    case ast4vx::NodeType::BitwiseXor:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::SimpleBinaryNode>(n);
        std::string op;

        switch(s->type)
        {
        case ast4vx::NodeType::Add:
            op = "+";
            break;
        case ast4vx::NodeType::Sub:
            op = "-";
            break;
        case ast4vx::NodeType::Mul:
            op = "*";
            break;
        case ast4vx::NodeType::Div:
            op = "/";
            break;
        case ast4vx::NodeType::ShiftLeft:
            op = "<<";
            break;
        case ast4vx::NodeType::ShiftRight:
            op = ">>";
            break;
        case ast4vx::NodeType::And:
            op = "&&";
            break;
        case ast4vx::NodeType::Or:
            op = "||";
            break;
        case ast4vx::NodeType::Xor:
            throw std::runtime_error("std::string generate(SimpleBinaryNode *s): No logical XOR available");
        case ast4vx::NodeType::BitwiseAnd:
            op = "&";
            break;
        case ast4vx::NodeType::BitwiseOr:
            op = "|";
            break;
        case ast4vx::NodeType::BitwiseXor:
            op = "^";
            break;
        case ast4vx::NodeType::Less:
            op = "<";
            break;
        case ast4vx::NodeType::LessEquals:
            op = "<=";
            break;
        case ast4vx::NodeType::Equals:
            op = "==";
            break;
        case ast4vx::NodeType::GreaterEquals:
            op = ">=";
            break;
        case ast4vx::NodeType::Greater:
            op = ">";
            break;
        case ast4vx::NodeType::Unequals:
            op = "!=";
            break;
        }

        auto left = this->visit(s->subnodes[0], 0);
        auto right = this->visit(s->subnodes[1], 0);

        return "(" + left + " " + op + " " + right + ")";
    }

    case ast4vx::NodeType::BitwiseNot:
    case ast4vx::NodeType::Not:
    case ast4vx::NodeType::Sqrt:
    case ast4vx::NodeType::Exp:
    case ast4vx::NodeType::Conversion:
    case ast4vx::NodeType::Abs:
    case ast4vx::NodeType::Atan2:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::SimpleUnaryFunctionNode>(n);
        std::string func;

        switch(s->type)
        {
        case ast4vx::NodeType::Sqrt:
            func = "sqrt";
            break;
        case ast4vx::NodeType::Exp:
            func = "exp";
            break;
        case ast4vx::NodeType::Atan2:
            func = "atan2";
            break;
        case ast4vx::NodeType::Abs:
            func = "abs";
            break;
        case ast4vx::NodeType::Not:
            func = "!";
            break;
        case ast4vx::NodeType::BitwiseNot:
            func = "~";
            break;
        case ast4vx::NodeType::Conversion:
            func = "(" + to_string(std::dynamic_pointer_cast<ast4vx::Conversion>(s)->to) + ")";
            break;
        }

        auto argument = this->visit(s->subnodes[0], 0);

        return func + "(" + argument + ")";
    }

    case ast4vx::NodeType::Constant:
    {
        if (auto c = dynamic_cast<ast4vx::Constant<float>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        else if (auto c = dynamic_cast<ast4vx::Constant<unsigned char>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        else if (auto c = dynamic_cast<ast4vx::Constant<unsigned int>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        else if (auto c = dynamic_cast<ast4vx::Constant<int>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        return "CPP Generate: Constant type fail";
    }

    case ast4vx::NodeType::Vect4:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::Vect4>(n);
        std::string func = "";

        switch(s->to_dtype)
        {
        case ast4vx::Datatype::UCHAR4:
            func = "uchar4";
            break;
        case ast4vx::Datatype::UINT4:
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

    case ast4vx::NodeType::Extract4:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::Extract4>(n);
        string channel;
        switch(s->channel)
        {
        case ast4vx::VectChannelType::CHANNEL0:
            channel = "0";
            break;
        case ast4vx::VectChannelType::CHANNEL1:
            channel = "1";
            break;
        case ast4vx::VectChannelType::CHANNEL2:
            channel = "2";
            break;
        case ast4vx::VectChannelType::CHANNEL3:
            channel = "3";
            break;
        default:
            throw std::runtime_error("AST: Vect4 is called with an unsupported datatype");
        }

        auto argument = this->visit(s->subnodes[0], 0);
        return argument + ".arr[" + channel + "]";
    }

    case ast4vx::NodeType::Variable:
    {
        return std::dynamic_pointer_cast<ast4vx::Variable>(n)->name;
    }

    case ast4vx::NodeType::VariableDefinition:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::VariableDefinition>(n);
        std::string datatype = to_string(std::dynamic_pointer_cast<ast4vx::Variable>(s->subnodes[0])->datatype);
        return datatype + " " + this->visit(s->subnodes[0], 0);
    }

    case ast4vx::NodeType::Assignment:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::Assignment>(n);

        if (s->subnodes[0]->type == ast4vx::NodeType::ReductionOutput)
        {
            auto right = this->visit(s->subnodes[1], 0);
            return "return " + right + ";";
        }
        else if(s->subnodes[0]->type == ast4vx::NodeType::PixelAccessorTest)
        {
            auto accessor = std::dynamic_pointer_cast<ast4vx::PixelAccessorTest>(s->subnodes[0]);

            std::string name = "PixelAccessor_" + std::to_string(accessor->num);
            std::string x = "x";
            std::string y = "y";
            if (pixelaccessor_mapping)
            {
                name = pixelaccessor_mapping->at(accessor->num);
            }
            if (current_output_x != "")
                x = current_output_x;
            if (current_output_y != "")
                y = current_output_y;

            if (accumulator_string == "" || name != accumulator_string)
                return name + ".write(" + x + ", " + y + ", " + this->visit(s->subnodes[1], 0) + ")";
            // else: fall through
        }

        auto left = this->visit(s->subnodes[0], 0);
        auto right = this->visit(s->subnodes[1], 0);

        return left + " = " + right;
    }

    case ast4vx::NodeType::TargetPixel:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::TargetPixel>(n);
        std::string name = generate_image_name((ast4vx::Image*) s->subnodes[0].get());
        return name + ".data[xy_matrix_flat]";
    }

    case ast4vx::NodeType::If:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::If>(n);
        std::string to_return;

        to_return += "if (" + this->visit(s->condition, 0) + ")\n";
        to_return += "{\n";
        to_return += this->visit(s->body, 0);
        to_return += "}\n";

        return to_return;
    }

    case ast4vx::NodeType::Else:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::Else>(n);
        std::string to_return;

        to_return += "else\n";
        to_return += "{\n";
        to_return += this->visit(s->body, 0);
        to_return += "}\n";

        return to_return;
    }

    case ast4vx::NodeType::Image:
    {
        return generate_image_name(std::dynamic_pointer_cast<ast4vx::Image>(n).get());
    }

    case ast4vx::NodeType::ForEveryPixel:
    {
        auto fep = std::dynamic_pointer_cast<ast4vx::ForEveryPixel>(n);
        ast4vx::Image *output = (ast4vx::Image*) fep->output.get();

        std::string definitions = "";

        for(auto node: fep->inputs)
        {
            if (node->type == ast4vx::NodeType::Stencil)
            {
                auto s = dynamic_cast<ast4vx::Stencil*>(node.get());

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

    case ast4vx::NodeType::CurrentPixelvalue:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::CurrentPixelvalue>(n);
        std::string name = generate_image_name((ast4vx::Image*) s->subnodes[0].get());

        // Legacy, only needed to allow current HipaVX implementation
        if (pixelaccessor_mapping == nullptr)
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

    case ast4vx::NodeType::Stencil:
    {
        return "Stencil generate todo";
    }

    case ast4vx::NodeType::IterateAroundPixel:
    {
        return "IterateAroundPixel generate todo";
    }

    case ast4vx::NodeType::ReductionOutput:
    {
        return "ReductionOutput should not generate";
    }

    case ast4vx::NodeType::ReduceAroundPixel:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::ReduceAroundPixel>(n);
        auto stencil = std::dynamic_pointer_cast<ast4vx::Stencil>(s->subnodes[1]);
        std::string reduction = "@std::string generate(ReduceAroundPixel *s)";

        switch(s->reduction_type)
        {
        case ast4vx::ReduceAroundPixel::Type::SUM:
            reduction = "a + b";
            break;
        case ast4vx::ReduceAroundPixel::Type::MIN:
            reduction = "(a > b) ? b : a";
            break;
        case ast4vx::ReduceAroundPixel::Type::MAX:
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
        t = use_template(t, "IMAGE_NAME", generate_image_name(std::dynamic_pointer_cast<ast4vx::Image>(s->subnodes[0]).get()));
        t = use_template(t, "BODY", this->visit(s->subnodes[2], i));

        return t;
    }

    case ast4vx::NodeType::PixelvalueAtCurrentStencilPos:
    {
        return "value";
    }

    case ast4vx::NodeType::StencilvalueAtCurrentStencilPos:
    {
        return "s";
    }

    case ast4vx::NodeType::Statements:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::Statements>(n);
        std::string to_return;

        for(auto statement: s->statements)
        {
            to_return += this->visit(statement, i);
            if (statement->type != ast4vx::NodeType::If && statement->type != ast4vx::NodeType::Else)
                to_return += ";\n";
        }

        return to_return;
    }

    case ast4vx::NodeType::Pregenerated:
        break;
    case ast4vx::NodeType::PixelAccessor:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::PixelAccessor>(n);
        if (pixelaccessor_mapping == nullptr)
            return "PixelAccessor_" + std::to_string(s->num);
        else
        {
            std::string name = (*pixelaccessor_mapping)[s->num];

            // Legacy, only needed to allow current HipaVX implementation
            if (pixelaccessor_mapping == nullptr)
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
                    return name + ".get(" + current_output_x + ", " + current_output_y + ")";
                }
            }
        }
    }

    case ast4vx::NodeType::PixelAccessorTest:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::PixelAccessorTest>(n);

        std::string name = "PixelAccessor_" + std::to_string(s->num);
        std::string x = "x";
        std::string y = "y";
        if (pixelaccessor_mapping)
            name = pixelaccessor_mapping->at(s->num);
        if (current_output_x != "")
            x = current_output_x;
        if (current_output_y != "")
            y = current_output_y;

        if (accumulator_string != "" && name == accumulator_string)
            return name;

        return name + ".get(" + x + ", " + y + ")";
    }

    case ast4vx::NodeType::WindowOperation:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::WindowOperation>(n);

        if (s->current_state == ast4vx::WindowOperation::State::ToPixel)
            return visit(s->ltp_statement);

        std::string code = "";

        std::shared_ptr<ast4vx::Variable> accum_var;
        if (s->current_state == ast4vx::WindowOperation::State::Reduce)
        {
            // Setup the accumulator variable
            accum_var = std::make_shared<ast4vx::Variable>();
            if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<int>>(s->reduction_statement->initial))
                accum_var->datatype = ast4vx::Datatype::INT32;
            else if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<float>>(s->reduction_statement->initial))
                accum_var->datatype = ast4vx::Datatype::FLOAT;
            else if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<unsigned char>>(s->reduction_statement->initial))
                accum_var->datatype = ast4vx::Datatype::UINT8;
            else if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<unsigned int>>(s->reduction_statement->initial))
                accum_var->datatype = ast4vx::Datatype::UINT32;
            else
                throw std::runtime_error("CPPVisitor: WindowOperation: reduce: could not determine the constants type");
            accumulator_string = "accumulator_" + std::to_string(s->reduction_statement->id);
            accum_var->name = accumulator_string;

            code += visit(std::make_shared<ast4vx::VariableDefinition>(accum_var)) + ";\n";
            code += visit(assign(accum_var, s->reduction_statement->initial)) + ";\n";

            // Change second input to the accumulator

        }

        // TODO check window_inputs if same domain
        auto domain = s->window_inputs[i].get()->domain;

        for(unsigned int y = 0; y < domain.size(); y++)
        {
            for(unsigned int x = 0; x < domain[0].size(); x++)
            {
                if (domain[y][x] == 0)
                    continue;
                if (s->current_state == ast4vx::WindowOperation::State::At)
                {
                    if (s->statements[y][x].get() == nullptr)
                        continue;
                    if (s->statements[y][x]->out_pixel_mappings.size() != 1)
                        throw std::runtime_error("CPPVisitor: WindowOperation: At operations requires exactly one output mapping");
                }
                auto old_x = current_output_x;
                auto old_y = current_output_y;
                current_output_x = std::to_string(x);
                current_output_y = std::to_string(y);

                if (s->current_state == ast4vx::WindowOperation::State::At)
                    code += visit(s->statements[y][x]);
                else if (s->current_state == ast4vx::WindowOperation::State::Forall)
                    code += visit(s->forall_statement);
                else if (s->current_state == ast4vx::WindowOperation::State::Reduce)
                    code += visit(s->reduction_statement);
                current_output_x = old_x;
                current_output_y = old_y;
            }
        }

        // Write back the Accumulator
        if (s->current_state == ast4vx::WindowOperation::State::Reduce)
        {
            code += visit(assign(std::make_shared<ast4vx::PixelAccessorTest>(0), accum_var));
        }

        return code;
    }

    case ast4vx::NodeType::LocalToPixel:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::LocalToPixel>(n);
        std::string to_return;


        for(auto statement: s->statements)
        {
            to_return += this->visit(statement, i);
            if (statement->type != ast4vx::NodeType::If && statement->type != ast4vx::NodeType::Else)
                to_return += ";\n";
        }

        return to_return;
    }

    case ast4vx::NodeType::Reduction:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::Reduction>(n);
        std::string to_return;

        std::vector<std::string> mapping;

        mapping.emplace_back(accumulator_string);
        mapping.emplace_back(accumulator_string);
        mapping.emplace_back(windowdescriptor_mapping->at(0));

        auto old_pixel_mapping = pixelaccessor_mapping;
        pixelaccessor_mapping = &mapping;

        for(auto statement: s->statements)
        {
            to_return += this->visit(statement, i);
            if (statement->type != ast4vx::NodeType::If && statement->type != ast4vx::NodeType::Else)
                to_return += ";\n";
        }

        pixelaccessor_mapping = old_pixel_mapping;

        return to_return;
    }

    case ast4vx::NodeType::WindowAccessorPosition:
    {
        auto s = std::dynamic_pointer_cast<ast4vx::WindowAccessor::Position>(n);
        auto parent = s->parent.lock();

        if (windowdescriptor_mapping == nullptr)
            return "Window" + std::to_string(parent->num) + "(" + std::to_string(s->x)
                     + ", " + std::to_string(s->y) + ")";

        return windowdescriptor_mapping->at(parent->num) + ".get(" + std::to_string(s->x)
                 + ", " + std::to_string(s->y) + ")";
    }
    }
    throw std::runtime_error("CPP Generate: reached end of switch case");
}


std::string create_matrix_def(std::string type, std::string name, std::string w, std::string h)
{
    std::string matrix_def = "std::matrix<@@@DATATYPE@@@> @@@MATRIXNAME@@@(@@@WIDTH@@@, @@@HEIGHT@@@);\n";
    matrix_def = use_template(matrix_def, "DATATYPE", type);
    matrix_def = use_template(matrix_def, "MATRIXNAME", name);
    matrix_def = use_template(matrix_def, "WIDTH", w);
    matrix_def = use_template(matrix_def, "HEIGHT", h);
    return matrix_def;
}
std::string create_matrix_def(std::string type, std::string name, unsigned int w, unsigned int h)
{
    return create_matrix_def(type, name, std::to_string(w), std::to_string(h));
}

std::string CPPVisitor::setup_outer_loop(std::shared_ptr<DomVX::Map> m)
{
    std::string y_index_name = "y_" + std::to_string(m->id);
    std::string x_index_name = "x_" + std::to_string(m->id);
    std::string templ =
R"END(for(int @@@Y_NAME@@@ = 0; @@@Y_NAME@@@ < @@@HEIGHT@@@; @@@Y_NAME@@@++)
{
    for (int @@@X_NAME@@@ = 0; @@@X_NAME@@@ < @@@WIDTH@@@; @@@X_NAME@@@++)
    {
        @@@CODE@@@
    }
}
)END";
    templ = use_template(templ, "HEIGHT", m->output_pixel_mappings[0]->h);
    templ = use_template(templ, "WIDTH", m->output_pixel_mappings[0]->w);
    templ = use_template(templ, "Y_NAME", y_index_name);
    templ = use_template(templ, "X_NAME", x_index_name);

    current_output_y = y_index_name;
    current_output_x = x_index_name;
    current_output_width = std::to_string(m->output_pixel_mappings[0]->w);
    current_output_height = std::to_string(m->output_pixel_mappings[0]->h);

    return templ;
}
std::string CPPVisitor::setup_outer_loop(std::shared_ptr<DomVX::LocalOperation> l, const std::vector<HipaVX::Image*>& out)
{
    std::string y_index_name = "y_" + std::to_string(l->id);
    std::string x_index_name = "x_" + std::to_string(l->id);
    std::string templ =
R"END(for(int @@@Y_NAME@@@ = 0; @@@Y_NAME@@@ < @@@HEIGHT@@@; @@@Y_NAME@@@++)
{
    for (int @@@X_NAME@@@ = 0; @@@X_NAME@@@ < @@@WIDTH@@@; @@@X_NAME@@@++)
    {
        @@@CODE@@@
    }
}
)END";
    templ = use_template(templ, "HEIGHT", out[0]->h);
    templ = use_template(templ, "WIDTH", out[0]->w);
    templ = use_template(templ, "Y_NAME", y_index_name);
    templ = use_template(templ, "X_NAME", x_index_name);

    current_output_y = y_index_name;
    current_output_x = x_index_name;
    current_output_width = std::to_string(out[0]->w);
    current_output_height = std::to_string(out[0]->h);

    return templ;
}

std::string CPPVisitor::visit(std::shared_ptr<DomVX::AbstractionNode> n, int i)
{
    switch(n->type)
    {
    case DomVX::AbstractionType::Map:
    {
        auto s = std::dynamic_pointer_cast<DomVX::Map>(n);

        std::string outer_loop = setup_outer_loop(s);

        std::vector<std::string> mappings;

        for(auto& im: s->output_pixel_mappings)
            mappings.emplace_back(generate_image_name(im));
        for(auto& im: s->input_pixel_mappings)
            mappings.emplace_back(generate_image_name(im));

        pixelaccessor_mapping = &mappings;
        std::string code = visit(s->get_statements(), 0);
        pixelaccessor_mapping = nullptr;

        current_output_y = current_output_x = current_output_width = current_output_height = "";

        outer_loop = use_template(outer_loop, "CODE", code);
        return outer_loop;
    }
    case DomVX::AbstractionType::LocalOperation:
    {
        auto s = std::dynamic_pointer_cast<DomVX::LocalOperation>(n);


        std::vector<HipaVX::Image *> output_images;

        for(auto images: s->operation_output_images)
        {
            output_images.insert(output_images.end(), images.begin(), images.end());
        }

        std::string outer_loop = setup_outer_loop(s, output_images);
        std::string code = "";

        for(unsigned int i = 0; i < s->input_descriptor.size(); i++)
        {
            auto& in_image = std::get<0>(s->input_descriptor[i]);
            auto& in_desc = std::get<1>(s->input_descriptor[i]);
            std::string input_matrix_name = "temp_window_" + std::to_string(in_desc->id);

            std::string copy = "std::matrix<@@@DATATYPE@@@> @@@COPY_NAME@@@ = @@@IMAGE_NAME@@@.copy_roi<@@@DATATYPE@@@>(@@@X@@@, @@@Y@@@, @@@W@@@, @@@H@@@);\n";
            copy = use_template(copy, "COPY_NAME", input_matrix_name);
            copy = use_template(copy, "IMAGE_NAME", generate_image_name(in_image));
            copy = use_template(copy, "DATATYPE", to_string(in_desc->datatype));
            copy = use_template(copy, "X", current_output_x + "-(" + std::to_string(in_desc->width/2) + ")");
            copy = use_template(copy, "Y", current_output_y + "-(" + std::to_string(in_desc->height/2) + ")");
            copy = use_template(copy, "W", std::to_string(in_desc->width));
            copy = use_template(copy, "H", std::to_string(in_desc->height));

            code += copy;
            desc_to_name[in_desc] = input_matrix_name;
        }

        code += "\n";

        for(unsigned int i = 0; i < s->operations.size(); i++)
        {
            auto &op = s->operations[i];

            if (op->output != nullptr)
            {
                auto& in_desc = op->output;
                std::string matrix_name = "temp_window_" + std::to_string(in_desc->id);

                std::string copy = "std::matrix<@@@DATATYPE@@@> @@@COPY_NAME@@@ = @@@ORIG_NAME@@@.copy_roi<@@@DATATYPE@@@>(0, 0, @@@W@@@, @@@H@@@);\n";
                copy = use_template(copy, "COPY_NAME", matrix_name);
                copy = use_template(copy, "ORIG_NAME", desc_to_name[op->window_inputs[0]]);
                copy = use_template(copy, "DATATYPE", to_string(in_desc->datatype));
                copy = use_template(copy, "W", std::to_string(in_desc->width));
                copy = use_template(copy, "H", std::to_string(in_desc->height));

                code += copy;
                desc_to_name[in_desc] = matrix_name;
            }

            std::vector<std::string> pixel_mappings;
            std::vector<std::string> window_mappings;
            if (op->current_state == ast4vx::WindowOperation::State::ToPixel || op->current_state == ast4vx::WindowOperation::State::Reduce)
            {
                // Bind an actual output image is only relevant for ToPixel and Reduce
                for(auto &out_images: s->operation_output_images[i])
                    pixel_mappings.emplace_back(generate_image_name(out_images));
                for(auto &windesc: op->window_inputs)
                    window_mappings.emplace_back(desc_to_name[windesc]);
                windowdescriptor_mapping = &window_mappings;
            }
            else if (op->current_state == ast4vx::WindowOperation::State::At || op->current_state == ast4vx::WindowOperation::State::Forall)
            {
                pixel_mappings.emplace_back(desc_to_name[op->output]);
                for(auto& map: op->window_inputs)
                {
                    pixel_mappings.emplace_back(desc_to_name[map]);
                }
            }

            auto old_mapping = pixelaccessor_mapping;
            pixelaccessor_mapping = &pixel_mappings;

            code += visit(op) + "\n";

            pixelaccessor_mapping = old_mapping;
            windowdescriptor_mapping = nullptr;
        }

        current_output_y = current_output_x = current_output_width = current_output_height = "";
        outer_loop = use_template(outer_loop, "CODE", code);
        return outer_loop;
    }
    default:
        throw std::runtime_error("CPPVisitor: visited no case for this AbstractionNode");
    }
    return "";
}

