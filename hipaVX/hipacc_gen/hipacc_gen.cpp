#include "hipacc_gen.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <map>

using std::string;

// move this to domVX_main.cpp
std::vector<HipaVX::Image*> get_all_images(HipaVX::Graph *g)
{
    std::vector<HipaVX::Image*> images;
    std::vector<HipaVX::Node*> nodes = g->graph;

    while(!nodes.empty())
    {
        auto node = nodes[nodes.size() - 1];
        nodes.pop_back();

        for(auto ref: node->get_inputs())
        {
            if (ref->type == VX_TYPE_IMAGE)
                images.push_back((HipaVX::Image*) ref);
        }
        for(auto ref: node->get_outputs())
        {
            if (ref->type == VX_TYPE_IMAGE)
                images.push_back((HipaVX::Image*) ref);
        }

        auto subnodes = node->get_subnodes();
        nodes.insert(nodes.end(), subnodes.begin(), subnodes.end());
    }

    std::sort(images.begin(), images.end());
    images.erase(std::unique(images.begin(), images.end()), images.end());

    return images;
}

void process_graph(HipaVX::Graph *graph)
{
    string main = read_file(hipaVX_folder + "/templates/hipacc_main.templ");

    string images;
    const string image_decl_template = "\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@);\n";

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
                fim_decl_template = "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = (@@@DATATYPE@@@*) load_data<uchar>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, 4, \"@@@FILENAME@@@\");\n"
                                    "\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
            }
            else
            {
                fim_decl_template = "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = load_data<@@@DATATYPE@@@>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, 1, \"@@@FILENAME@@@\");\n"
                                    "\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
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

        temp = use_template(temp, "DATATYPE", VX_DF_IMAGE_to_hipacc[image->col]);
        temp = use_template(temp, "IMAGE_NAME", image_name);
        temp = use_template(temp, "IMAGE_WIDTH", image->w);
        temp = use_template(temp, "IMAGE_HEIGHT", image->h);

        images += temp;
    }

    main = use_template(main, "IMAGE_DEFINITIONS", images);

    string node_definitions;
    for(const auto node: graph->graph)
    {
        node_definitions += node->generateClassDefinition();
    }

    main = use_template(main, "KERNEL_DEFINITIONS", node_definitions);

    string node_calls;
    for(const auto node: graph->graph)
    {
        //node_calls += node->generateNodeCall();
        node_calls += function_ast::generate_call(&node->kernel);
    }

    main = use_template(main, "KERNEL_CALLS", node_calls);

    _write_to_file("main.hipaVX.cpp", main);
}
