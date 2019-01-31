#pragma once

#include "../VX/vx_types.h"
#include "ast.hpp"

#include <vector>
#include <string>

function_ast::Datatype convert_type(vx_df_image type);

namespace HipaVX
{
class Object
{
	static int next_id;
public:
	const int my_id;
	Object();

	vx_type_e type;
};

class Scalar: public Object
{
public:
	Scalar(vx_type_e t, const void *ptr);

	vx_type_e data_type;
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

class Image: public Object
{
public:
	Image(vx_uint32 width, vx_uint32 height, vx_df_image color);
	virtual ~Image() = default;
	vx_uint32 w, h;
	vx_df_image col;
};

class Array: public Image
{
public:
	Array(vx_enum item_type, vx_size cap, vx_size rows);
	virtual ~Array() = default;
	vx_enum type;
	vx_size capacity;
};

class Node: public Object
{
protected:
	std::string node_name = "nodename not set";
public:
	Node()
	{
		type = VX_TYPE_NODE;
	}
	virtual ~Node() = default;
	std::string get_name() {return node_name;}

	vx_border_e border_mode = VX_BORDER_UNDEFINED;
    function_ast::ForEveryPixel kernel;

	virtual std::vector<Object*> get_inputs() = 0;
	virtual std::vector<Object*> get_outputs() = 0;
    virtual std::vector<Node*> get_subnodes(){return {};}
	virtual std::string generateClassDefinition() = 0;
	virtual std::string generateNodeCall() = 0;
	virtual void build(){}
};

class Graph: public Object
{
public:
	std::vector<Node*> graph;
	bool built = false;

	void build();
};

class VX_Matrix: public Object
{
public:
	VX_Matrix()
	{
		type = VX_TYPE_MATRIX;
	}

	vx_enum data_type;
	vx_size rows;
	vx_size columns;
	std::vector<u_char> mat;
};

class Threshold: public Object
{
public:
	Threshold()
	{
		type = VX_TYPE_THRESHOLD;
	}
	vx_threshold_type_e threshold_type;

	vx_int32 value;
	vx_int32 lower;
	vx_int32 upper;

	vx_int32 true_value = 255;
	vx_int32 false_value = 0;

	vx_df_image input_format;
	vx_df_image output_format;
};

class Context: public Object
{
public:

	std::vector<Image*> images;
	std::vector<Graph*> graphs;
};

class Convolution: public Object
{
public:
	Convolution()
	{
		type = VX_TYPE_CONVOLUTION;
	}
	std::vector<vx_int16> coefficients;
	vx_size  rows;
	vx_size  columns;
	vx_uint32 scale;
};
}


struct _vx_reference
{
	HipaVX::Object *o;
};

struct _vx_scalar
{
	HipaVX::Object *o;
};

struct _vx_kernel
{
	HipaVX::Object *o;
};

struct _vx_node
{
	HipaVX::Object *o;
};

struct _vx_graph
{
	HipaVX::Object *o;
};

struct _vx_context
{
	HipaVX::Object *o;
};

struct _vx_matrix
{
	HipaVX::Object *o;
};

struct _vx_threshold
{
	HipaVX::Object *o;
};

struct _vx_convolution
{
	HipaVX::Object *o;
};

struct _vx_array
{
	HipaVX::Object *o;
};

struct _vx_image
{
	HipaVX::Object *o;
};
