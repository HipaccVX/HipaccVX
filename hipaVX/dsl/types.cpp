#include "types.hpp"

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

namespace DomVX {

// TOOD: extend this
ObjectTask set_task_from_type(DomVX::ObjectType _type) {
  switch (_type) {
    case VX_TYPE_NODE:
    case VX_TYPE_KERNEL:
    case VX_TYPE_CONVOLUTION:
      return ObjectTask::Computation;
    case VX_TYPE_IMAGE:
    case VX_TYPE_SCALAR:
    case VX_TYPE_ARRAY:
    case VX_TYPE_MATRIX:
      return ObjectTask::Buffer;
    case VX_TYPE_INVALID:
      throw std::runtime_error("set task: an object has a VX_TYPE_INVALID");
    default:
      return ObjectTask::API;
  }
}

int Object::next_id = 0;
int Acc::next_id = 0;

// Graph
void Graph::build() {
  for (auto& node : graph) {
    node->build();
  }

  built = true;
}

}  // namespace DomVX
