//#include "../VX/vx.h"
//#include "ast.hpp"
//#include <string>
//#include "domVX_types.hpp"
//#include "domVX_kernels.hpp"
#include "domVX_kernels.hpp"

namespace DomVX {
FileinputImage::FileinputImage(vx_uint32 width, vx_uint32 height,
                               vx_df_image color, std::string filename)
    : Image(width, height, color), file(filename) {}

WriteImageNode::WriteImageNode() { node_name = "Image Writer"; }
void WriteImageNode::build() { inputs.emplace_back(in); }
}  // namespace DomVX
