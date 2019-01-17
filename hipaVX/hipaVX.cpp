#include "../VX/vx.h"
#include "../VX/vx_compatibility.h"

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
#include "hipaVX_kernels.hpp"

VX_API_ENTRY vx_status VX_API_CALL vxSetNodeAttribute (vx_node node, vx_enum attribute, void *ptr, vx_size size)
{
    if (attribute == VX_NODE_BORDER)
    {
        node->border_mode = *((vx_border_e*) ptr);
        return VX_SUCCESS;
    }
    return VX_ERROR_NOT_IMPLEMENTED;
}


VX_API_ENTRY vx_status VX_API_CALL vxReleaseNode(vx_node *node)
{
    return 0;
}


VX_API_ENTRY vx_status VX_API_CALL vxGetStatus(vx_reference reference)
{
    return VX_SUCCESS;
}

VX_API_ENTRY vx_threshold VX_API_CALL vxCreateThresholdForImage (vx_context context,vx_enum thresh_type, vx_df_image input_format, vx_df_image output_format)
{
    HipaVX::Threshold *t = new HipaVX::Threshold;

    t->threshold_type = (vx_threshold_type_e) thresh_type;
    t->input_format = input_format;
    t->output_format = output_format;

    return t;
}
VX_API_ENTRY vx_status VX_API_CALL vxSetThresholdAttribute (vx_threshold thresh, vx_enum attribute, const void *ptr, vx_size size)
{
    vx_int32 *int32_ptr = (vx_int32 *)ptr;
    switch (attribute)
    {
    case VX_THRESHOLD_THRESHOLD_VALUE:
        thresh->value = *int32_ptr;
        break;
    case VX_THRESHOLD_THRESHOLD_LOWER:
        thresh->lower = *int32_ptr;
        break;
    case VX_THRESHOLD_THRESHOLD_UPPER:
        thresh->upper = *int32_ptr;
        break;
    case VX_THRESHOLD_TRUE_VALUE:
        thresh->true_value = *int32_ptr;
        break;
    case VX_THRESHOLD_FALSE_VALUE:
        thresh->false_value = *int32_ptr;
        break;
    default:
        return VX_FAILURE;
    }
    return VX_SUCCESS;
}




VX_API_ENTRY vx_convolution VX_API_CALL vxCreateConvolution (vx_context context, vx_size columns, vx_size rows)
{
    HipaVX::Convolution *conv = new HipaVX::Convolution();
    conv->columns = columns;
    conv->rows = rows;
    conv->coefficients.resize(conv->columns * conv->rows);
    return conv;
}
VX_API_ENTRY vx_status VX_API_CALL vxCopyConvolutionCoefficients (vx_convolution conv, void *user_ptr, vx_enum usage, vx_enum user_mem_type)
{
    if (usage != VX_WRITE_ONLY && user_mem_type != VX_MEMORY_TYPE_HOST)
        return VX_FAILURE;

    vx_int16 *int16_ptr = (vx_int16*) user_ptr;
    for(unsigned int i = 0; i < conv->coefficients.size(); i++)
    {
        conv->coefficients[i] = int16_ptr[i];
    }

    return VX_SUCCESS;
}
VX_API_ENTRY vx_status VX_API_CALL vxSetConvolutionAttribute (vx_convolution conv, vx_enum attribute, const void *ptr, vx_size size)
{
    if (attribute != VX_CONVOLUTION_SCALE)
        return VX_FAILURE;

    conv->scale = *((vx_uint32*) ptr);

    return VX_SUCCESS;
}

VX_API_ENTRY vx_matrix VX_API_CALL vxCreateMatrix (vx_context c, vx_enum data_type, vx_size columns, vx_size rows)
{
    if (data_type != VX_TYPE_UINT8 && data_type != VX_TYPE_INT32 && data_type != VX_TYPE_FLOAT32)
        return nullptr;


    HipaVX::VX_Matrix *mat = new HipaVX::VX_Matrix();

    mat->data_type = data_type;
    mat->columns = columns;
    mat->rows = rows;
    mat->mat.resize(sizeof(int32_t) * columns * rows); //Optimization available: Only resize to what you actually need (e.g. in uint8 case)

    return mat;
}
VX_API_ENTRY vx_status VX_API_CALL vxCopyMatrix (vx_matrix matrix, void *user_ptr, vx_enum usage, vx_enum user_mem_type)
{
    if (usage != VX_WRITE_ONLY)
        return VX_FAILURE;
    if (user_mem_type != VX_MEMORY_TYPE_HOST)
        return VX_FAILURE;

    switch(matrix->data_type)
    {
    case VX_TYPE_UINT8:
        std::memcpy(matrix->mat.data(), user_ptr, matrix->columns * matrix->rows);
        break;
    case VX_TYPE_INT32:
    case VX_TYPE_FLOAT32:
        std::memcpy(matrix->mat.data(), user_ptr, matrix->columns * matrix->rows * 4);
        break;
    }

    return VX_SUCCESS;
}

VX_API_ENTRY vx_image VX_API_CALL vxCreateImage(vx_context context, vx_uint32 width, vx_uint32 height, vx_df_image color)
{
    HipaVX::Image *image;
    image = new HipaVX::Image(width, height, color);
    context->images.emplace_back(image);
    return image;
}

VX_API_ENTRY vx_array VX_API_CALL vxCreateArray(vx_context context, vx_enum item_type, vx_size capacity)
{
    if (item_type != VX_TYPE_KEYPOINT)
        throw std::runtime_error("vx_array: Only VX_TYPE_KEYPOINT is currently supported");

    HipaVX::Array *arr = new HipaVX::Array(item_type, capacity, 7); // vx_keypoint_t has 7 32bit members
    context->images.emplace_back(arr);
    return arr;
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
