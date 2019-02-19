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

std::string generate(ForEveryPixel *s)
{
    return "";
}

std::string generate_call(ForEveryPixel *fep)
{
    function_ast::Image *output = (function_ast::Image*) fep->output.get();

    std::string definitions = "";

    for(auto node: fep->inputs)
    {
        if (node->type == NodeType::Stencil)
        {
            auto s = std::dynamic_pointer_cast<Stencil>(node);

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

    std::string code = generate(&fep->function);




    string call = read_file(hipaVX_folder + "/templates/cpp_rasterscan.templ");

    call = use_template(call, "HEIGHT", ::generate_image_name(output->image) + ".h");
    call = use_template(call, "WIDTH", ::generate_image_name(output->image) + ".w");
    call = use_template(call, "DEFINITIONS", definitions);
    call = use_template(call, "CODE", code);
    return call;
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
            sources += function_ast::generate_call(&node->kernel) + '\n';
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
        string image_name = generate_image_name(image);

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
