#include "../VX/vx.h"
#include "domVX_extensions.hpp"
#include "kernels/domVX_kernels.hpp"

// High priority
// TODO: Nodes consisting of graphs should be described in here not in class
// definitions.
//       We can extend vxAPI so that composite graph functions can be written
//       with function calls.

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
static DomVX::Node* convert(vx_node n) { return (DomVX::Node*)(n->o); }
static DomVX::Graph* convert(vx_graph n) { return (DomVX::Graph*)(n->o); }
static DomVX::Image* convert(vx_image n) { return (DomVX::Image*)(n->o); }
static DomVX::Scalar* convert(vx_scalar n) { return (DomVX::Scalar*)(n->o); }
static DomVX::Array* convert(vx_array n) { return (DomVX::Array*)(n->o); }
static DomVX::Threshold* convert(vx_threshold n) {
  return (DomVX::Threshold*)(n->o);
}
static DomVX::Convolution* convert(vx_convolution n) {
  return (DomVX::Convolution*)(n->o);
}

VX_API_ENTRY vx_node VX_API_CALL vxChannelExtractNode(vx_graph graph,
                                                      vx_image input,
                                                      vx_enum channel,
                                                      vx_image output) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL
vxChannelCombineNode(vx_graph graph, vx_image plane0, vx_image plane1,
                     vx_image plane2, vx_image plane3, vx_image output) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxPhaseNode(vx_graph graph, vx_image grad_x,
                                             vx_image grad_y,
                                             vx_image orientation) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxCopyNode(vx_graph graph, vx_reference input,
                                            vx_reference output) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxSobel3x3Node(vx_graph graph, vx_image input,
                                                vx_image output_x,
                                                vx_image output_y) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxMagnitudeNode(vx_graph graph,
                                                 vx_image grad_x,
                                                 vx_image grad_y,
                                                 vx_image mag) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxMultiplyNode(vx_graph graph, vx_image in1,
                                                vx_image in2, vx_scalar scale,
                                                vx_enum overflow_policy,
                                                vx_enum rounding_policy,
                                                vx_image out) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxAbsDiffNode(vx_graph graph, vx_image in1,
                                               vx_image in2, vx_image out) {
  if (convert(out)->col == VX_DF_IMAGE_U8 &&
      (convert(in1)->col != VX_DF_IMAGE_U8 ||
       convert(in2)->col != VX_DF_IMAGE_U8))
    return nullptr;

  if (convert(in1)->col != VX_DF_IMAGE_U8 &&
      convert(in1)->col != VX_DF_IMAGE_S16)
    return nullptr;
  if (convert(in2)->col != VX_DF_IMAGE_U8 &&
      convert(in2)->col != VX_DF_IMAGE_S16)
    return nullptr;
  if (convert(out)->col != VX_DF_IMAGE_U8 &&
      convert(out)->col != VX_DF_IMAGE_S16)
    return nullptr;

  std::string out_type;
  std::string in1_type;
  std::string in2_type;

  if (convert(out)->col == VX_DF_IMAGE_U8)
    out_type = "u8";
  else
    out_type = "s16";
  if (convert(in1)->col == VX_DF_IMAGE_U8)
    in1_type = "u8";
  else
    in1_type = "s16";
  if (convert(in2)->col == VX_DF_IMAGE_U8)
    in2_type = "u8";
  else
    in2_type = "s16";

  vx_kernel kern = vxHipaccKernel("hipacc_kernels/point/absdiff_" + out_type +
                                  "_" + in1_type + "_" + in2_type + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)out);
  vxSetParameterByIndex(hn, 1, (vx_reference)in1);
  vxSetParameterByIndex(hn, 2, (vx_reference)in2);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxThresholdNode(vx_graph graph, vx_image input,
                                                 vx_threshold thresh,
                                                 vx_image output) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxErode3x3Node(vx_graph graph, vx_image input,
                                                vx_image output) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxDilate3x3Node(vx_graph graph, vx_image input,
                                                 vx_image output) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxBox3x3Node(vx_graph graph, vx_image input,
                                              vx_image output) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxGaussian3x3Node(vx_graph graph,
                                                   vx_image input,
                                                   vx_image output) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxConvolveNode(vx_graph graph, vx_image input,
                                                vx_convolution conv,
                                                vx_image output) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxAccumulateImageNode(vx_graph graph,
                                                       vx_image input,
                                                       vx_image accum) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxAccumulateWeightedImageNode(vx_graph graph,
                                                               vx_image input,
                                                               vx_scalar alpha,
                                                               vx_image accum) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxAccumulateSquareImageNode(vx_graph graph,
                                                             vx_image input,
                                                             vx_scalar shift,
                                                             vx_image accum) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxAndNode(vx_graph graph, vx_image in1,
                                           vx_image in2, vx_image out) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxOrNode(vx_graph graph, vx_image in1,
                                          vx_image in2, vx_image out) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxXorNode(vx_graph graph, vx_image in1,
                                           vx_image in2, vx_image out) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxNotNode(vx_graph graph, vx_image input,
                                           vx_image output) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxAddNode(vx_graph graph, vx_image in1,
                                           vx_image in2, vx_enum policy,
                                           vx_image out) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxSubtractNode(vx_graph graph, vx_image in1,
                                                vx_image in2, vx_enum policy,
                                                vx_image out) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxConvertDepthNode(vx_graph graph,
                                                    vx_image input,
                                                    vx_image output,
                                                    vx_enum policy,
                                                    vx_scalar shift) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxHarrisCornersNode(
    vx_graph graph, vx_image input, vx_scalar strength_thresh,
    vx_scalar min_distance, vx_scalar sensitivity, vx_int32 gradient_size,
    vx_int32 block_size, vx_array corners, vx_scalar num_corners) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxAnotherBilateralFilterNode(vx_graph graph,
                                                              vx_image input,
                                                              vx_int32 sigma_r,
                                                              vx_image output) {
  return nullptr;
}
