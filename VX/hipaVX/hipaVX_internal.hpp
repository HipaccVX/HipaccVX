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

class Sobel3x3Node: public Node
{
public:
	virtual ~Sobel3x3Node() override = default;
	Image *in;
	Image *out_x;
	Image *out_y;

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

class MagnitudeNode: public Node
{
public:
	virtual ~MagnitudeNode() override = default;
	Image *grad_x;
	Image *grad_y;
	Image *mag;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};


class AbsDiffNode: public Node
{
public:
	virtual ~AbsDiffNode() override = default;
	Image *in_1;
	Image *in_2;
	Image *out;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};
class AddNode: public Node
{
public:
	virtual ~AddNode() override = default;
	Image *in_1;
	Image *in_2;
	Image *out;

	vx_enum policy;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};
class SubtractNode: public Node
{
public:
	virtual ~SubtractNode() override = default;
	Image *in_1;
	Image *in_2;
	Image *out;

	vx_enum policy;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};

class AndNode: public Node
{
public:
	virtual ~AndNode() override = default;
	Image *in_1;
	Image *in_2;
	Image *out;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};
class XorNode: public Node
{
public:
	virtual ~XorNode() override = default;
	Image *in_1;
	Image *in_2;
	Image *out;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};
class OrNode: public Node
{
public:
	virtual ~OrNode() override = default;
	Image *in_1;
	Image *in_2;
	Image *out;

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

class BoxFilter: public Node
{
public:
	virtual ~BoxFilter() override = default;

	Image *in;
	Image *out;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};
class GaussianFilter: public Node
{
public:
	virtual ~GaussianFilter() override = default;

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

class HarrisCorners: public Node
{
public:
	HarrisCorners(Image *in)
		:in(in),
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
		  Mc(in->w, in->h, VX_TYPE_FLOAT32),
		  Vc(in->w, in->h, VX_TYPE_FLOAT32)
	{
	}
	virtual ~HarrisCorners() override = default;

	Image *in;
	Scalar *strength_thresh;
	Scalar *min_distance;
	Scalar *sensitivity;
	vx_int32 gradient_size;
	vx_int32 block_size;
	Array *corners;
	Scalar *num_corners;

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


	Image Mc;
	Image Vc;


	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
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
std::string node_generator(HipaVX::Sobel3x3Node* n, Type t);
std::string node_generator(HipaVX::ConvertDepthNode* n, Type t);
std::string node_generator(HipaVX::MagnitudeNode* n, Type t);

std::string node_generator(HipaVX::AbsDiffNode* n, Type t);
std::string node_generator(HipaVX::AddNode* n, Type t);
std::string node_generator(HipaVX::SubtractNode* n, Type t);

std::string node_generator(HipaVX::AndNode* n, Type t);
std::string node_generator(HipaVX::XorNode* n, Type t);
std::string node_generator(HipaVX::OrNode* n, Type t);
std::string node_generator(HipaVX::NotNode* n, Type t);

std::string node_generator(HipaVX::BoxFilter* n, Type t);
std::string node_generator(HipaVX::GaussianFilter* n, Type t);

std::string node_generator(HipaVX::Dilate* n, Type t);
std::string node_generator(HipaVX::Erode* n, Type t);

std::string node_generator(HipaVX::HarrisCorners* n, Type t);

}
