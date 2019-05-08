#include "domVX_kernels_point.hpp"

#pragma once

// namespace HipaVX
// {
// class FileinputImage;
// class WriteImageNode;
// class HipaccNode;
// template<typename T> class Mask2D;
// template <typename T> class LinearMask;
// class Sobel3x3Node;
// class BoxFilter;
// class GaussianFilter;
// class Dilate;
// class Erode;
// class Add3_3;
// class HarrisCorners;
// class VXConvolveNode;
// class VXBilateralFilterNode;
// class AnotherBilateralFilterNode;
// }
//

namespace generator
{
enum class Type
{
    Definition = 0,
    Call
};
}

namespace DomVX
{
class FileinputImage: public Image
{
public:
    FileinputImage(vx_uint32 width, vx_uint32 height, vx_df_image color, std::string filename);
    virtual ~FileinputImage() = default;
    std::string file;
};

class WriteImageNode: public Node
{
public:
    WriteImageNode();
    virtual ~WriteImageNode() override = default;
    Image *in;
    std::string out_file;
    virtual void build() override;
};
}
