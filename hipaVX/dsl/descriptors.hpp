#pragma once

#include <memory>

namespace ast4vx {
class WindowDescriptor;
}
namespace DomVX {
class Image;
class Domain;
}  // namespace DomVX

class WindowDesc {
 public:
  std::shared_ptr<ast4vx::WindowDescriptor> w_desc;
  DomVX::Image* im;
  std::shared_ptr<DomVX::Domain> dom;
};
