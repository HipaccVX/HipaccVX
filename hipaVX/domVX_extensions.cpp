#include "domVX_extensions.hpp"
#include "kernels/domVX_kernels.hpp"

vx_node vxHipaccNode(vx_graph graph, std::string filename,
                     vx_reference *parameters, vx_size count, vx_image out) {
  return nullptr;
}

vx_image vxCreateImageFromFile(vx_context context, vx_uint32 width,
                               vx_uint32 height, vx_df_image color,
                               std::string filename) {
  return nullptr;
}

vx_node vxFWriteImageNode(vx_graph graph, vx_image image, std::string file) {
  return nullptr;
}

static std::string get_object_name(DomVX::Object *object) { return nullptr; }

void vxDrawDotGraph(vx_graph graph, std::string filename,
                    vx_uint32 node_depth) {
  if (node_depth != 0)
    throw std::runtime_error("Currently only node_depth of 0 is supported");
  std::vector<DomVX::Node *> nodes = ((DomVX::Graph *)(graph->o))->graph;
  std::vector<DomVX::Object *> objects;

  std::string content;

  std::string edges;
  for (auto node : nodes) {
    auto inputs = node->inputs;
    auto outputs = node->outputs;

    for (auto input : inputs) {
      edges += "\tID_" + std::to_string(input->my_id) + " -> ID_" +
               std::to_string(node->my_id) + ";\n";
    }
    for (auto output : outputs) {
      edges += "\tID_" + std::to_string(node->my_id) + " -> ID_" +
               std::to_string(output->my_id) + ";\n";
    }

    objects.insert(objects.end(), inputs.begin(), inputs.end());
    objects.insert(objects.end(), outputs.begin(), outputs.end());
  }

  std::sort(objects.begin(), objects.end());
  objects.erase(std::unique(objects.begin(), objects.end()), objects.end());

  std::string node_definitions;
  for (auto node : nodes)
    node_definitions += "\tID_" + std::to_string(node->my_id) + " [label=\"" +
                        node->get_name() + "\", color=green];\n";
  for (auto object : objects)
    node_definitions += "\tID_" + std::to_string(object->my_id) + " [label=\"" +
                        get_object_name(object) +
                        "\", shape=box, color=blue];\n";

  content += node_definitions;
  content += edges;
  std::ofstream dot_writer(filename);
  dot_writer << "digraph graphname {\n" << content << "}";
}
