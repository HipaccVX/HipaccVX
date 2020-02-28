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

vx_kernel vxHipaccKernel(std::string filename) {
  auto kern = new DomVX::HipaccKernel();
  kern->filename = filename;
  auto vx = new _vx_kernel();
  vx->o = kern;
  return vx;
}

vx_kernel vxCppKernel(std::string filename) {
  auto kern = new DomVX::CppKernel();
  kern->filename = filename;
  auto vx = new _vx_kernel();
  vx->o = kern;
  return vx;
}

void set_output_filename(std::string filename)
{
    hipaVX_output_filename = filename;
}
