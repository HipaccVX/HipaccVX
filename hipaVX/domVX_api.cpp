#include "domVX_api.hpp"

//******************************************************************************
// OPERATORS
//******************************************************************************
std::shared_ptr<DomVX::Map> create_point_op() {
  return std::shared_ptr<DomVX::Map>(new DomVX::Map());
}

std::shared_ptr<DomVX::LocalOperation> create_local_op() {
  return std::shared_ptr<DomVX::LocalOperation>(new DomVX::LocalOperation());
}

std::shared_ptr<DomVX::GlobalOperation> create_global_op() {
  return std::shared_ptr<DomVX::GlobalOperation>(new DomVX::GlobalOperation());
}

//******************************************************************************
// Image
//******************************************************************************
domVX_image create_image(unsigned int width, unsigned int height,
                         vx_df_image type) {
  return (new DomVX::Image(1024, 512, VX_DF_IMAGE_U8));
}

//******************************************************************************
// DOMAIN  (TODO: Consider renaming as Window)
//******************************************************************************
std::shared_ptr<DomVX::Domain> create_dom(unsigned int x, unsigned int y,
                                          std::vector<int> dom) {
  return std::shared_ptr<DomVX::Domain>(new DomVX::Domain(x, y, dom));
}

std::shared_ptr<DomVX::Domain> create_dom(std::shared_ptr<DomVX::Mask> mask) {
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

//******************************************************************************
// MASK
//******************************************************************************
std::shared_ptr<DomVX::Mask> create_mask(unsigned int x, unsigned int y,
                                         std::initializer_list<int32_t> mask) {
  return std::shared_ptr<DomVX::Mask>(new DomVX::Mask(x, y, mask));
}

std::shared_ptr<DomVX::Mask> create_mask(unsigned int x, unsigned int y,
                                         std::initializer_list<float> mask) {
  return std::shared_ptr<DomVX::Mask>(new DomVX::Mask(x, y, mask));
}
