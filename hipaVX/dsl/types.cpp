#include "types.hpp"

namespace DomVX {

// TOOD: extend this
ObjectTask set_task_from_type(DomVX::ObjectType _type) {
  switch (_type) {
    case VX_TYPE_NODE:
    case VX_TYPE_KERNEL:
    case VX_TYPE_CONVOLUTION:
      return ObjectTask::Computation;
    case VX_TYPE_IMAGE:
    case VX_TYPE_BOOL:
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

}  // namespace DomVX
