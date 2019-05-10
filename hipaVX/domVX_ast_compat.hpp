#pragma once

#include <memory>

namespace ast4vx {
class WindowDescriptor;
}
namespace DomVX {
class Image;
class Domain;
}  // namespace DomVX

class BoundedWindowDescriptor {
 public:
  std::shared_ptr<ast4vx::WindowDescriptor> w_desc;
  DomVX::Image* im;
  std::shared_ptr<DomVX::Domain> dom;
};

std::shared_ptr<BoundedWindowDescriptor> get_window(DomVX::Image* im, int width,
                                                    int height);

std::shared_ptr<BoundedWindowDescriptor> get_window(
    DomVX::Image* im, std::shared_ptr<DomVX::Domain> dom);


// TODO: Maybe we can seperate normal ast functions from
//       ast_accessors (pix2pix, win2pix; forall etc)
