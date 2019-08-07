#include "domVX_extensions.hpp"
#include <algorithm>
#include "dsl/abstractions.hpp"
#include "graph/graph.hpp"

vx_node vxHipaccNode(vx_graph graph, std::string filename,
                     vx_reference *parameters, vx_size count, vx_image out) {
  return nullptr;
}

vx_image vxCreateImageFromFile(vx_context context, vx_uint32 width,
                               vx_uint32 height, vx_df_image color,
                               std::string filename) {
  DomVX::FileinputImage *image;
  image = new DomVX::FileinputImage(width, height, color, filename);
  auto vx = new _vx_image();
  vx->o = image;
  ((DomVX::Context *)(context->o))->images.emplace_back(image);
  return vx;
}

void vxWriteImageAfterGraphCompletion(vx_graph graph, vx_image image,
                                      std::string file) {
  ((DomVX::Graph *)(graph->o))
      ->write_after_completion[(DomVX::Image *)(image->o)] = file;
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
      edges += "\tID_" + input->id() + " -> ID_" + node->id() + ";\n";
    }
    for (auto output : outputs) {
      edges += "\tID_" + node->id() + " -> ID_" + output->id() + ";\n";
    }

    objects.insert(objects.end(), inputs.begin(), inputs.end());
    objects.insert(objects.end(), outputs.begin(), outputs.end());
  }

  std::sort(objects.begin(), objects.end());
  objects.erase(std::unique(objects.begin(), objects.end()), objects.end());

  std::string node_definitions;
  for (auto node : nodes)
    node_definitions += "\tID_" + node->id() + " [label=\"" + node->name() +
                        "\", color=green];\n";
  for (auto object : objects)
    node_definitions += "\tID_" + object->id() + " [label=\"" +
                        get_object_name(object) +
                        "\", shape=box, color=blue];\n";

  content += node_definitions;
  content += edges;
  std::ofstream dot_writer(filename);
  dot_writer << "digraph graphname {\n" << content << "}";
}

vx_kernel vxHipaccKernel(std::string filename) {
  auto kern = new DomVX::HipaccKernel();
  kern->filename = filename;
  auto vx = new _vx_kernel();
  vx->o = kern;
  return vx;
}
