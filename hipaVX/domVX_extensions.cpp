#include "domVX_extensions.hpp"

vx_node vxHipaccNode(vx_graph graph, std::string filename, vx_reference *parameters, vx_size count, vx_image out)
{
    HipaVX::HipaccNode *hipaccNode = new HipaVX::HipaccNode();

    hipaccNode->filename = filename;
    hipaccNode->out = out;
    for(vx_size i = 0; i < count; i++)
        hipaccNode->parameters.push_back(parameters[i]);

    graph->graph.emplace_back(hipaccNode);
    graph->built = false;

    return hipaccNode;
}

vx_image vxCreateImageFromFile(vx_context context, vx_uint32 width, vx_uint32 height, vx_df_image color, std::string filename)
{
    HipaVX::FileinputImage *image;
    image = new HipaVX::FileinputImage(width, height, color, filename);
    context->images.emplace_back(image);
    return image;
}

vx_node vxFWriteImageNode(vx_graph graph, vx_image image, std::string file)
{
    HipaVX::WriteImageNode *win = new HipaVX::WriteImageNode();
    win->in = image;
    win->out_file = file;
    graph->graph.emplace_back(win);
    graph->built = false;
    return win;
}

static std::string get_object_name(HipaVX::Object *object)
{
    std::string type;

    switch(object->type)
    {
    case VX_TYPE_IMAGE:
        type = "Image";
        break;
    case VX_TYPE_SCALAR:
        type = "Scalar";
        break;
    case VX_TYPE_MATRIX:
    case VX_TYPE_HIPAVX_MATRIX:
        type = "Matrix";
        break;
    case VX_TYPE_THRESHOLD:
        type = "Threshold";
        break;
    case VX_TYPE_CONVOLUTION:
        type = "Convolution";
        break;
    default:
        throw std::runtime_error("static std::string generate_object_name(HipaVX::Object *object):\n\tUnsupported objecttype");
    }

    return type + "_" + std::to_string(object->my_id);
}

void vxDrawDotGraph(vx_graph graph, std::string filename, vx_uint32 node_depth)
{
    if (node_depth != 0)
        throw std::runtime_error("Currently only node_depth of 0 is supported");
    std::vector<HipaVX::Node*> nodes = graph->graph;
    std::vector<HipaVX::Object*> objects;

    std::string content;

    std::string edges;
    for(auto node: nodes)
    {
        auto inputs = node->get_inputs();
        auto outputs = node->get_outputs();

        for(auto input: inputs)
        {
            edges += "\tID_" + std::to_string(input->my_id) + " -> ID_" + std::to_string(node->my_id) + ";\n";
        }
        for(auto output: outputs)
        {
            edges += "\tID_" + std::to_string(node->my_id) + " -> ID_" + std::to_string(output->my_id) + ";\n";
        }

        objects.insert(objects.end(), inputs.begin(), inputs.end());
        objects.insert(objects.end(), outputs.begin(), outputs.end());
    }

    std::sort(objects.begin(), objects.end());
    objects.erase(std::unique(objects.begin(), objects.end()), objects.end());


    std::string node_definitions;
    for(auto node: nodes)
        node_definitions += "\tID_" + std::to_string(node->my_id) + " [label=\"" + node->get_name() + "\", color=green];\n";
    for(auto object: objects)
        node_definitions += "\tID_" + std::to_string(object->my_id) + " [label=\"" + get_object_name(object) + "\", shape=box, color=blue];\n";

    content += node_definitions;
    content += edges;
    std::ofstream dot_writer(filename);
    dot_writer << "digraph graphname {\n" << content << "}";
}