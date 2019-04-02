#include "domVX_types.hpp"
#include "../VX/vx.h"

#include <iostream>

ast4vx::Datatype convert_type(vx_df_image type) {
    switch (type) {
    case VX_DF_IMAGE_U8:
		return ast4vx::Datatype::UINT8;
    case VX_DF_IMAGE_S16:
		return ast4vx::Datatype::INT16;
    case VX_DF_IMAGE_U16:
		return ast4vx::Datatype::UINT16;
    case VX_DF_IMAGE_S32:
		return ast4vx::Datatype::INT32;
    case VX_DF_IMAGE_U32:
		return ast4vx::Datatype::UINT32;
    case VX_TYPE_FLOAT32:
		return ast4vx::Datatype::FLOAT;
  }
  throw std::runtime_error("Unknown type in function_ast::Datatype convert");
}

namespace HipaVX {

  // TOOD: extend this
VertexTask set_task_from_type(HipaVX::ObjectType _type) {
  switch (_type) {
    case VX_TYPE_NODE:
    case VX_TYPE_KERNEL:
    case VX_TYPE_CONVOLUTION:
      return VertexTask::Computation;
    case VX_TYPE_IMAGE:
    case VX_TYPE_SCALAR:
    case VX_TYPE_ARRAY:
    case VX_TYPE_MATRIX:
      return VertexTask::Buffer;
    case VX_TYPE_INVALID:
      throw std::runtime_error("set task: an object has a VX_TYPE_INVALID");
    default:
      return VertexTask::API;
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
