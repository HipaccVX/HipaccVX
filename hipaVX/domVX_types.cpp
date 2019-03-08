#include "domVX_types.hpp"
#include "../VX/vx.h"

#include <iostream>

function_ast::Datatype convert_type(vx_df_image type) {
  switch (type) {
    case VX_DF_IMAGE_U8:
      return function_ast::Datatype::UINT8;
    case VX_DF_IMAGE_S16:
      return function_ast::Datatype::INT16;
    case VX_DF_IMAGE_U16:
      return function_ast::Datatype::UINT16;
    case VX_DF_IMAGE_S32:
      return function_ast::Datatype::INT32;
    case VX_DF_IMAGE_U32:
      return function_ast::Datatype::UINT32;
    case VX_TYPE_FLOAT32:
      return function_ast::Datatype::FLOAT;
  }
  throw std::runtime_error("Unknown type in function_ast::Datatype convert");
}

namespace HipaVX {
VertexTask set_task_from_type(HipaVX::ObjectType type) {
  switch (type) {
    case VX_TYPE_IMAGE:
    case VX_TYPE_SCALAR:
      return VertexTask::Buffer;
    case VX_TYPE_NODE:
      return VertexTask::Computation;
    default:
      throw std::runtime_error("Unsupported vx_type_e in set_task");
  }
}

int Object::next_id = 0;

// Graph
void Graph::build() {
  for (auto& node : graph) {
    node->build();
  }

  built = true;
}

}  // namespace HipaVX
