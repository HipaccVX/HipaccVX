#include "../VX/vx.h"
#include "kernels/domVX_kernels.hpp"

// High priority
// TODO: Nodes consisting of graphs should be described in here not in class definitions.
//       We can extend vxAPI so that composite graph functions can be written with function calls.

/*
 * 63 functions (25, 38)
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
 * VX_API_ENTRY vx_node VX_API_CALL vxCopyNode
*/

// TODO: maybe move convert to domVX_types.hpp ???
static HipaVX::Node* convert(vx_node n)
{
	return (HipaVX::Node*)(n->o);
}
static HipaVX::Graph* convert(vx_graph n)
{
	return (HipaVX::Graph*)(n->o);
}
static HipaVX::Image* convert(vx_image n)
{
	return (HipaVX::Image*)(n->o);
}
static HipaVX::Scalar* convert(vx_scalar n)
{
	return (HipaVX::Scalar*)(n->o);
}
static HipaVX::Array* convert(vx_array n)
{
	return (HipaVX::Array*)(n->o);
}
static HipaVX::Threshold* convert(vx_threshold n)
{
	return (HipaVX::Threshold*)(n->o);
}
static HipaVX::Convolution* convert(vx_convolution n)
{
	return (HipaVX::Convolution*)(n->o);
}

VX_API_ENTRY vx_node VX_API_CALL vxChannelExtractNode (vx_graph graph, vx_image input, vx_enum channel, vx_image output)
{
	if (convert(output)->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::VXChannelExtractNode *channel_extract = new HipaVX::VXChannelExtractNode();
	auto vx = new _vx_node();
	vx->o = channel_extract;
	channel_extract->in = convert(input);
    channel_extract->channel_vx = (vx_channel_e) channel;
	channel_extract->out = convert(output);
	convert(graph)->graph.emplace_back(channel_extract);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxChannelCombineNode (vx_graph graph, vx_image plane0, vx_image plane1, vx_image plane2, vx_image plane3, vx_image output)
{
    if (plane2 == nullptr || plane3 == nullptr)
        throw std::runtime_error("vxChannelCombineNode: 4 planes are needed for this implementation");
	if (convert(output)->col != VX_DF_IMAGE_RGBX || convert(plane0)->col != VX_DF_IMAGE_U8 || convert(plane1)->col != VX_DF_IMAGE_U8 || convert(plane2)->col != VX_DF_IMAGE_U8 || convert(plane3)->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::VXChannelCombineNode *channel_combine = new HipaVX::VXChannelCombineNode();
	auto vx = new _vx_node();
	vx->o = channel_combine;
	channel_combine->in_1 = convert(plane0);
	channel_combine->in_2 = convert(plane1);
	channel_combine->in_3 = convert(plane2);
	channel_combine->in_4 = convert(plane3);
	channel_combine->out = convert(output);
	convert(graph)->graph.emplace_back(channel_combine);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxPhaseNode (vx_graph graph, vx_image grad_x, vx_image grad_y, vx_image orientation)
{
	if (convert(grad_x)->col != VX_DF_IMAGE_S16 || convert(grad_y)->col != VX_DF_IMAGE_S16 || convert(orientation)->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::PhaseNode *phase = new HipaVX::PhaseNode();
	auto vx = new _vx_node();
	vx->o = phase;
	phase->in_1 = convert(grad_x);
	phase->in_2 = convert(grad_y);
	phase->out = convert(orientation);
	convert(graph)->graph.emplace_back(phase);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxCopyNode (vx_graph graph, vx_reference input, vx_reference output)
{
    if (input->o->type != VX_TYPE_IMAGE || output->o->type != VX_TYPE_IMAGE)
        throw std::runtime_error("Currently only supports Image");

    if (convert((vx_image)input)->col != convert((vx_image)output)->col)
        return nullptr;
    if (convert((vx_image)input)->h != convert((vx_image)output)->h)
        return nullptr;
    if (convert((vx_image)input)->w != convert((vx_image)output)->w)
        return nullptr;

    HipaVX::VXCopy *copy = new HipaVX::VXCopy();
    auto vx = new _vx_node();
    vx->o = copy;
    copy->in = convert((vx_image)input);
    copy->out = convert((vx_image)output);
    convert(graph)->graph.emplace_back(copy);
    convert(graph)->built = false;
    return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxSobel3x3Node(vx_graph graph, vx_image input, vx_image output_x, vx_image output_y)
{
	if (convert(input)->col != VX_DF_IMAGE_U8)
        return nullptr;
	if (convert(output_x)->col != VX_DF_IMAGE_S16 || convert(output_y)->col != VX_DF_IMAGE_S16)
        return nullptr;

    HipaVX::Sobel3x3Node *sobel = new HipaVX::Sobel3x3Node();
	auto vx = new _vx_node();
	vx->o = sobel;
	sobel->in = convert(input);
	sobel->out_x = convert(output_x);
	sobel->out_y = convert(output_y);
	convert(graph)->graph.emplace_back(sobel);
	convert(graph)->built = false;

	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxMagnitudeNode(vx_graph graph, vx_image grad_x, vx_image grad_y, vx_image mag)
{
    auto node = new HipaVX::CompositeNode("vxMagnitude");
    HipaVX::Image *x_im   = convert(grad_x);
    HipaVX::Image *y_im   = convert(grad_y);
    HipaVX::Image *mag_im = convert(mag);
    node->inp(x_im);
    node->inp(y_im);
    node->outp(mag_im);

    auto *xx_n = new HipaVX::SquareNode();
    auto *yy_n = new HipaVX::SquareNode();
    auto *add_n = new HipaVX::SimplePointAdd();
    auto *sqrt_n = new HipaVX::SqrtNode();
    auto *sat_n = new HipaVX::SaturateNode();

    node->img_list.emplace_back(new HipaVX::Image(x_im->w, x_im->h, VX_DF_IMAGE_S32));
    node->img_list.emplace_back(new HipaVX::Image(y_im->w, y_im->h, VX_DF_IMAGE_S32));
    node->img_list.emplace_back(new HipaVX::Image(y_im->w, y_im->h, VX_DF_IMAGE_S32));
    node->img_list.emplace_back(new HipaVX::Image(y_im->w, y_im->h, VX_DF_IMAGE_S32));
    auto   xx_im = node->img_list[0].get();
    auto   yy_im = node->img_list[1].get();
    auto  add_im = node->img_list[2].get();
    auto sqrt_im = node->img_list[3].get();

    xx_n->in = x_im;
    yy_n->in = y_im;
    xx_n->out = xx_im;
    yy_n->out = yy_im;
    add_n->in_1 = xx_im;
    add_n->in_2 = yy_im;
    add_n->out = add_im;
    sqrt_n->in = add_im;
    sqrt_n->out = sqrt_im;
    sat_n->in = sqrt_im;
    sat_n->out = mag_im;

  	node->subgraph.push_back(xx_n);
  	node->subgraph.push_back(yy_n);
  	node->subgraph.push_back(add_n);
  	node->subgraph.push_back(sqrt_n);
  	node->subgraph.push_back(sat_n);

    // return an OpenVX node
    auto vx = new _vx_node();
	vx->o = node;
	convert(graph)->graph.emplace_back(node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxMultiplyNode (vx_graph graph, vx_image in1, vx_image in2, vx_scalar scale, vx_enum overflow_policy, vx_enum rounding_policy, vx_image out)
{
	if (convert(out)->col == VX_DF_IMAGE_U8 && (convert(in1)->col != VX_DF_IMAGE_U8 ||
												convert(in2)->col != VX_DF_IMAGE_U8))
        return nullptr;

	if (convert(in1)->col != VX_DF_IMAGE_U8 && convert(in1)->col != VX_DF_IMAGE_S16)
        return nullptr;
	if (convert(in2)->col != VX_DF_IMAGE_U8 && convert(in2)->col != VX_DF_IMAGE_S16)
        return nullptr;
	if (convert(out)->col != VX_DF_IMAGE_U8 && convert(out)->col != VX_DF_IMAGE_S16)
        return nullptr;

    if (rounding_policy == VX_ROUND_POLICY_TO_NEAREST_EVEN)
        return nullptr; //Only truncation is currently supported

    auto node = new HipaVX::CompositeNode("vxMultiply");
    HipaVX::Image *in1_im   = convert(in1);
    HipaVX::Image *in2_im   = convert(in2);
    HipaVX::Scalar *scale_s = convert(scale);
    HipaVX::Image *out_im = convert(out);
    node->inp(in1_im);
    node->inp(in2_im);
    node->inp(scale_s);
    node->outp(out_im);

    auto *mul_n = new HipaVX::SimplePointMul();
    auto *muls_n =  new HipaVX::SimplePointScalarMul<float>();
    auto *sat_n = new HipaVX::SaturateNode();
  	node->subgraph.push_back(mul_n);
  	node->subgraph.push_back(muls_n);

    std::unique_ptr<HipaVX::Image> muls_im, mul_im;
    mul_im.reset(new HipaVX::Image(in1_im->w, in2_im->h, VX_DF_IMAGE_S32));

    mul_n->in_1 = in1_im;
    mul_n->in_2 = in2_im;
    mul_n->out  = mul_im.get();
    muls_n->in  = mul_im.get(); 
    muls_n->scalar = scale_s->f32; 
    if (overflow_policy != VX_CONVERT_POLICY_SATURATE) {
        muls_n->out = out_im;
    }
    else {
        muls_im.reset(new HipaVX::Image(in1_im->w, in2_im->h, VX_DF_IMAGE_S32));
  	    node->subgraph.push_back(sat_n);
        muls_n->out = muls_im.get();
        sat_n->in = muls_im.get();
        sat_n->out = out_im;
    }

    auto vx = new _vx_node();
	vx->o = node;
	convert(graph)->graph.emplace_back(node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxAbsDiffNode(vx_graph graph, vx_image in1, vx_image in2, vx_image out)
{
	if (convert(out)->col == VX_DF_IMAGE_U8 && (convert(in1)->col != VX_DF_IMAGE_U8 ||
										convert(in2)->col != VX_DF_IMAGE_U8))
        return nullptr;

	if (convert(in1)->col != VX_DF_IMAGE_U8 && convert(in1)->col != VX_DF_IMAGE_S16)
        return nullptr;
	if (convert(in2)->col != VX_DF_IMAGE_U8 && convert(in2)->col != VX_DF_IMAGE_S16)
        return nullptr;
	if (convert(out)->col != VX_DF_IMAGE_U8 && convert(out)->col != VX_DF_IMAGE_S16)
        return nullptr;

    auto node = new HipaVX::AbsDiffNode();
	auto vx = new _vx_node();
	vx->o = node;
	node->in_1 = convert(in1);
	node->in_2 = convert(in2);
	node->out = convert(out);
	convert(graph)->graph.emplace_back(node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxThresholdNode (vx_graph graph, vx_image input, vx_threshold thresh, vx_image output)
{
	if (convert(input)->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::VXThresholdNode *thresh_node = new HipaVX::VXThresholdNode();
	auto vx = new _vx_node();
	vx->o = thresh_node;
	thresh_node->in = convert(input);
	thresh_node->threshold = convert(thresh);
	thresh_node->out = convert(output);
	convert(graph)->graph.emplace_back(thresh_node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxErode3x3Node(vx_graph graph, vx_image input, vx_image output)
{
	if (convert(input)->col != VX_DF_IMAGE_U8 || convert(output)->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::Erode *erode = new HipaVX::Erode();
	auto vx = new _vx_node();
	vx->o = erode;
	erode->in = convert(input);
	erode->out = convert(output);
	convert(graph)->graph.emplace_back(erode);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxDilate3x3Node(vx_graph graph, vx_image input, vx_image output)
{
	if (convert(input)->col != VX_DF_IMAGE_U8 || convert(output)->col != VX_DF_IMAGE_U8)
        return nullptr;

	HipaVX::Dilate *dilate = new HipaVX::Dilate();
	auto vx = new _vx_node();
	vx->o = dilate;
	dilate->in = convert(input);
	dilate->out = convert(output);
	convert(graph)->graph.emplace_back(dilate);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxBox3x3Node(vx_graph graph, vx_image input, vx_image output)
{
	if (convert(input)->col != VX_DF_IMAGE_U8 || convert(output)->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::BoxFilter *box = new HipaVX::BoxFilter();
	auto vx = new _vx_node();
	vx->o = box;
	box->in = convert(input);
	box->out = convert(output);
	convert(graph)->graph.emplace_back(box);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxGaussian3x3Node(vx_graph graph, vx_image input, vx_image output)
{
	if (convert(input)->col != VX_DF_IMAGE_U8 || convert(output)->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::GaussianFilter *gaussian = new HipaVX::GaussianFilter();
	auto vx = new _vx_node();
	vx->o = gaussian;
	gaussian->in = convert(input);
	gaussian->out = convert(output);
	convert(graph)->graph.emplace_back(gaussian);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxConvolveNode (vx_graph graph, vx_image input, vx_convolution conv, vx_image output)
{
	if (convert(input)->col != VX_DF_IMAGE_U8)
        return nullptr;

	if (convert(output)->col != VX_DF_IMAGE_U8 && convert(output)->col != VX_DF_IMAGE_S16)
        return nullptr;

    HipaVX::VXConvolveNode *convolve_node = new HipaVX::VXConvolveNode();
	auto vx = new _vx_node();
	vx->o = convolve_node;
	convolve_node->in = convert(input);
	convolve_node->convolution = convert(conv);
	convolve_node->out = convert(output);
	convert(graph)->graph.emplace_back(convolve_node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxAccumulateImageNode (vx_graph graph, vx_image input, vx_image accum)
{
	if (convert(input)->col != VX_DF_IMAGE_U8 || convert(accum)->col != VX_DF_IMAGE_S16)
        return nullptr;

    HipaVX::VXAccumulateNode *accum_node = new HipaVX::VXAccumulateNode();
	auto vx = new _vx_node();
	vx->o = accum_node;
	accum_node->in = convert(input);
	accum_node->in_out = convert(accum);
	convert(graph)->graph.emplace_back(accum_node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxAccumulateWeightedImageNode (vx_graph graph, vx_image input, vx_scalar alpha, vx_image accum)
{
	if (convert(input)->col != VX_DF_IMAGE_U8 || convert(accum)->col != VX_DF_IMAGE_U8)
        return nullptr;

	if (convert(alpha)->f32 < 0 || convert(alpha)->f32 > 1)
        return nullptr;

    HipaVX::VXAccumulateWeightedNode *accum_weighted_node = new HipaVX::VXAccumulateWeightedNode();
	auto vx = new _vx_node();
	vx->o = accum_weighted_node;
	accum_weighted_node->in = convert(input);
	accum_weighted_node->in_out = convert(accum);
	accum_weighted_node->alpha = convert(alpha);
	convert(graph)->graph.emplace_back(accum_weighted_node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxAccumulateSquareImageNode (vx_graph graph, vx_image input, vx_scalar shift, vx_image accum)
{
	if (convert(input)->col != VX_DF_IMAGE_U8 || convert(accum)->col != VX_DF_IMAGE_S16)
        return nullptr;

	if (convert(shift)->ui32 > 15)
        return nullptr;

    HipaVX::VXAccumulateSquareNode *accum_square_node = new HipaVX::VXAccumulateSquareNode();
	auto vx = new _vx_node();
	vx->o = accum_square_node;
	accum_square_node->in = convert(input);
	accum_square_node->in_out = convert(accum);
	accum_square_node->shift = convert(shift);
	convert(graph)->graph.emplace_back(accum_square_node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxAndNode(vx_graph graph, vx_image in1, vx_image in2, vx_image out)
{
    auto node = new HipaVX::AndNode();
	auto vx = new _vx_node();
	vx->o = node;
	node->in_1 = convert(in1);
	node->in_2 = convert(in2);
	node->out = convert(out);
	convert(graph)->graph.emplace_back(node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxOrNode(vx_graph graph, vx_image in1, vx_image in2, vx_image out)
{
    auto node = new HipaVX::OrNode();
	auto vx = new _vx_node();
	vx->o = node;
	node->in_1 = convert(in1);
	node->in_2 = convert(in2);
	node->out = convert(out);
	convert(graph)->graph.emplace_back(node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxXorNode(vx_graph graph, vx_image in1, vx_image in2, vx_image out)
{
    auto node = new HipaVX::XorNode();
	auto vx = new _vx_node();
	vx->o = node;
	node->in_1 = convert(in1);
	node->in_2 = convert(in2);
	node->out = convert(out);
	convert(graph)->graph.emplace_back(node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxNotNode(vx_graph graph, vx_image input, vx_image output)
{
    auto node = new HipaVX::NotNode();
	auto vx = new _vx_node();
	vx->o = node;
	node->in = convert(input);
	node->out = convert(output);
	convert(graph)->graph.emplace_back(node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxAddNode(vx_graph graph, vx_image in1, vx_image in2, vx_enum policy, vx_image out)
{
	if (convert(out)->col == VX_DF_IMAGE_U8 && (convert(in1)->col != VX_DF_IMAGE_U8 ||
												convert(in2)->col != VX_DF_IMAGE_U8))
        return nullptr;

	if (convert(in1)->col != VX_DF_IMAGE_U8 && convert(in1)->col != VX_DF_IMAGE_S16)
        return nullptr;
	if (convert(in2)->col != VX_DF_IMAGE_U8 && convert(in2)->col != VX_DF_IMAGE_S16)
        return nullptr;
	if (convert(out)->col != VX_DF_IMAGE_U8 && convert(out)->col != VX_DF_IMAGE_S16)
        return nullptr;

    auto node = new HipaVX::VXAddNode();
	auto vx = new _vx_node();
	vx->o = node;
	node->in_1 = convert(in1);
	node->in_2 = convert(in2);
    node->policy = policy;
	node->out = convert(out);
	convert(graph)->graph.emplace_back(node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxSubtractNode(vx_graph graph, vx_image in1, vx_image in2, vx_enum policy, vx_image out)
{
	if (convert(out)->col == VX_DF_IMAGE_U8 && (convert(in1)->col != VX_DF_IMAGE_U8 ||
												convert(in2)->col != VX_DF_IMAGE_U8))
        return nullptr;

	if (convert(in1)->col != VX_DF_IMAGE_U8 && convert(in1)->col != VX_DF_IMAGE_S16)
        return nullptr;
	if (convert(in2)->col != VX_DF_IMAGE_U8 && convert(in2)->col != VX_DF_IMAGE_S16)
        return nullptr;
	if (convert(out)->col != VX_DF_IMAGE_U8 && convert(out)->col != VX_DF_IMAGE_S16)
        return nullptr;

    auto node = new HipaVX::VXSubtractNode();
	auto vx = new _vx_node();
	vx->o = node;
	node->in_1 = convert(in1);
	node->in_2 = convert(in2);
    node->policy = policy;
	node->out = convert(out);
	convert(graph)->graph.emplace_back(node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxConvertDepthNode(vx_graph graph, vx_image input, vx_image output, vx_enum policy, vx_scalar shift)
{
    auto node = new HipaVX::ConvertDepthNode();
	auto vx = new _vx_node();
	vx->o = node;
	node->in = convert(input);
	node->out = convert(output);
    node->policy = policy;
	node->shift = convert(shift);
	convert(graph)->graph.emplace_back(node);
	convert(graph)->built = false;
	return vx;
}

VX_API_ENTRY vx_node VX_API_CALL vxHarrisCornersNode (vx_graph graph, vx_image input, vx_scalar strength_thresh, vx_scalar min_distance,
                                                      vx_scalar sensitivity, vx_int32 gradient_size, vx_int32 block_size,
                                                      vx_array corners, vx_scalar num_corners)
{
	if (convert(input)->col != VX_DF_IMAGE_U8)
        return nullptr;

    if (gradient_size != 3 && block_size != 3)
        return nullptr;

	HipaVX::HarrisCorners *harris = new HipaVX::HarrisCorners(convert(input));
	auto vx = new _vx_node();
	vx->o = harris;
	harris->strength_thresh = convert(strength_thresh);
	harris->min_distance = convert(min_distance);
	harris->sensitivity = convert(sensitivity);
    harris->gradient_size = gradient_size;
    harris->block_size = block_size;
	harris->corners = convert(corners);
	harris->num_corners = convert(num_corners);

	convert(graph)->graph.emplace_back(harris);
	convert(graph)->built = false;
	return vx;
}

// TODO: change to Bilateral
VX_API_ENTRY vx_node VX_API_CALL vxAnotherBilateralFilterNode(vx_graph graph, vx_image input, vx_int32 sigma_r, vx_image output)
{
	if (convert(input)->col != VX_DF_IMAGE_U8 || convert(output)->col != VX_DF_IMAGE_U8)
        return nullptr;

    HipaVX::AnotherBilateralFilterNode *bilateral_filter = new HipaVX::AnotherBilateralFilterNode();
	auto vx = new _vx_node();
	vx->o = bilateral_filter;
	bilateral_filter->in = convert(input);
	bilateral_filter->out = convert(output);
    bilateral_filter->sigma_r = sigma_r;
	convert(graph)->graph.emplace_back(bilateral_filter);
	convert(graph)->built = false;
	return vx;
}
