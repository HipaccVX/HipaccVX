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

#include "hipaVX_hipacc_generator.hpp"
#include "hipaVX_internal.hpp"

vx_node vxFWriteImageNode(vx_graph graph, vx_image image, std::string file)
{
	HipaVX::WriteImageNode *win = new HipaVX::WriteImageNode();
	win->src = image;
	win->dst_file = file;
	graph->graph.emplace_back(win);
	graph->built = false;
	return win;
}

VX_API_ENTRY vx_status VX_API_CALL vxSetNodeAttribute (vx_node node, vx_enum attribute, void *ptr, vx_size size)
{
	if (attribute == VX_NODE_BORDER)
	{
		node->border_mode = *((vx_border_e*) ptr);
		return VX_SUCCESS;
	}
	return VX_ERROR_NOT_IMPLEMENTED;
}

VX_API_ENTRY vx_node VX_API_CALL vxGaussian3x3Node(vx_graph graph, vx_image input, vx_image output)
{
	return nullptr;
}


VX_API_ENTRY vx_node VX_API_CALL vxSobel3x3Node(vx_graph graph, vx_image input, vx_image output_x, vx_image output_y)
{
	if (input->col != VX_DF_IMAGE_U8)
		return nullptr;
	if (output_x->col != VX_DF_IMAGE_S16 || output_y->col != VX_DF_IMAGE_S16)
		return nullptr;

	HipaVX::Sobel3x3Node *sobel = new HipaVX::Sobel3x3Node();
	sobel->src = input;
	sobel->dst_x = output_x;
	sobel->dst_y = output_y;
	graph->graph.emplace_back(sobel);
	graph->built = false;
	return sobel;
}

VX_API_ENTRY vx_node VX_API_CALL vxConvertDepthNode(vx_graph graph, vx_image input, vx_image output, vx_enum policy, vx_scalar shift)
{
	auto node = new HipaVX::ConvertDepthNode();
	node->src = input;
	node->dst = output;
	node->policy = policy;
	node->shift = shift;
	graph->graph.emplace_back(node);
	graph->built = false;
	return node;
}

VX_API_ENTRY vx_node VX_API_CALL vxMagnitudeNode(vx_graph graph, vx_image grad_x, vx_image grad_y, vx_image mag)
{
	auto node = new HipaVX::MagnitudeNode();
	node->grad_x = grad_x;
	node->grad_y = grad_y;
	node->mag = mag;
	graph->graph.emplace_back(node);
	graph->built = false;
	return node;
}

VX_API_ENTRY vx_node VX_API_CALL vxAndNode(vx_graph graph, vx_image in1, vx_image in2, vx_image out)
{
	auto node = new HipaVX::AndNode();
	node->in_1 = in1;
	node->in_2 = in2;
	node->out = out;
	graph->graph.emplace_back(node);
	graph->built = false;
	return node;
}
VX_API_ENTRY vx_node VX_API_CALL vxOrNode(vx_graph graph, vx_image in1, vx_image in2, vx_image out)
{
	auto node = new HipaVX::OrNode();
	node->in_1 = in1;
	node->in_2 = in2;
	node->out = out;
	graph->graph.emplace_back(node);
	graph->built = false;
	return node;
}
VX_API_ENTRY vx_node VX_API_CALL vxXorNode(vx_graph graph, vx_image in1, vx_image in2, vx_image out)
{
	auto node = new HipaVX::XorNode();
	node->in_1 = in1;
	node->in_2 = in2;
	node->out = out;
	graph->graph.emplace_back(node);
	graph->built = false;
	return node;
}
VX_API_ENTRY vx_node VX_API_CALL vxNotNode(vx_graph graph, vx_image input, vx_image output)
{
	auto node = new HipaVX::NotNode();
	node->in = input;
	node->out = output;
	graph->graph.emplace_back(node);
	graph->built = false;
	return node;
}





VX_API_ENTRY vx_status VX_API_CALL vxReleaseNode(vx_node *node)
{
	return 0;
}



VX_API_ENTRY vx_status VX_API_CALL vxGetStatus(vx_reference reference)
{
	return VX_SUCCESS;
}




vx_image vxCreateImageFromFile(vx_context context, vx_uint32 width, vx_uint32 height, vx_df_image color, std::string filename)
{
	HipaVX::FileinputImage *image;
	image = new HipaVX::FileinputImage(width, height, color, filename);
	context->images.emplace_back(image);
	return image;
}

VX_API_ENTRY vx_image vxCreateImage(vx_context context, vx_uint32 width, vx_uint32 height, vx_df_image color)
{
	HipaVX::Image *image;
	image = new HipaVX::Image(width, height, color);
	context->images.emplace_back(image);
	return image;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseImage(vx_image *image)
{
	return 0;
}



VX_API_ENTRY vx_context VX_API_CALL vxCreateContext(void)
{
	return new HipaVX::Context();
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseContext(vx_context *context)
{
	return 0;
}


VX_API_ENTRY vx_graph VX_API_CALL vxCreateGraph(vx_context context)
{
	auto graph = new HipaVX::Graph();
	context->graphs.emplace_back(graph);
	return graph;
}

VX_API_ENTRY vx_status VX_API_CALL vxVerifyGraph(vx_graph graph)
{
	graph->build();
	return VX_SUCCESS;
}

VX_API_ENTRY vx_status VX_API_CALL vxProcessGraph(vx_graph graph)
{
	// 1. Get the base main function
	//First, declare the images

	process_graph(graph);
	return 0;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseGraph(vx_graph *graph)
{
	return 0;
}

VX_API_ENTRY vx_scalar VX_API_CALL vxCreateScalar(vx_context context, vx_enum data_type, const void *ptr)
{
	if (data_type <= VX_TYPE_INVALID || data_type >= VX_TYPE_SCALAR)
		return nullptr;
	return new HipaVX::Scalar((vx_type_e) data_type, ptr);
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseScalar(vx_scalar *scalar)
{
	return 0;
}



