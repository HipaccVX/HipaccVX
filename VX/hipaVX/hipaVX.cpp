#include "../vx.h"
#include "../vx_compatibility.h"

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

vx_node vxFWriteImageNode(vx_graph graph, vx_image image, std::string file)
{
    HipaVX::WriteImageNode *win = new HipaVX::WriteImageNode();
    win->in = image;
    win->out_file = file;
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

VX_API_ENTRY vx_node VX_API_CALL vxSobel3x3Node(vx_graph graph, vx_image input, vx_image output_x, vx_image output_y)
{
    if (input->col != VX_DF_IMAGE_U8)
        return nullptr;
    if (output_x->col != VX_DF_IMAGE_S16 || output_y->col != VX_DF_IMAGE_S16)
        return nullptr;

    /*HipaVX::Sobel3x3Node *sobel = new HipaVX::Sobel3x3Node();
    sobel->in = input;
    sobel->out_x = output_x;
    sobel->out_y = output_y;
    graph->graph.emplace_back(sobel);
    graph->built = false;/**/


    HipaVX::Sobel3x3Node *sobel = new HipaVX::Sobel3x3Node();
    sobel->in = input;
    sobel->out_x = output_x;
    sobel->out_y = output_y;
    graph->graph.emplace_back(sobel);
    graph->built = false;/**/



    return sobel;
}

VX_API_ENTRY vx_node VX_API_CALL vxConvertDepthNode(vx_graph graph, vx_image input, vx_image output, vx_enum policy, vx_scalar shift)
{
    auto node = new HipaVX::ConvertDepthNode();
    node->in = input;
    node->out = output;
    node->policy = policy;
    node->shift = shift;
    graph->graph.emplace_back(node);
    graph->built = false;
    return node;
}

VX_API_ENTRY vx_node VX_API_CALL vxMagnitudeNode(vx_graph graph, vx_image grad_x, vx_image grad_y, vx_image mag)
{
    auto node = new HipaVX::MagnitudeNode();
    node->in_1 = grad_x;
    node->in_2 = grad_y;
    node->out = mag;
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

VX_API_ENTRY vx_node VX_API_CALL vxAbsDiffNode(vx_graph graph, vx_image in1, vx_image in2, vx_image out)
{
    if (out->col == VX_DF_IMAGE_U8 && (in1->col != VX_DF_IMAGE_U8 ||
                                       in2->col != VX_DF_IMAGE_U8))
        return nullptr;

    if (in1->col != VX_DF_IMAGE_U8 && in1->col != VX_DF_IMAGE_S16)
        return nullptr;
    if (in2->col != VX_DF_IMAGE_U8 && in2->col != VX_DF_IMAGE_S16)
        return nullptr;
    if (out->col != VX_DF_IMAGE_U8 && out->col != VX_DF_IMAGE_S16)
        return nullptr;

    auto node = new HipaVX::AbsDiffNode();
    node->in_1 = in1;
    node->in_2 = in2;
    node->out = out;
    graph->graph.emplace_back(node);
    graph->built = false;
    return node;
}
VX_API_ENTRY vx_node VX_API_CALL vxAddNode(vx_graph graph, vx_image in1, vx_image in2, vx_enum policy, vx_image out)
{
    if (out->col == VX_DF_IMAGE_U8 && (in1->col != VX_DF_IMAGE_U8 ||
                                       in2->col != VX_DF_IMAGE_U8))
        return nullptr;

    if (in1->col != VX_DF_IMAGE_U8 && in1->col != VX_DF_IMAGE_S16)
        return nullptr;
    if (in2->col != VX_DF_IMAGE_U8 && in2->col != VX_DF_IMAGE_S16)
        return nullptr;
    if (out->col != VX_DF_IMAGE_U8 && out->col != VX_DF_IMAGE_S16)
        return nullptr;

    auto node = new HipaVX::VXAddNode();
    node->in_1 = in1;
    node->in_2 = in2;
    node->policy = policy;
    node->out = out;
    graph->graph.emplace_back(node);
    graph->built = false;
    return node;
}
VX_API_ENTRY vx_node VX_API_CALL vxSubtractNode(vx_graph graph, vx_image in1, vx_image in2, vx_enum policy, vx_image out)
{
    if (out->col == VX_DF_IMAGE_U8 && (in1->col != VX_DF_IMAGE_U8 ||
                                       in2->col != VX_DF_IMAGE_U8))
        return nullptr;

    if (in1->col != VX_DF_IMAGE_U8 && in1->col != VX_DF_IMAGE_S16)
        return nullptr;
    if (in2->col != VX_DF_IMAGE_U8 && in2->col != VX_DF_IMAGE_S16)
        return nullptr;
    if (out->col != VX_DF_IMAGE_U8 && out->col != VX_DF_IMAGE_S16)
        return nullptr;

    auto node = new HipaVX::VXSubtractNode();
    node->in_1 = in1;
    node->in_2 = in2;
    node->policy = policy;
    node->out = out;
    graph->graph.emplace_back(node);
    graph->built = false;
    return node;
}

VX_API_ENTRY vx_node VX_API_CALL vxBox3x3Node(vx_graph graph, vx_image input, vx_image output)
{
    if (input->col != VX_DF_IMAGE_U8 || output->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::BoxFilter *box = new HipaVX::BoxFilter();
    box->in = input;
    box->out = output;
    graph->graph.emplace_back(box);
    graph->built = false;
    return box;
}
VX_API_ENTRY vx_node VX_API_CALL vxGaussian3x3Node(vx_graph graph, vx_image input, vx_image output)
{
    if (input->col != VX_DF_IMAGE_U8 || output->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::GaussianFilter *gaussian = new HipaVX::GaussianFilter();
    gaussian->in = input;
    gaussian->out = output;
    graph->graph.emplace_back(gaussian);
    graph->built = false;
    return gaussian;
}


VX_API_ENTRY vx_node VX_API_CALL vxDilate3x3Node(vx_graph graph, vx_image input, vx_image output)
{
    if (input->col != VX_DF_IMAGE_U8 || output->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::Dilate *dilate = new HipaVX::Dilate();
    dilate->in = input;
    dilate->out = output;
    graph->graph.emplace_back(dilate);
    graph->built = false;
    return dilate;
}
VX_API_ENTRY vx_node VX_API_CALL vxErode3x3Node(vx_graph graph, vx_image input, vx_image output)
{
    if (input->col != VX_DF_IMAGE_U8 || output->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::Erode *erode = new HipaVX::Erode();
    erode->in = input;
    erode->out = output;
    graph->graph.emplace_back(erode);
    graph->built = false;
    return erode;
}

VX_API_ENTRY vx_node VX_API_CALL vxHarrisCornersNode (vx_graph graph, vx_image input, vx_scalar strength_thresh, vx_scalar min_distance,
                                                      vx_scalar sensitivity, vx_int32 gradient_size, vx_int32 block_size,
                                                      vx_array corners, vx_scalar num_corners)
{
    if (input->col != VX_DF_IMAGE_U8)
        return nullptr;

    if (gradient_size != 3 && block_size != 3)
        return nullptr;

    HipaVX::HarrisCorners *harris = new HipaVX::HarrisCorners(input);
    harris->strength_thresh = strength_thresh;
    harris->min_distance = min_distance;
    harris->sensitivity = sensitivity;
    harris->gradient_size = gradient_size;
    harris->block_size = block_size;
    harris->corners = corners;
    harris->num_corners = num_corners;

    graph->graph.emplace_back(harris);
    graph->built = false;
    return harris;
}

VX_API_ENTRY vx_node VX_API_CALL vxScaleImageNode (vx_graph graph, vx_image src, vx_image dst, vx_enum type)
{
    if (src->col != VX_DF_IMAGE_U8 || dst->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::VXScaleNode *dilate = new HipaVX::VXScaleNode();
    dilate->in = src;
    dilate->out = dst;
    graph->graph.emplace_back(dilate);
    graph->built = false;
    return dilate;
}

VX_API_ENTRY vx_node VX_API_CALL vxPhaseNode (vx_graph graph, vx_image grad_x, vx_image grad_y, vx_image orientation)
{
    if (grad_x->col != VX_DF_IMAGE_S16 || grad_y->col != VX_DF_IMAGE_S16 || orientation->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::PhaseNode *phase = new HipaVX::PhaseNode();
    phase->in_1 = grad_x;
    phase->in_2 = grad_y;
    phase->out = orientation;
    graph->graph.emplace_back(phase);
    graph->built = false;
    return phase;
}

VX_API_ENTRY vx_node VX_API_CALL vxMultiplyNode (vx_graph graph, vx_image in1, vx_image in2, vx_scalar scale, vx_enum overflow_policy, vx_enum rounding_policy, vx_image out)
{
    if (out->col == VX_DF_IMAGE_U8 && (in1->col != VX_DF_IMAGE_U8 ||
                                       in2->col != VX_DF_IMAGE_U8))
        return nullptr;

    if (in1->col != VX_DF_IMAGE_U8 && in1->col != VX_DF_IMAGE_S16)
        return nullptr;
    if (in2->col != VX_DF_IMAGE_U8 && in2->col != VX_DF_IMAGE_S16)
        return nullptr;
    if (out->col != VX_DF_IMAGE_U8 && out->col != VX_DF_IMAGE_S16)
        return nullptr;

    if (rounding_policy == VX_ROUND_POLICY_TO_NEAREST_EVEN)
        return nullptr; //Only truncation is currently supported

    HipaVX::VXMultiplyNode *multiply = new HipaVX::VXMultiplyNode();
    multiply->in_1 = in1;
    multiply->in_2 = in2;
    multiply->out = out;
    multiply->scalar = scale;
    multiply->overflow_policy = overflow_policy;
    multiply->rounding_policy = rounding_policy;
    graph->graph.emplace_back(multiply);
    graph->built = false;
    return multiply;
}

VX_API_ENTRY vx_node VX_API_CALL vxAccumulateImageNode (vx_graph graph, vx_image input, vx_image accum)
{
    if (input->col != VX_DF_IMAGE_U8 || accum->col != VX_DF_IMAGE_S16)
        return nullptr;

    HipaVX::VXAccumulateNode *accum_node = new HipaVX::VXAccumulateNode();
    accum_node->in = input;
    accum_node->in_out = accum;
    graph->graph.emplace_back(accum_node);
    graph->built = false;
    return accum_node;
}

VX_API_ENTRY vx_node VX_API_CALL vxAccumulateSquareImageNode (vx_graph graph, vx_image input, vx_scalar shift, vx_image accum)
{
    if (input->col != VX_DF_IMAGE_U8 || accum->col != VX_DF_IMAGE_S16)
        return nullptr;

    if (shift->ui32 > 15)
        return nullptr;

    HipaVX::VXAccumulateSquareNode *accum_square_node = new HipaVX::VXAccumulateSquareNode();
    accum_square_node->in = input;
    accum_square_node->in_out = accum;
    accum_square_node->shift = shift;
    graph->graph.emplace_back(accum_square_node);
    graph->built = false;
    return accum_square_node;
}

VX_API_ENTRY vx_node VX_API_CALL vxAccumulateWeightedImageNode (vx_graph graph, vx_image input, vx_scalar alpha, vx_image accum)
{
    if (input->col != VX_DF_IMAGE_U8 || accum->col != VX_DF_IMAGE_U8)
        return nullptr;

    if (alpha->f32 < 0 || alpha->f32 > 1)
        return nullptr;

    HipaVX::VXAccumulateWeightedNode *accum_weighted_node = new HipaVX::VXAccumulateWeightedNode();
    accum_weighted_node->in = input;
    accum_weighted_node->in_out = accum;
    accum_weighted_node->alpha = alpha;
    graph->graph.emplace_back(accum_weighted_node);
    graph->built = false;
    return accum_weighted_node;
}

VX_API_ENTRY vx_node VX_API_CALL vxThresholdNode (vx_graph graph, vx_image input, vx_threshold thresh, vx_image output)
{
    if (input->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::VXThresholdNode *thresh_node = new HipaVX::VXThresholdNode();
    thresh_node->in = input;
    thresh_node->threshold = thresh;
    thresh_node->out = output;
    graph->graph.emplace_back(thresh_node);
    graph->built = false;
    return thresh_node;
}

VX_API_ENTRY vx_node VX_API_CALL vxChannelExtractNode (vx_graph graph, vx_image input, vx_enum channel, vx_image output)
{
    if (output->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::VXChannelExtractNode *channel_extract = new HipaVX::VXChannelExtractNode();
    channel_extract->in = input;
    channel_extract->channel = (vx_channel_e) channel;
    channel_extract->out = output;
    graph->graph.emplace_back(channel_extract);
    graph->built = false;
    return channel_extract;
}

VX_API_ENTRY vx_node VX_API_CALL vxChannelCombineNode (vx_graph graph, vx_image plane0, vx_image plane1, vx_image plane2, vx_image plane3, vx_image output)
{
    if (plane2 == nullptr || plane3 == nullptr)
        throw std::runtime_error("vxChannelCombineNode: 4 planes are needed for this implementation");
    if (output->col != VX_DF_IMAGE_RGBX || plane0->col != VX_DF_IMAGE_U8 || plane1->col != VX_DF_IMAGE_U8 || plane2->col != VX_DF_IMAGE_U8 || plane3->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::VXChannelCombineNode *channel_combine = new HipaVX::VXChannelCombineNode();
    channel_combine->in_1 = plane0;
    channel_combine->in_2 = plane1;
    channel_combine->in_3 = plane2;
    channel_combine->in_4 = plane3;
    channel_combine->out = output;
    graph->graph.emplace_back(channel_combine);
    graph->built = false;
    return channel_combine;
}

VX_API_ENTRY vx_node VX_API_CALL vxAnotherBilateralFilterNode(vx_graph graph, vx_image input, vx_int32 sigma_r, vx_image output)
{
    if (input->col != VX_DF_IMAGE_U8 || output->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::AnotherBilateralFilterNode *bilateral_filter = new HipaVX::AnotherBilateralFilterNode();
    bilateral_filter->in = input;
    bilateral_filter->out = output;
    bilateral_filter->sigma_r = sigma_r;
    graph->graph.emplace_back(bilateral_filter);
    graph->built = false;
    return bilateral_filter;
}


VX_API_ENTRY vx_node VX_API_CALL vxConvolveNode (vx_graph graph, vx_image input, vx_convolution conv, vx_image output)
{
    if (input->col != VX_DF_IMAGE_U8)
        return nullptr;

    if (output->col != VX_DF_IMAGE_U8 && output->col != VX_DF_IMAGE_S16)
        return nullptr;

    HipaVX::VXConvolveNode *convolve_node = new HipaVX::VXConvolveNode();
    convolve_node->in = input;
    convolve_node->convolution = conv;
    convolve_node->out = output;
    graph->graph.emplace_back(convolve_node);
    graph->built = false;
    return convolve_node;
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

vx_image vxCreateImageFromFile(vx_context context, vx_uint32 width, vx_uint32 height, vx_df_image color, std::string filename)
{
    HipaVX::FileinputImage *image;
    image = new HipaVX::FileinputImage(width, height, color, filename);
    context->images.emplace_back(image);
    return image;
}

vx_node vxHipaccNode(vx_graph graph, std::string filename, vx_reference *parameters, vx_size count, vx_image out)
{
    HipaVX::HipaccNode *hipaccNode = new HipaVX::HipaccNode();

    hipaccNode->filename = filename;
    hipaccNode->out = out;
    for(vx_size i = 0; i < count; i++)
        hipaccNode->parameters.push_back(parameters[i]);

    graph->graph.emplace_back(hipaccNode);
    graph->built = false;

    return hipaccNode;
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



