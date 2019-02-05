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

namespace HipaVX
{

// TODO: write thiii
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

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
	virtual void build() override;
};

class Sobel3x3Node: public Node
{
public:
    Sobel3x3Node();
    virtual ~Sobel3x3Node() override = default;

    LinearMask<int> sobel_x;
    LinearMask<int> sobel_y;

    Image *in;
    Image *out_x;
    Image *out_y;

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
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
    virtual void build() override;
};

class HarrisCorners: public Node
{
public:
    HarrisCorners(Image *in);
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
    virtual void build() override;
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
    virtual void build() override;
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

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
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

    virtual std::vector<Object*> get_inputs() override;
    virtual std::vector<Object*> get_outputs() override;
    virtual void build() override;
};

}

namespace generator
{
// TODO: remove all except HipaccNode, WriteImageNode
std::string node_generator(HipaVX::WriteImageNode* n, Type t);
std::string node_generator(HipaVX::HarrisCorners* n, Type t);

std::string node_generator(HipaVX::HipaccNode *n, Type t);
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

}
