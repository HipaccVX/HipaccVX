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
	Image *src;
	std::string dst_file;
	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};

class Sobel3x3Node: public Node
{
public:
	virtual ~Sobel3x3Node() override = default;
	Image *src;
	Image *dst_x;
	Image *dst_y;

	virtual std::vector<Image*> get_used_images() override;
	virtual std::string generateClassDefinition() override;
	virtual std::string generateNodeCall() override;
};

class ConvertDepthNode: public Node
{
public:
	virtual ~ConvertDepthNode() override = default;
	Image *src;
	Image *dst;

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
}
