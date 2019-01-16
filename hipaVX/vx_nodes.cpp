#include "../VX/vx.h"
#include "hipaVX_kernels.hpp"

/*
 * 63 functions (24, 39)
 *
** VX_API_ENTRY vx_node VX_API_CALL vxColorConvertNode
 * VX_API_ENTRY vx_node VX_API_CALL vxChannelExtractNode
 * VX_API_ENTRY vx_node VX_API_CALL vxChannelCombineNode
 * VX_API_ENTRY vx_node VX_API_CALL vxPhaseNode
 * VX_API_ENTRY vx_node VX_API_CALL vxSobel3x3Node
 * VX_API_ENTRY vx_node VX_API_CALL vxMagnitudeNode
** VX_API_ENTRY vx_node VX_API_CALL vxScaleImageNode
** VX_API_ENTRY vx_node VX_API_CALL vxTableLookupNode
** VX_API_ENTRY vx_node VX_API_CALL vxHistogramNode
** VX_API_ENTRY vx_node VX_API_CALL vxEqualizeHistNode
 * VX_API_ENTRY vx_node VX_API_CALL vxAbsDiffNode
** VX_API_ENTRY vx_node VX_API_CALL vxMeanStdDevNode
 * VX_API_ENTRY vx_node VX_API_CALL vxThresholdNode
** VX_API_ENTRY vx_node VX_API_CALL vxNonMaxSuppressionNode
** VX_API_ENTRY vx_node VX_API_CALL vxIntegralImageNode
 * VX_API_ENTRY vx_node VX_API_CALL vxErode3x3Node
 * VX_API_ENTRY vx_node VX_API_CALL vxDilate3x3Node
** VX_API_ENTRY vx_node VX_API_CALL vxMedian3x3Node
 * VX_API_ENTRY vx_node VX_API_CALL vxBox3x3Node
 * VX_API_ENTRY vx_node VX_API_CALL vxGaussian3x3Node
** VX_API_ENTRY vx_node VX_API_CALL vxNonLinearFilterNode
 * VX_API_ENTRY vx_node VX_API_CALL vxConvolveNode
** VX_API_ENTRY vx_node VX_API_CALL vxGaussianPyramidNode
** VX_API_ENTRY vx_node VX_API_CALL vxLaplacianPyramidNode
** VX_API_ENTRY vx_node VX_API_CALL vxLaplacianReconstructNode
 * VX_API_ENTRY vx_node VX_API_CALL vxAccumulateImageNode
 * VX_API_ENTRY vx_node VX_API_CALL vxAccumulateWeightedImageNode
 * VX_API_ENTRY vx_node VX_API_CALL vxAccumulateSquareImageNode
** VX_API_ENTRY vx_node VX_API_CALL vxMinMaxLocNode
** VX_API_ENTRY vx_node VX_API_CALL vxMinNode
** VX_API_ENTRY vx_node VX_API_CALL vxMaxNode
 * VX_API_ENTRY vx_node VX_API_CALL vxAndNode
 * VX_API_ENTRY vx_node VX_API_CALL vxOrNode
 * VX_API_ENTRY vx_node VX_API_CALL vxXorNode
 * VX_API_ENTRY vx_node VX_API_CALL vxNotNode
** VX_API_ENTRY vx_node VX_API_CALL vxScalarOperationNode
** VX_API_ENTRY vx_node VX_API_CALL vxSelectNode
 * VX_API_ENTRY vx_node VX_API_CALL vxMultiplyNode
 * VX_API_ENTRY vx_node VX_API_CALL vxAddNode
 * VX_API_ENTRY vx_node VX_API_CALL vxSubtractNode
 * VX_API_ENTRY vx_node VX_API_CALL vxConvertDepthNode
** VX_API_ENTRY vx_node VX_API_CALL vxCannyEdgeDetectorNode
** VX_API_ENTRY vx_node VX_API_CALL vxWarpAffineNode
** VX_API_ENTRY vx_node VX_API_CALL vxWarpPerspectiveNode
 * VX_API_ENTRY vx_node VX_API_CALL vxHarrisCornersNode
** VX_API_ENTRY vx_node VX_API_CALL vxFastCornersNode
** VX_API_ENTRY vx_node VX_API_CALL vxOpticalFlowPyrLKNode
** VX_API_ENTRY vx_node VX_API_CALL vxRemapNode
** VX_API_ENTRY vx_node VX_API_CALL vxHalfScaleGaussianNode
** VX_API_ENTRY vx_node VX_API_CALL vxMatchTemplateNode
** VX_API_ENTRY vx_node VX_API_CALL vxLBPNode
** VX_API_ENTRY vx_node VX_API_CALL vxHOGCellsNode
** VX_API_ENTRY vx_node VX_API_CALL vxHOGFeaturesNode
** VX_API_ENTRY vx_node VX_API_CALL vxHoughLinesPNode
** VX_API_ENTRY vx_node VX_API_CALL vxBilateralFilterNode
** VX_API_ENTRY vx_node VX_API_CALL vxTensorMultiplyNode
** VX_API_ENTRY vx_node VX_API_CALL vxTensorAddNode
** VX_API_ENTRY vx_node VX_API_CALL vxTensorSubtractNode
** VX_API_ENTRY vx_node VX_API_CALL vxTensorTableLookupNode
** VX_API_ENTRY vx_node VX_API_CALL vxTensorTransposeNode
** VX_API_ENTRY vx_node VX_API_CALL vxTensorConvertDepthNode
** VX_API_ENTRY vx_node VX_API_CALL vxTensorMatrixMultiplyNode
** VX_API_ENTRY vx_node VX_API_CALL vxCopyNode
*/

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
    graph->built = false;*/

    HipaVX::Sobel3x3Node *sobel = new HipaVX::Sobel3x3Node();
    sobel->in = input;
    sobel->out_x = output_x;
    sobel->out_y = output_y;
    graph->graph.emplace_back(sobel);
    graph->built = false;

    return sobel;
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

// TODO: change to Bilateral
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
