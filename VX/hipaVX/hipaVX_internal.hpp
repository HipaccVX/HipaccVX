#pragma once

#include "../vx.h"

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


namespace HipaVX
{

class Scalar
{
public:
	Scalar(vx_type_e t, const void *ptr);

	vx_type_e type;
	union
	{
		vx_char c;
		vx_int8 i8;
		vx_uint8 ui8;
		vx_int16 i16;
		vx_uint16 ui16;
		vx_int32 i32;
		vx_uint32 ui32;
		vx_int64 i64;
		vx_uint64 ui64;
		vx_float32 f32;
		vx_float64 f64;
		vx_enum e;
		vx_size s;
		vx_df_image df_image;
		vx_bool b;
	};

};


class Image
{
	static int next_id;
public:
	const int my_id;
	Image(vx_uint32 width, vx_uint32 height, vx_df_image color);
	virtual ~Image() = default;
	vx_uint32 w, h;
	vx_df_image col;
};

class FileinputImage: public Image
{
public:
	FileinputImage(vx_uint32 width, vx_uint32 height, vx_df_image color, std::string filename);
	virtual ~FileinputImage() = default;
	std::string file;
};

class Array: public Image
{
public:
	Array(vx_enum item_type, vx_size cap, vx_size rows);
	virtual ~Array() = default;
	vx_enum type;
	vx_size capacity;
};


class Node
{
	static int next_id;
public:
	const int my_id;
	Node();
	virtual ~Node() = default;

	vx_border_e border_mode = VX_BORDER_UNDEFINED;

	virtual std::vector<Image*> get_used_images() = 0;
	virtual std::string generateClassDefinition() = 0;
	virtual std::string generateNodeCall() = 0;
	virtual void build(){}
};

class Graph
{
public:
	std::vector<Node*> graph;
	bool built = false;

	std::vector<Image*> used_images;
	void build();
};

class Context
{
public:

	std::vector<Image*> images;
	std::vector<Graph*> graphs;
};


class WriteImageNode: public Node
{
public:
	virtual ~WriteImageNode() override = default;
	Image *in;
	std::string out_file;
	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};

class ConvertDepthNode: public Node
{
public:
	virtual ~ConvertDepthNode() override = default;
	Image *in;
	Image *out;

	vx_enum policy;
	vx_scalar shift;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};

class NotNode: public Node
{
public:
	virtual ~NotNode() override = default;
	Image *in;
	Image *out;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};


class Dilate: public Node
{
public:
	virtual ~Dilate() override = default;

	Image *in;
	Image *out;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};
class Erode: public Node
{
public:
	virtual ~Erode() override = default;

	Image *in;
	Image *out;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};







// Experimental new stuff

class SimplePoint: public Node
{
public:
	virtual ~SimplePoint() override = default;

	Image *in_1;
	Image *in_2;
	Image *out;
	std::string operation;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};
class SimplePointAdd: public SimplePoint
{
public:
	SimplePointAdd()
	{
		operation = "+";
	}
	virtual ~SimplePointAdd() override = default;
};
class SimplePointSub: public SimplePoint
{
public:
	SimplePointSub()
	{
		operation = "-";
	}
	virtual ~SimplePointSub() override = default;
};
class SimplePointMul: public SimplePoint
{
public:
	SimplePointMul()
	{
		operation = "*";
	}
	virtual ~SimplePointMul() override = default;
};
class SimplePointDiv: public SimplePoint
{
public:
	SimplePointDiv()
	{
		operation = "/";
	}
	virtual ~SimplePointDiv() override = default;
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

	virtual std::vector<Image*> get_used_images() override;
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
	}
	virtual ~SimplePointScalarDiv() override = default;
};

class SquareNode: public Node
{
public:
	virtual ~SquareNode() override = default;
	Image *in;
	Image *out;

	SimplePointMul mul_node;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class AndNode: public SimplePoint
{
public:
	AndNode()
	{
		operation = "&";
	}
	virtual ~AndNode() override = default;
};
class XorNode: public SimplePoint
{
public:
	XorNode()
	{
		operation = "^";
	}
	virtual ~XorNode() override = default;
};
class OrNode: public SimplePoint
{
public:
	OrNode()
	{
		operation = "|";
	}
	virtual ~OrNode() override = default;
};

class SaturateNode: public Node
{
public:
	virtual ~SaturateNode() override = default;
	Image *in;
	Image *out;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};

template <typename T>
class LinearMask: public Node
{
public:
	virtual ~LinearMask() override = default;
	std::vector<T> mask;
	int dim[2]; //dim[0] = x, dim[1] = y
	float normalization;

	Image *in;
	Image *out;

	bool use_image_datatype_for_sum = true;
	vx_df_image sum_datatype;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};

class Sobel3x3Node: public Node
{
public:
	virtual ~Sobel3x3Node() override = default;

	LinearMask<int> sobel_x;
	LinearMask<int> sobel_y;


	Image *in;
	Image *out_x;
	Image *out_y;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class BoxFilter: public LinearMask<int>
{
public:
	BoxFilter()
	{
		this->mask = {1, 1, 1, 1, 1, 1, 1, 1, 1};
		this->dim[0] = this->dim[1] = 3;
		this->normalization = 1.0f / 9;
		this->use_image_datatype_for_sum = false;
		this->sum_datatype = VX_DF_IMAGE_S16;
	}
	virtual ~BoxFilter() override = default;
};
class GaussianFilter: public LinearMask<int>
{
public:
	GaussianFilter()
	{
		this->mask = {1, 2, 1, 2, 4, 2, 1, 2, 1};
		this->dim[0] = this->dim[1] = 3;
		this->normalization = 1.0f / 16;
		this->use_image_datatype_for_sum = false;
		this->sum_datatype = VX_DF_IMAGE_S16;
	}
	virtual ~GaussianFilter() override = default;
};


class Add3_3: public Node
{
public:
	virtual ~Add3_3() override = default;

	LinearMask<int> add;

	Image *in;
	Image *out;

	virtual std::vector<Image*> get_used_images() override;
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


	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class UnaryFunctionNode: public Node
{
public:
	virtual ~UnaryFunctionNode() override = default;
	Image *in;
	Image *out;

	std::string function;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};

class SqrtNode: public Node
{
public:
	virtual ~SqrtNode() override = default;
	Image *in;
	Image *out;

	UnaryFunctionNode function_node;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};
class AbsNode: public Node
{
public:
	virtual ~AbsNode() override = default;
	Image *in;
	Image *out;

	UnaryFunctionNode function_node;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};
class Atan2Node: public Node
{
public:
	virtual ~Atan2Node() override = default;
	Image *in;
	Image *out;

	UnaryFunctionNode function_node;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};


class AbsDiffNode: public Node
{
public:
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

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class AddNode: public Node
{
public:
	virtual ~AddNode() override = default;
	Image *in_1;
	Image *in_2;
	Image *out;

	SimplePointAdd add_node;
	std::unique_ptr<Image> add_image;

	SaturateNode saturate_node;

	vx_enum policy;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};
class SubtractNode: public Node
{
public:
	virtual ~SubtractNode() override = default;
	Image *in_1;
	Image *in_2;
	Image *out;

	SimplePointSub diff_node;
	std::unique_ptr<Image> diff_image;

	SaturateNode saturate_node;

	vx_enum policy;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};


class MagnitudeNode: public Node
{
public:
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

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class PhaseNode: public Node
{
public:
	virtual ~PhaseNode() override = default;
	Image *in_1;
	Image *in_2;
	Image *out;

	SimplePointDiv div_node;
	std::unique_ptr<Image> div_image;

	Atan2Node atan2_node;
	std::unique_ptr<Image> atan2_image;

	SimplePointScalarMul<float> mapping_node;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class VXMultiplyNode: public Node
{
public:
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


	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class VXAccumulateNode: public Node
{
public:
	virtual ~VXAccumulateNode() override = default;
	Image *in;
	Image *in_out;

	SimplePointAdd add_node;
	std::unique_ptr<Image> add_image;

	SaturateNode saturate_node;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class VXAccumulateSquareNode: public Node
{
public:
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

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
	virtual void build() override;
};

class VXAccumulateWeightedNode: public Node
{
public:
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


	virtual std::vector<Image*> get_used_images() override;
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

std::string node_generator(HipaVX::Dilate* n, Type t);
std::string node_generator(HipaVX::Erode* n, Type t);





template <typename T>
std::string node_generator(HipaVX::LinearMask<T>* n, Type t);
std::string node_generator(HipaVX::SimplePoint* n, Type t);
template <typename T>
std::string node_generator(HipaVX::SimplePointScalar<T>* n, Type t);
std::string node_generator(HipaVX::HarrisCorners* n, Type t);

std::string node_generator(HipaVX::SaturateNode* n, Type t);
std::string node_generator(HipaVX::UnaryFunctionNode* n, Type t);




}







namespace HipaVX
{
template <typename T>
std::vector<Image *> LinearMask<T>::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out);
	return used_images;
}
template <typename T>
std::string LinearMask<T>::generateClassDefinition()
{
	return generator::node_generator(this, generator::Type::Definition);
}
template <typename T>
std::string LinearMask<T>::generateNodeCall()
{
	return generator::node_generator(this, generator::Type::Call);
}


template <typename T>
std::vector<Image *> SimplePointScalar<T>::get_used_images()
{
	std::vector<Image*> used_images;
	used_images.emplace_back(in);
	used_images.emplace_back(out);
	return used_images;
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
}





