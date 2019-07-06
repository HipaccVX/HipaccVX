#include "../VX/vx.h"
#include "domVX_extensions.hpp"
#include "graph/graph.hpp"
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

static std::string type_str(vx_image image) {
  if (convert(image)->col == VX_DF_IMAGE_U8)
    return "u8";
  else if (convert(image)->col == VX_DF_IMAGE_S16)
    return "s16";
  else
    return "not_implemented";
}
static std::string policy_str(vx_enum policy) {
  if (policy == VX_CONVERT_POLICY_WRAP)
    return "wrap";
  else if (policy == VX_CONVERT_POLICY_SATURATE)
    return "saturate";
  else
    return "not_implemented";
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
  if ((convert(input)->col != VX_DF_IMAGE_U8) ||
      ((!output_x) && convert(output_x)->col != VX_DF_IMAGE_S16) ||
      ((!output_y) && convert(output_y)->col != VX_DF_IMAGE_S16))
    return nullptr;

  return nullptr;  // We can't support multiple image outputs currently
  /*if (output_x)
  const int coef[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix mat = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
  vxCopyMatrix(mat, (void*)coef, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_kernel kern = vxHipaccKernel("hipacc_kernels/local/erode_" + type(output) +
                                  "_" + type(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_MATRIX, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  vxSetParameterByIndex(hn, 2, (vx_reference)mat);
  return hn;*/
}

VX_API_ENTRY vx_node VX_API_CALL vxMagnitudeNode(vx_graph graph,
                                                 vx_image grad_x,
                                                 vx_image grad_y,
                                                 vx_image mag) {
  if (convert(mag)->col != VX_DF_IMAGE_S16 ||
      convert(grad_x)->col != VX_DF_IMAGE_S16 ||
      convert(grad_y)->col != VX_DF_IMAGE_S16)
    return nullptr;

  vx_kernel kern =
      vxHipaccKernel("hipacc_kernels/point/magnitude_" + type_str(mag) + "_" +
                     type_str(grad_x) + "_" + type_str(grad_y) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)mag);
  vxSetParameterByIndex(hn, 1, (vx_reference)grad_x);
  vxSetParameterByIndex(hn, 2, (vx_reference)grad_y);
  return hn;
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

  vx_kernel kern =
      vxHipaccKernel("hipacc_kernels/point/absdiff_" + type_str(out) + "_" +
                     type_str(in1) + "_" + type_str(in2) + ".hpp");
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
  // Currently only output U8 supported
  if (convert(output)->col != VX_DF_IMAGE_U8 ||
      convert(input)->col != VX_DF_IMAGE_U8)
    return nullptr;

  vx_context c = new _vx_context();
  c->o = convert(graph)->context;

  int true_value = convert(thresh)->true_value;
  int false_value = convert(thresh)->false_value;
  auto true_scalar = vxCreateScalar(c, VX_TYPE_INT32, &true_value);
  auto false_scalar = vxCreateScalar(c, VX_TYPE_INT32, &false_value);

  if (convert(thresh)->threshold_type == VX_THRESHOLD_TYPE_BINARY) {
    int binary_value = convert(thresh)->value;
    auto binary_scalar = vxCreateScalar(c, VX_TYPE_INT32, &binary_value);
    vx_kernel kern =
        vxHipaccKernel("hipacc_kernels/point/thresholdbinary_" +
                       type_str(output) + "_" + type_str(input) + ".hpp");
    vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_SCALAR, 0);
    vxAddParameterToKernel(kern, 3, VX_INPUT, VX_TYPE_SCALAR, 0);
    vxAddParameterToKernel(kern, 4, VX_INPUT, VX_TYPE_SCALAR, 0);

    auto hn = vxCreateGenericNode(graph, kern);
    vxSetParameterByIndex(hn, 0, (vx_reference)output);
    vxSetParameterByIndex(hn, 1, (vx_reference)input);
    vxSetParameterByIndex(hn, 2, (vx_reference)true_scalar);
    vxSetParameterByIndex(hn, 3, (vx_reference)false_scalar);
    vxSetParameterByIndex(hn, 4, (vx_reference)binary_scalar);
    return hn;
  } else {
    int lower_value = convert(thresh)->lower;
    int upper_value = convert(thresh)->upper;
    auto lower_scalar = vxCreateScalar(c, VX_TYPE_INT32, &lower_value);
    auto upper_scalar = vxCreateScalar(c, VX_TYPE_INT32, &upper_value);
    vx_kernel kern =
        vxHipaccKernel("hipacc_kernels/point/thresholdrange_" +
                       type_str(output) + "_" + type_str(input) + ".hpp");
    vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_SCALAR, 0);
    vxAddParameterToKernel(kern, 3, VX_INPUT, VX_TYPE_SCALAR, 0);
    vxAddParameterToKernel(kern, 4, VX_INPUT, VX_TYPE_SCALAR, 0);
    vxAddParameterToKernel(kern, 5, VX_INPUT, VX_TYPE_SCALAR, 0);

    auto hn = vxCreateGenericNode(graph, kern);
    vxSetParameterByIndex(hn, 0, (vx_reference)output);
    vxSetParameterByIndex(hn, 1, (vx_reference)input);
    vxSetParameterByIndex(hn, 2, (vx_reference)true_scalar);
    vxSetParameterByIndex(hn, 3, (vx_reference)false_scalar);
    vxSetParameterByIndex(hn, 4, (vx_reference)lower_scalar);
    vxSetParameterByIndex(hn, 5, (vx_reference)upper_scalar);
    return hn;
  }
}

VX_API_ENTRY vx_node VX_API_CALL vxErode3x3Node(vx_graph graph, vx_image input,
                                                vx_image output) {
  if (convert(output)->col != VX_DF_IMAGE_U8 ||
      convert(input)->col != VX_DF_IMAGE_U8)
    return nullptr;

  const int coef[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix mat = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
  vxCopyMatrix(mat, (void*)coef, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_kernel kern =
      vxHipaccKernel("hipacc_kernels/local/erode_" + type_str(output) + "_" +
                     type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_MATRIX, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  vxSetParameterByIndex(hn, 2, (vx_reference)mat);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxDilate3x3Node(vx_graph graph, vx_image input,
                                                 vx_image output) {
  if (convert(output)->col != VX_DF_IMAGE_U8 ||
      convert(input)->col != VX_DF_IMAGE_U8)
    return nullptr;

  const int coef[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix mat = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
  vxCopyMatrix(mat, (void*)coef, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_kernel kern =
      vxHipaccKernel("hipacc_kernels/local/dilate_" + type_str(output) + "_" +
                     type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_MATRIX, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  vxSetParameterByIndex(hn, 2, (vx_reference)mat);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxBox3x3Node(vx_graph graph, vx_image input,
                                              vx_image output) {
  if (convert(input)->col != VX_DF_IMAGE_U8 ||
      convert(output)->col != VX_DF_IMAGE_U8)
    return nullptr;

  const int coef_box[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix box_values = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
  vxCopyMatrix(box_values, (void*)coef_box, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_kernel kern =
      vxHipaccKernel("hipacc_kernels/local/box_" + type_str(output) + "_" +
                     type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_MATRIX, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  vxSetParameterByIndex(hn, 2, (vx_reference)box_values);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxGaussian3x3Node(vx_graph graph,
                                                   vx_image input,
                                                   vx_image output) {
  if (convert(input)->col != VX_DF_IMAGE_U8 ||
      convert(output)->col != VX_DF_IMAGE_U8)
    return nullptr;

  const int coef_box[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix box_values = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
  vxCopyMatrix(box_values, (void*)coef_box, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_kernel kern =
      vxHipaccKernel("hipacc_kernels/local/gaussian_" + type_str(output) + "_" +
                     type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_MATRIX, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  vxSetParameterByIndex(hn, 2, (vx_reference)box_values);
  return hn;
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
  if (convert(out)->col != VX_DF_IMAGE_U8 ||
      convert(in1)->col != VX_DF_IMAGE_U8 ||
      convert(in2)->col != VX_DF_IMAGE_U8)
    return nullptr;

  vx_kernel kern =
      vxHipaccKernel("hipacc_kernels/point/bitwiseand_" + type_str(out) + "_" +
                     type_str(in1) + "_" + type_str(in2) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)out);
  vxSetParameterByIndex(hn, 1, (vx_reference)in1);
  vxSetParameterByIndex(hn, 2, (vx_reference)in2);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxOrNode(vx_graph graph, vx_image in1,
                                          vx_image in2, vx_image out) {
  if (convert(out)->col != VX_DF_IMAGE_U8 ||
      convert(in1)->col != VX_DF_IMAGE_U8 ||
      convert(in2)->col != VX_DF_IMAGE_U8)
    return nullptr;

  vx_kernel kern =
      vxHipaccKernel("hipacc_kernels/point/bitwiseor_" + type_str(out) + "_" +
                     type_str(in1) + "_" + type_str(in2) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)out);
  vxSetParameterByIndex(hn, 1, (vx_reference)in1);
  vxSetParameterByIndex(hn, 2, (vx_reference)in2);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxXorNode(vx_graph graph, vx_image in1,
                                           vx_image in2, vx_image out) {
  if (convert(out)->col != VX_DF_IMAGE_U8 ||
      convert(in1)->col != VX_DF_IMAGE_U8 ||
      convert(in2)->col != VX_DF_IMAGE_U8)
    return nullptr;

  vx_kernel kern =
      vxHipaccKernel("hipacc_kernels/point/bitwisexor_" + type_str(out) + "_" +
                     type_str(in1) + "_" + type_str(in2) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)out);
  vxSetParameterByIndex(hn, 1, (vx_reference)in1);
  vxSetParameterByIndex(hn, 2, (vx_reference)in2);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxNotNode(vx_graph graph, vx_image input,
                                           vx_image output) {
  if (convert(output)->col != VX_DF_IMAGE_U8 ||
      convert(input)->col != VX_DF_IMAGE_U8)
    return nullptr;

  vx_kernel kern =
      vxHipaccKernel("hipacc_kernels/point/bitwisenot_" + type_str(output) +
                     "_" + type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxAddNode(vx_graph graph, vx_image in1,
                                           vx_image in2, vx_enum policy,
                                           vx_image out) {
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

  vx_kernel kern = vxHipaccKernel(
      "hipacc_kernels/point/add_" + policy_str(policy) + "_" + type_str(out) +
      "_" + type_str(in1) + "_" + type_str(in2) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)out);
  vxSetParameterByIndex(hn, 1, (vx_reference)in1);
  vxSetParameterByIndex(hn, 2, (vx_reference)in2);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxSubtractNode(vx_graph graph, vx_image in1,
                                                vx_image in2, vx_enum policy,
                                                vx_image out) {
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

  vx_kernel kern = vxHipaccKernel(
      "hipacc_kernels/point/sub_" + policy_str(policy) + "_" + type_str(out) +
      "_" + type_str(in1) + "_" + type_str(in2) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)out);
  vxSetParameterByIndex(hn, 1, (vx_reference)in1);
  vxSetParameterByIndex(hn, 2, (vx_reference)in2);
  return hn;
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
