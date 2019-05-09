#include "domVX_ast_compat.hpp"
#include <vector>
#include "abstractions.hpp"
#include "ast.hpp"

std::shared_ptr<BoundedWindowDescriptor> get_window(DomVX::Image* im, int width,
                                                    int height) {
  auto bwd = std::make_shared<BoundedWindowDescriptor>();
  bwd->w_desc = std::make_shared<ast4vx::WindowDescriptor>(width, height);
  bwd->w_desc->bounded = bwd;
  bwd->im = im;
  std::vector<int> domain_values(width * height, 1);
  bwd->dom = std::make_shared<DomVX::Domain>(width, height, domain_values);
  return bwd;
}

std::shared_ptr<BoundedWindowDescriptor> get_window(
    DomVX::Image* im, std::shared_ptr<DomVX::Domain> dom) {
  auto bwd = std::make_shared<BoundedWindowDescriptor>();
  bwd->w_desc =
      std::make_shared<ast4vx::WindowDescriptor>(dom->width, dom->height);
  bwd->w_desc->bounded = bwd;
  bwd->im = im;
  bwd->dom = dom;
  return bwd;
}
