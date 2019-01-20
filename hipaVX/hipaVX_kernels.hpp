#ifndef __HIPAVX_KERNELS__
#define __HIPAVX_KERNELS__

#include "../VX/vx.h"
#include "ast.hpp"
#include "domVX_types.hpp"

#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <memory>
#include <string>
#include <algorithm>
#include <memory>

// TODO: consider writing individual files for every node type (parallel pattern abstractions)

namespace HipaVX
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
    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class ConvertDepthNode: public Node
{
public:
    ConvertDepthNode();
    virtual ~ConvertDepthNode() override = default;
    Image *in;
    Image *out;

    vx_enum policy;
    Scalar *shift;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class NotNode: public Node
{
public:
    NotNode();
    virtual ~NotNode() override = default;
    Image *in;
    Image *out;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};


class HipaccNode: public Node
{
public:
    HipaccNode();
    std::string filename;
    Image *out;
	std::vector<Object*> parameters;



    virtual ~HipaccNode() override = default;

    std::string kernel_name;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};


class SimplePoint: public Node
{
public:
    virtual ~SimplePoint() override = default;

    Image *in_1;
    Image *in_2;
	Image *out;

	function_ast::NodeType operation;
	function_ast::ForEveryPixel kernel;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
	virtual void build() override;
};
class SimplePointAdd: public SimplePoint
{
public:
    SimplePointAdd()
    {
		operation = function_ast::NodeType::Add;
        node_name = "Point Addition";
    }
    virtual ~SimplePointAdd() override = default;
};
class SimplePointSub: public SimplePoint
{
public:
    SimplePointSub()
    {
		operation = function_ast::NodeType::Sub;
        node_name = "Point Subtraction";
    }
    virtual ~SimplePointSub() override = default;
};
class SimplePointMul: public SimplePoint
{
public:
    SimplePointMul()
    {
		operation = function_ast::NodeType::Mul;
        node_name = "Point Multiplication";
    }
    virtual ~SimplePointMul() override = default;
};
class SimplePointDiv: public SimplePoint
{
public:
    SimplePointDiv()
    {
		operation = function_ast::NodeType::Div;
        node_name = "Point Division";
    }
    virtual ~SimplePointDiv() override = default;
};
class AndNode: public SimplePoint
{
public:
    AndNode()
    {
		operation = function_ast::NodeType::BitwiseAnd;
        node_name = "And";
    }
    virtual ~AndNode() override = default;
};
class XorNode: public SimplePoint
{
public:
    XorNode()
    {
		operation = function_ast::NodeType::BitwiseXor;
        node_name = "Xor";
    }
    virtual ~XorNode() override = default;
};
class OrNode: public SimplePoint
{
public:
    OrNode()
    {
		operation = function_ast::NodeType::BitwiseOr;
        node_name = "or";
    }
    virtual ~OrNode() override = default;
};


template <typename T>
class SimplePointScalar: public Node
{
public:
    virtual ~SimplePointScalar() override = default;

    Image *in;
    T scalar;
    Image *out;
    std::string operation;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};
template <typename T>
class SimplePointScalarAdd: public SimplePointScalar<T>
{
public:
    SimplePointScalarAdd()
    {
        this->operation = "+";
        this->node_name = "Scalar Addition";
    }
    virtual ~SimplePointScalarAdd() override = default;
};
template <typename T>
class SimplePointScalarSub: public SimplePointScalar<T>
{
public:
    SimplePointScalarSub()
    {
        this->operation = "-";
        this->node_name = "Scalar Subtraction";
    }
    virtual ~SimplePointScalarSub() override = default;
};
template <typename T>
class SimplePointScalarMul: public SimplePointScalar<T>
{
public:
    SimplePointScalarMul()
    {
        this->operation = "*";
        this->node_name = "Scalar Multiplication";
    }
    virtual ~SimplePointScalarMul() override = default;
};
template <typename T>
class SimplePointScalarDiv: public SimplePointScalar<T>
{
public:
    SimplePointScalarDiv()
    {
        this->operation = "/";
        this->node_name = "Scalar Division";
    }
    virtual ~SimplePointScalarDiv() override = default;
};

class SquareNode: public Node
{
public:
    SquareNode();
    virtual ~SquareNode() override = default;
    Image *in;
    Image *out;

    SimplePointMul mul_node;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class SaturateNode: public Node
{
public:
    SaturateNode();
    virtual ~SaturateNode() override = default;
    Image *in;
    Image *out;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

template<typename T>
class Mask2D: public Object
{
public:
    Mask2D()
    {
        type = VX_TYPE_HIPAVX_MATRIX;
    }

    std::vector<T> mask;
    int dim[2]; //dim[0] = x, dim[1] = y

    void from_VX_Matrix(VX_Matrix *m);
};

template <typename T>
class LinearMask: public Node
{
public:
    LinearMask();
    virtual ~LinearMask() override = default;
    Mask2D<T> mask;
    std::unique_ptr<Scalar> normalization;

    Image *in;
    Image *out;

    bool use_image_datatype_for_sum = true;
    vx_df_image sum_datatype;

	function_ast::ReduceAroundPixel::Type r_type = function_ast::ReduceAroundPixel::Type::SUM;
	std::shared_ptr<function_ast::Stencil> stencil;
	function_ast::ForEveryPixel kernel;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class Sobel3x3Node: public Node
{
public:
    Sobel3x3Node();
    virtual ~Sobel3x3Node() override = default;

    Image *in;
    Image *out_x;
    Image *out_y;


    std::shared_ptr<function_ast::Stencil> stencil_x;
    std::shared_ptr<function_ast::Stencil> stencil_y;

    function_ast::ForEveryPixel kernel_x;
    function_ast::ForEveryPixel kernel_y;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class BoxFilter: public LinearMask<int>
{
public:
    BoxFilter()
    {
        this->mask.mask = {1, 1, 1, 1, 1, 1, 1, 1, 1};
        this->mask.dim[0] = this->mask.dim[1] = 3;
        float norm = 1.0f / 9;
        Scalar* norm_scalar = new Scalar(VX_TYPE_FLOAT32, &norm);
        this->normalization.reset(norm_scalar);
        this->use_image_datatype_for_sum = false;
        this->sum_datatype = VX_DF_IMAGE_S16;
        this->node_name = "Box Filter 3x3";
    }
    virtual ~BoxFilter() override = default;
};
class GaussianFilter: public LinearMask<int>
{
public:
    GaussianFilter()
    {
        this->mask.mask = {1, 2, 1, 2, 4, 2, 1, 2, 1};
        this->mask.dim[0] = this->mask.dim[1] = 3;
        float norm = 1.0f / 16;
        Scalar* norm_scalar = new Scalar(VX_TYPE_FLOAT32, &norm);
        this->normalization.reset(norm_scalar);
        this->use_image_datatype_for_sum = false;
        this->sum_datatype = VX_DF_IMAGE_S16;
        this->node_name = "Gaussian Filter 3x3";
    }
    virtual ~GaussianFilter() override = default;
};
class Dilate: public LinearMask<int>
{
public:
	Dilate()
	{
        this->mask.mask = {1, 1, 1, 1, 1, 1, 1, 1, 1};
        this->mask.dim[0] = this->mask.dim[1] = 3;
		this->r_type = function_ast::ReduceAroundPixel::Type::MAX;
		this->use_image_datatype_for_sum = true;
		this->node_name = "Dilate";
	}
	virtual ~Dilate() override = default;
};
class Erode: public LinearMask<int>
{
public:
	Erode()
	{
        this->mask.mask = {1, 1, 1, 1, 1, 1, 1, 1, 1};
        this->mask.dim[0] = this->mask.dim[1] = 3;
		this->r_type = function_ast::ReduceAroundPixel::Type::MIN;
		this->use_image_datatype_for_sum = true;
		this->node_name = "Erode";
	}
	virtual ~Erode() override = default;
};


class Add3_3: public Node
{
public:
    Add3_3();
    virtual ~Add3_3() override = default;

    LinearMask<int> add;

    Image *in;
    Image *out;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class HarrisCorners: public Node
{
public:
    HarrisCorners(Image *in)
        :in(in),
          sob_x(in->w, in->h, VX_TYPE_FLOAT32),
          sob_y(in->w, in->h, VX_TYPE_FLOAT32),
          Gx(in->w, in->h, VX_TYPE_FLOAT32),
          Gy(in->w, in->h, VX_TYPE_FLOAT32),
          square_Gx(in->w, in->h, VX_TYPE_FLOAT32),
          square_Gy(in->w, in->h, VX_TYPE_FLOAT32),
          square_Gx_sum(in->w, in->h, VX_TYPE_FLOAT32),
          square_Gy_sum(in->w, in->h, VX_TYPE_FLOAT32),
          trace_A(in->w, in->h, VX_TYPE_FLOAT32),
          det_A_minuend(in->w, in->h, VX_TYPE_FLOAT32),
          det_A_mul_Gx_Gy(in->w, in->h, VX_TYPE_FLOAT32),
          det_A_mul_Gx_Gy_sum(in->w, in->h, VX_TYPE_FLOAT32),
          det_A_subtrahend(in->w, in->h, VX_TYPE_FLOAT32),
          det_A(in->w, in->h, VX_TYPE_FLOAT32),
          trace_A_square(in->w, in->h, VX_TYPE_FLOAT32),
          trace_A_square_k(in->w, in->h, VX_TYPE_FLOAT32),
          Mc(in->w, in->h, VX_TYPE_FLOAT32),
          Vc(in->w, in->h, VX_TYPE_FLOAT32)
    {
        node_name = "Harris Corner";
    }
    virtual ~HarrisCorners() override = default;

    Sobel3x3Node sobel;
    SimplePointScalarMul<float> sobel_x_norm;//Gx
    SimplePointScalarMul<float> sobel_y_norm;//Gy
    SimplePointMul sobel_x_square;//Gx²
    SimplePointMul sobel_y_square;//Gx²

    SimplePointMul sobel_x_y;//Gx*Gy

    Add3_3 gx_square_A;
    Add3_3 gy_square_A;
    Add3_3 gx_gy_A;

    SimplePointAdd trace_add;

    SimplePointMul gx_A_gy_A; //gx_A² * gy_A²
    SimplePointMul gx_gy_A_square; //gx_gy_A * gx_gy_A
    SimplePointSub det_kernel;

    SimplePointMul trace_A_square_kernel;

    SimplePointScalarMul<float> trace_A_square_k_kernel;

    SimplePointSub Mc_kernel;




    Image *in;
    Scalar *strength_thresh;
    Scalar *min_distance;
    Scalar *sensitivity;
    vx_int32 gradient_size;
    vx_int32 block_size;
    Array *corners;
    Scalar *num_corners;




    Image sob_x;
    Image sob_y;

    Image Gx;
    Image Gy;

    Image square_Gx;
    Image square_Gy;

    Image square_Gx_sum;
    Image square_Gy_sum;

    //Mask A
    Image trace_A;


    Image det_A_minuend;

    Image det_A_mul_Gx_Gy;
    Image det_A_mul_Gx_Gy_sum;
    Image det_A_subtrahend;

    Image det_A;

    Image trace_A_square;
    Image trace_A_square_k;
    Image Mc;




    Image Vc;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class UnaryFunctionNode: public Node
{
public:
    UnaryFunctionNode();
    virtual ~UnaryFunctionNode() override = default;
    Image *in;
    Image *out;

    std::string function;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class SqrtNode: public Node
{
public:
    SqrtNode();
    virtual ~SqrtNode() override = default;
    Image *in;
    Image *out;

    UnaryFunctionNode function_node;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};
class AbsNode: public Node
{
public:
    AbsNode();
    virtual ~AbsNode() override = default;
    Image *in;
    Image *out;

    UnaryFunctionNode function_node;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};
class Atan2Node: public Node
{
public:
    Atan2Node();
    virtual ~Atan2Node() override = default;
    Image *in;
    Image *out;

    UnaryFunctionNode function_node;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};


class AbsDiffNode: public Node
{
public:
    AbsDiffNode();
    virtual ~AbsDiffNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *out;

    SimplePointSub diff_node;
    std::unique_ptr<Image> diff_image;

    AbsNode abs_node;
    std::unique_ptr<Image> abs_image;

    SaturateNode saturate_node;

    bool saturate;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXAddNode: public Node
{
public:
    VXAddNode();
    virtual ~VXAddNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *out;

    SimplePointAdd add_node;
    std::unique_ptr<Image> add_image;

    SaturateNode saturate_node;

    vx_enum policy;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};
class VXSubtractNode: public Node
{
public:
    VXSubtractNode();
    virtual ~VXSubtractNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *out;

    SimplePointSub diff_node;
    std::unique_ptr<Image> diff_image;

    SaturateNode saturate_node;

    vx_enum policy;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class ImageComparision
{
public:
    std::string comp_op;
    std::string value;
    bool image_is_first_operand = true;
};

template <typename T>
class ConditionalAssignmentNode: public Node
{
public:
    ConditionalAssignmentNode();
    virtual ~ConditionalAssignmentNode() override = default;

    Image *in;
    Image *out;

    std::vector<ImageComparision> comparision;
    std::vector<std::string> chaining_operators;
    T true_value;
    T false_value;

    bool use_image_datatype_for_sum = true;
    vx_df_image sum_datatype;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class VXThresholdNode: public Node
{
public:
    VXThresholdNode();
    virtual ~VXThresholdNode() override = default;

    Image *in;
    Image *out;
    Threshold *threshold;

    ConditionalAssignmentNode<int> comparision_node;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class MagnitudeNode: public Node
{
public:
    MagnitudeNode();
    virtual ~MagnitudeNode() override = default;

    Image *in_1;
    Image *in_2;

    Image *out;

    SquareNode grad_x_square_node;
    std::unique_ptr<Image> grad_x_square_image;
    SquareNode grad_y_square_node;
    std::unique_ptr<Image> grad_y_square_image;

    SimplePointAdd add_node;
    std::unique_ptr<Image> add_image;

    SqrtNode sqrt_node;
    std::unique_ptr<Image> sqrt_image;

    SaturateNode saturate_node;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class PhaseNode: public Node
{
public:
    PhaseNode();
    virtual ~PhaseNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *out;

    SimplePointDiv div_node;
    std::unique_ptr<Image> div_image;

    Atan2Node atan2_node;
    std::unique_ptr<Image> atan2_image;

    SimplePointScalarMul<float> mapping_node;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXMultiplyNode: public Node
{
public:
    VXMultiplyNode();
    virtual ~VXMultiplyNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *out;
    Scalar *scalar;

    vx_enum overflow_policy;
    vx_enum rounding_policy;


    SimplePointMul mul_node;
    std::unique_ptr<Image> mul_image;

    SimplePointScalarMul<float> mul_scalar_node;
    std::unique_ptr<Image> mul_scalar_image;

    SaturateNode saturate_node;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXAccumulateNode: public Node
{
public:
    VXAccumulateNode();
    virtual ~VXAccumulateNode() override = default;
    Image *in;
    Image *in_out;

    SimplePointAdd add_node;
    std::unique_ptr<Image> add_image;

    SaturateNode saturate_node;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXAccumulateSquareNode: public Node
{
public:
    VXAccumulateSquareNode();
    virtual ~VXAccumulateSquareNode() override = default;
    Image *in;
    Image *in_out;
    Scalar *shift;

    SquareNode square_node;
    std::unique_ptr<Image> square_image;

    ConvertDepthNode depth_node;
    std::unique_ptr<Scalar> depth_scalar;
    std::unique_ptr<Image> depth_image;

    SimplePointAdd add_node;
    std::unique_ptr<Image> add_image;

    SaturateNode saturate_node;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXAccumulateWeightedNode: public Node
{
public:
    VXAccumulateWeightedNode();
    virtual ~VXAccumulateWeightedNode() override = default;
    Image *in;
    Image *in_out;
    Scalar *alpha;


    SimplePointScalarMul<float> mul_scalar_left_node;
    std::unique_ptr<Image> mul_scalar_left_image;

    SimplePointScalarMul<float> mul_scalar_right_node;
    std::unique_ptr<Image> mul_scalar_right_image;

    SimplePointAdd add_node;
    std::unique_ptr<Image> add_image;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXChannelExtractNode: public Node
{
public:
    VXChannelExtractNode();
    virtual ~VXChannelExtractNode() override = default;
    Image *in;
    vx_channel_e channel;
    Image *out;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class VXChannelCombineNode: public Node
{
public:
    VXChannelCombineNode();
    virtual ~VXChannelCombineNode() override = default;
    Image *in_1;
    Image *in_2;
    Image *in_3;
    Image *in_4;

    Image *out;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class VXConvolveNode: public Node
{
public:
    VXConvolveNode();
    virtual ~VXConvolveNode() override = default;
    Image *in;
    Image *out;
    Convolution *convolution;

    LinearMask<short> lin_mask_node;
    std::unique_ptr<Image> lin_mask_image;

    SaturateNode saturate_node;


    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::vector<Node*> get_subnodes() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

class VXScaleNode: public Node
{
public:
    VXScaleNode();
    virtual ~VXScaleNode() override = default;

    Image *in;
    Image *out;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class VXBilateralFilterNode: public Node
{
public:
    VXBilateralFilterNode();
    virtual ~VXBilateralFilterNode() override = default;

    Image *in;
    Image *out;
    vx_int32 diameter;
    vx_float32 sigmaSpace;
    vx_float32 sigmaValues;

    function_ast::ForEveryPixel kernel;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
};

class AnotherBilateralFilterNode: public Node
{
public:
    AnotherBilateralFilterNode();
    virtual ~AnotherBilateralFilterNode() override = default;

    Image *in;
    Image *out;

    vx_int32 sigma_r;

    std::shared_ptr<function_ast::Stencil> stencil;
    std::shared_ptr<function_ast::ForEveryPixel> kernel;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual std::string generateClassDefinition() override;
    virtual std::string generateNodeCall() override;
    virtual void build() override;
};

}

namespace generator
{
enum class Type
{
    Definition = 0,
    Call
};

std::string node_generator(HipaVX::WriteImageNode* n, Type t);
std::string node_generator(HipaVX::ConvertDepthNode* n, Type t);

std::string node_generator(HipaVX::NotNode* n, Type t);

std::string node_generator(HipaVX::VXChannelExtractNode* n, Type t);
std::string node_generator(HipaVX::VXChannelCombineNode* n, Type t);
template <typename T>
std::string node_generator(HipaVX::SimplePointScalar<T>* n, Type t);
std::string node_generator(HipaVX::HarrisCorners* n, Type t);

std::string node_generator(HipaVX::SaturateNode* n, Type t);
std::string node_generator(HipaVX::UnaryFunctionNode* n, Type t);
template <typename T>
std::string node_generator(HipaVX::ConditionalAssignmentNode<T>* n, Type t);

std::string node_generator(HipaVX::HipaccNode *n, Type t);
std::string node_generator(HipaVX::VXScaleNode *n, Type t);

}


namespace HipaVX
{
template<typename T>
void Mask2D<T>::from_VX_Matrix(VX_Matrix *m)
{
    dim[0] = m->rows;
    dim[1] = m->columns;

    T* data = (T*) m->mat.data();

    mask.clear();
    for(unsigned int i = 0; i < dim[0] * dim[1]; i++)
    {
        mask.push_back(data[i]);
    }
}




template <typename T>
LinearMask<T>::LinearMask()
{
    node_name = "Linear Mask";
}
template <typename T>
std::vector<Object *> LinearMask<T>::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
	if (normalization != nullptr)
		used_objects.emplace_back(normalization.get());
    return used_objects;
}
template <typename T>
std::vector<Object *> LinearMask<T>::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}
template <typename T>
std::string LinearMask<T>::generateClassDefinition()
{
	std::string s = function_ast::generate(&kernel);
	return s;
}
template <typename T>
std::string LinearMask<T>::generateNodeCall()
{
	std::string s = function_ast::generate_call(&kernel);
	return s;
}
template <typename T>
void LinearMask<T>::build()
{
	stencil = std::make_shared<function_ast::Stencil>();
    stencil->dim[0] = mask.dim[0];
    stencil->dim[1] = mask.dim[1];
    stencil->mask = function_ast::Stencil::from_t<T>(mask.mask);
	stencil->name = "stencil";
	if (std::is_same<T, int>::value)
		stencil->datatype = function_ast::Datatype::INT32;
	else if (std::is_same<T, float>::value)
		stencil->datatype = function_ast::Datatype::FLOAT;
	else if (std::is_same<T, uint8_t>::value)
		stencil->datatype = function_ast::Datatype::UINT8;
	else if (std::is_same<T, int16_t>::value)
		stencil->datatype = function_ast::Datatype::INT16;
	else
		throw std::runtime_error("void LinearMask<T>::build() error: no datatype for Stencil");

	auto in_node = std::make_shared<function_ast::Image>(in);
	kernel.inputs.push_back(in_node);
	kernel.inputs.push_back(stencil);
	auto out_node = std::make_shared<function_ast::Image>(out);
	kernel.output = out_node;

	auto reduce = std::make_shared<function_ast::ReduceAroundPixel>();

	auto reduce_body = std::make_shared<function_ast::Statements>();
	*reduce_body << assign(reduce->reduction_output, reduce->stencil_value * reduce->pixel_value);

	reduce->subnodes[0] = in_node;
	reduce->subnodes[1] = stencil;
	reduce->subnodes[2] = reduce_body;
	reduce->reduction_type = r_type;
	reduce->datatype = (use_image_datatype_for_sum) ? convert_type(out->col) : convert_type(sum_datatype);

	auto temp_variable = std::make_shared<function_ast::Variable>("temp_variable", reduce->datatype);
	kernel.function << define(temp_variable) << assign(temp_variable, reduce);
	if (normalization != nullptr)
	{
		switch(normalization->data_type)
		{
		case VX_TYPE_UINT8:
			kernel.function << assign(temp_variable, temp_variable * constant(normalization->ui8));
			break;
		case VX_TYPE_INT32:
			kernel.function << assign(temp_variable, temp_variable * constant(normalization->ui32));
			break;
		case VX_TYPE_FLOAT32:
			kernel.function << assign(temp_variable, temp_variable * constant(normalization->f32));
			break;
		default:
			throw std::runtime_error("void LinearMask<T>::build():\n\tNot supported Scalar type");
		}
	}

	kernel.function << assign(target_pixel(out_node), temp_variable);
 }



template <typename T>
std::vector<Object *> SimplePointScalar<T>::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}
template <typename T>
std::vector<Object *> SimplePointScalar<T>::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}
template <typename T>
std::string SimplePointScalar<T>::generateClassDefinition()
{
    return generator::node_generator(this, generator::Type::Definition);
}
template <typename T>
std::string SimplePointScalar<T>::generateNodeCall()
{
    return generator::node_generator(this, generator::Type::Call);
}

template <typename T>
ConditionalAssignmentNode<T>::ConditionalAssignmentNode()
{
    node_name = "Conditional Assignment";
}
template <typename T>
std::vector<Object *> ConditionalAssignmentNode<T>::get_inputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(in);
    return used_objects;
}
template <typename T>
std::vector<Object *> ConditionalAssignmentNode<T>::get_outputs()
{
    std::vector<Object*> used_objects;
    used_objects.emplace_back(out);
    return used_objects;
}
template <typename T>
std::string ConditionalAssignmentNode<T>::generateClassDefinition()
{
    return generator::node_generator(this, generator::Type::Definition);
}
template <typename T>
std::string ConditionalAssignmentNode<T>::generateNodeCall()
{
    return generator::node_generator(this, generator::Type::Call);
}




}

#endif
