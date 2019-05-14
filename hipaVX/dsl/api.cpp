#include "api.hpp"

//*********************************************************************
// Image
//*********************************************************************
domVX_image create_image(unsigned int width, unsigned int height,
                         vx_df_image type) {
  return (new DomVX::Image(1024, 512, VX_DF_IMAGE_U8));
}

//*********************************************************************
// Descriptors
//*********************************************************************
domVX_window get_window(DomVX::Image* im, int width, int height) {
  auto bwd = std::make_shared<WindowDesc>();
  bwd->w_desc = std::make_shared<ast4vx::WindowDescriptor>(width, height);
  bwd->w_desc->bounded = bwd;
  bwd->im = im;
  std::vector<int> domain_values(width * height, 1);
  bwd->dom = std::make_shared<DomVX::Domain>(width, height, domain_values);
  return bwd;
}

domVX_window get_window(DomVX::Image* im, std::shared_ptr<DomVX::Domain> dom) {
  auto bwd = std::make_shared<WindowDesc>();
  bwd->w_desc =
      std::make_shared<ast4vx::WindowDescriptor>(dom->width, dom->height);
  bwd->w_desc->bounded = bwd;
  bwd->im = im;
  bwd->dom = dom;
  return bwd;
}

//*********************************************************************
// OPERATORS
//*********************************************************************
domVX_point create_point_op() {
  return std::shared_ptr<DomVX::PointOperator>(new DomVX::PointOperator());
}

domVX_local create_local_op() {
  return std::shared_ptr<DomVX::LocalOperator>(new DomVX::LocalOperator());
}

domVX_global create_global_op() {
  return std::shared_ptr<DomVX::GlobalOperator>(new DomVX::GlobalOperator());
}

//*********************************************************************
// DOMAIN  (TODO: Consider renaming as Window)
//*********************************************************************
domVX_domain create_domain(unsigned int x, unsigned int y,
                           std::vector<int> dom) {
  return std::shared_ptr<DomVX::Domain>(new DomVX::Domain(x, y, dom));
}

domVX_domain create_domain(domVX_mask mask) {
  std::vector<int> values;
  for (auto& row : mask->mask) {
    for (auto cell : row) {
      if (mask->mask_is_int)
        values.push_back((cell.i != 0) ? 1 : 0);
      else
        values.push_back((cell.f != 0.f) ? 1 : 0);
    }
  }
  return std::shared_ptr<DomVX::Domain>(
      new DomVX::Domain(mask->width, mask->height, values));
}

//*********************************************************************
// MASK
//*********************************************************************
domVX_mask create_mask(unsigned int w, unsigned int h,
                       std::initializer_list<int32_t> mask) {
  return std::shared_ptr<DomVX::Mask>(new DomVX::Mask(w, h, mask));
}

domVX_mask create_mask(unsigned int w, unsigned int h,
                       std::initializer_list<float> mask) {
  return std::shared_ptr<DomVX::Mask>(new DomVX::Mask(w, h, mask));
}
