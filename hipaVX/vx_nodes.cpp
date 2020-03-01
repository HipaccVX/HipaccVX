#include "../VX/vx.h"
#include "domVX_extensions.hpp"
#include "dsl/abstractions.hpp"
#include "graph/graph.hpp"

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

static std::string type_str(DomVX::Image* image) {
  if (image->col == VX_DF_IMAGE_U8)
    return "u8";
  else if (image->col == VX_DF_IMAGE_S16)
    return "s16";
  else if (image->col == VX_TYPE_FLOAT32)
    return "f";
  else if (image->col == VX_DF_IMAGE_S32)
    return "s32";
  else if (image->col == VX_DF_IMAGE_U32)
    return "u32";
  else if (image->col == VX_DF_IMAGE_RGBX)
    return "uchar4";
  else if (image->col == VX_DF_IMAGE_UYVY)
    return "uchar4";
  else
    return "not_implemented";
}
static std::string type_str(vx_image image) { return type_str(convert(image)); }
static std::string policy_str(vx_enum policy) {
  if (policy == VX_CONVERT_POLICY_WRAP)
    return "wrap";
  else if (policy == VX_CONVERT_POLICY_SATURATE)
    return "saturate";
  else
    return "not_implemented";
}
static std::string channel_str(vx_channel_e channel) {
  switch (channel) {
    case VX_CHANNEL_0:
    case VX_CHANNEL_B:
      return "0";
    case VX_CHANNEL_1:
    case VX_CHANNEL_G:
      return "1";
    case VX_CHANNEL_2:
    case VX_CHANNEL_R:
      return "2";
    case VX_CHANNEL_3:
    case VX_CHANNEL_A:
      return "3";
    default:
      return "not_implemented";
  }
}

VX_API_ENTRY vx_node VX_API_CALL vxChannelExtractNode(vx_graph graph,
                                                      vx_image input,
                                                      vx_enum channel,
                                                      vx_image output) {
  if ((convert(input)->col != VX_DF_IMAGE_RGBX &&
       convert(input)->col != VX_DF_IMAGE_UYVY) ||
      convert(output)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern =
      vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/point/channelextract_" +
                     channel_str((vx_channel_e)channel) + "_" +
                     type_str(output) + "_" + type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL
vxChannelCombineNode(vx_graph graph, vx_image plane0, vx_image plane1,
                     vx_image plane2, vx_image plane3, vx_image output) {
  // currently only 4 channel -> rgbx is supported
  if (convert(output)->col != VX_DF_IMAGE_RGBX || plane0 == nullptr ||
      convert(plane0)->col != VX_DF_IMAGE_U8 || plane1 == nullptr ||
      convert(plane1)->col != VX_DF_IMAGE_U8 || plane2 == nullptr ||
      convert(plane2)->col != VX_DF_IMAGE_U8 || plane3 == nullptr ||
      convert(plane3)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern = vxHipaccKernel(
      std::string(HIPACC_KERNEL_DIR) + "/point/channelcombine_" +
      type_str(output) + "_" + type_str(plane0) + "_" + type_str(plane1) + "_" +
      type_str(plane2) + "_" + type_str(plane3) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 3, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 4, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)plane0);
  vxSetParameterByIndex(hn, 2, (vx_reference)plane1);
  vxSetParameterByIndex(hn, 3, (vx_reference)plane2);
  vxSetParameterByIndex(hn, 4, (vx_reference)plane3);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxPhaseNode(vx_graph graph, vx_image grad_x,
                                             vx_image grad_y,
                                             vx_image orientation) {
  return nullptr;
}

VX_API_ENTRY vx_node VX_API_CALL vxCopyNode(vx_graph graph, vx_reference input,
                                            vx_reference output) {
  // Current support only for images
  if (input->o->type() != VX_TYPE_IMAGE || output->o->type() != VX_TYPE_IMAGE) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }
  auto in_im = dynamic_cast<DomVX::Image*>(input->o);
  auto out_im = dynamic_cast<DomVX::Image*>(output->o);
  if (in_im->col != out_im->col ||
      (in_im->col != VX_DF_IMAGE_U8 && in_im->col != VX_DF_IMAGE_S16 &&
       in_im->col != VX_TYPE_FLOAT32)) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern =
      vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/point/copy_" +
                     type_str(out_im) + "_" + type_str(in_im) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxSobel3x3Node(vx_graph graph, vx_image input,
                                                vx_image output_x,
                                                vx_image output_y) {
  if ((convert(input)->col != VX_DF_IMAGE_U8) ||
      ((!output_x) && convert(output_x)->col != VX_DF_IMAGE_S16) ||
      ((!output_y) && convert(output_y)->col != VX_DF_IMAGE_S16)) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  auto multi_node = new DomVX::MultiNode();

  if (output_x) {
    const int coef[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    vx_context c = new _vx_context();
    c->o = convert(graph)->context;
    vx_matrix mat = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
    vxCopyMatrix(mat, (void*)coef, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

    vx_kernel kern =
        vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/sobelx_" +
                       type_str(output_x) + "_" + type_str(input) + ".hpp");
    vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_MATRIX, 0);

    auto hn = vxCreateGenericNode(graph, kern);
    vxSetParameterByIndex(hn, 0, (vx_reference)output_x);
    vxSetParameterByIndex(hn, 1, (vx_reference)input);
    vxSetParameterByIndex(hn, 2, (vx_reference)mat);

    multi_node->nodes.emplace_back(hn);
  }
  if (output_y) {
    const int coef[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    vx_context c = new _vx_context();
    c->o = convert(graph)->context;
    vx_matrix mat = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
    vxCopyMatrix(mat, (void*)coef, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

    vx_kernel kern =
        vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/sobely_" +
                       type_str(output_y) + "_" + type_str(input) + ".hpp");
    vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_MATRIX, 0);

    auto hn = vxCreateGenericNode(graph, kern);
    vxSetParameterByIndex(hn, 0, (vx_reference)output_y);
    vxSetParameterByIndex(hn, 1, (vx_reference)input);
    vxSetParameterByIndex(hn, 2, (vx_reference)mat);

    multi_node->nodes.emplace_back(hn);
  }

  vx_node n = new _vx_node();
  n->o = multi_node;
  return n;
}

VX_API_ENTRY vx_node VX_API_CALL vxMagnitudeNode(vx_graph graph,
                                                 vx_image grad_x,
                                                 vx_image grad_y,
                                                 vx_image mag) {
  if (convert(mag)->col != VX_DF_IMAGE_S16 ||
      convert(grad_x)->col != VX_DF_IMAGE_S16 ||
      convert(grad_y)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern = vxHipaccKernel(
      std::string(HIPACC_KERNEL_DIR) + "/point/magnitude_" + type_str(mag) +
      "_" + type_str(grad_x) + "_" + type_str(grad_y) + ".hpp");
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
  if (convert(out)->col == VX_DF_IMAGE_U8 &&
      (convert(in1)->col != VX_DF_IMAGE_U8 ||
       convert(in2)->col != VX_DF_IMAGE_U8)) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  if (convert(in1)->col != VX_DF_IMAGE_U8 &&
      convert(in1)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }
  if (convert(in2)->col != VX_DF_IMAGE_U8 &&
      convert(in2)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }
  if (convert(out)->col != VX_DF_IMAGE_U8 &&
      convert(out)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  if (rounding_policy !=
      VX_ROUND_POLICY_TO_ZERO) {  // Nearest not supported currently
    convert(graph)->status = VX_ERROR_INVALID_NODE;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_NOT_SUPPORTED;
    return nullptr;
  }

  vx_kernel kern =
      vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/point/mul_" +
                     policy_str(overflow_policy) + "_" + type_str(out) + "_" +
                     type_str(in1) + "_" + type_str(in2) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 3, VX_INPUT, VX_TYPE_SCALAR, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)out);
  vxSetParameterByIndex(hn, 1, (vx_reference)in1);
  vxSetParameterByIndex(hn, 2, (vx_reference)in2);
  vxSetParameterByIndex(hn, 3, (vx_reference)scale);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxAbsDiffNode(vx_graph graph, vx_image in1,
                                               vx_image in2, vx_image out) {
  if (convert(out)->col == VX_DF_IMAGE_U8 &&
      (convert(in1)->col != VX_DF_IMAGE_U8 ||
       convert(in2)->col != VX_DF_IMAGE_U8)) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  if (convert(in1)->col != VX_DF_IMAGE_U8 &&
      convert(in1)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }
  if (convert(in2)->col != VX_DF_IMAGE_U8 &&
      convert(in2)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }
  if (convert(out)->col != VX_DF_IMAGE_U8 &&
      convert(out)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern = vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) +
                                  "/point/absdiff_" + type_str(out) + "_" +
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
  if (convert(output)->col != VX_DF_IMAGE_U8 ||
      (convert(input)->col != VX_DF_IMAGE_U8 &&
       convert(input)->col != VX_DF_IMAGE_S16)) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_context c = new _vx_context();
  c->o = convert(graph)->context;

  int true_value = convert(thresh)->true_value;
  int false_value = convert(thresh)->false_value;
  auto true_scalar = vxCreateScalar(c, VX_TYPE_INT32, &true_value);
  auto false_scalar = vxCreateScalar(c, VX_TYPE_INT32, &false_value);

  if (convert(thresh)->threshold_type == VX_THRESHOLD_TYPE_BINARY) {
    int binary_value = convert(thresh)->value;
    auto binary_scalar = vxCreateScalar(c, VX_TYPE_INT32, &binary_value);
    vx_kernel kern = vxHipaccKernel(
        std::string(HIPACC_KERNEL_DIR) + "/point/thresholdbinary_" +
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
    vx_kernel kern = vxHipaccKernel(
        std::string(HIPACC_KERNEL_DIR) + "/point/thresholdrange_" +
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
      convert(input)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  const int coef[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix mat = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
  vxCopyMatrix(mat, (void*)coef, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_kernel kern =
      vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/erode_" +
                     type_str(output) + "_" + type_str(input) + ".hpp");
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
      convert(input)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  const int coef[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix mat = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
  vxCopyMatrix(mat, (void*)coef, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_kernel kern =
      vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/dilate_" +
                     type_str(output) + "_" + type_str(input) + ".hpp");
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
      convert(output)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  const int coef_box[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix box_values = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
  vxCopyMatrix(box_values, (void*)coef_box, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_kernel kern =
      vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/box_" +
                     type_str(output) + "_" + type_str(input) + ".hpp");
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
      convert(output)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  const int coef_box[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix box_values = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
  vxCopyMatrix(box_values, (void*)coef_box, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_kernel kern =
      vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/gaussian_" +
                     type_str(output) + "_" + type_str(input) + ".hpp");
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
  if (convert(input)->col != VX_DF_IMAGE_U8 ||
      (convert(output)->col != VX_DF_IMAGE_U8 &&
       convert(output)->col != VX_DF_IMAGE_S16)) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  auto convolution = convert(conv);
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix box_values =
      vxCreateMatrix(c, VX_TYPE_INT16, convolution->rows, convolution->columns);
  vxCopyMatrix(box_values, (void*)convolution->coefficients.data(),
               VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_scalar scale = vxCreateScalar(c, VX_TYPE_UINT32, &convolution->scale);

  vx_kernel kern =
      vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/convolve_" +
                     type_str(output) + "_" + type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_MATRIX, 0);
  vxAddParameterToKernel(kern, 3, VX_INPUT, VX_TYPE_SCALAR, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  vxSetParameterByIndex(hn, 2, (vx_reference)box_values);
  vxSetParameterByIndex(hn, 3, (vx_reference)scale);
  return hn;
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

VX_API_ENTRY vx_node VX_API_CALL vxNonMaxSuppressionNode(vx_graph graph,
                                                         vx_image input,
                                                         vx_image mask,
                                                         vx_int32 win_size,
                                                         vx_image output) {
  if ((convert(output)->col != VX_DF_IMAGE_U8 ||
       convert(input)->col != VX_DF_IMAGE_U8) &&
      (convert(output)->col != VX_DF_IMAGE_S16 ||
       convert(input)->col != VX_DF_IMAGE_S16)) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  if (mask != nullptr) {  // Currently not supported
    convert(graph)->status = VX_ERROR_INVALID_NODE;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_NOT_SUPPORTED;
    return nullptr;
  }

  if (win_size % 2 == 0) {
    convert(graph)->status = VX_ERROR_INVALID_VALUE;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_VALUE;
    return nullptr;
  }

  vx_int32 width = convert(input)->w;
  vx_int32 height = convert(input)->h;
  vx_int32 small_side = width;
  if (small_side > height) small_side = height;

  if (small_side < win_size) {
    convert(graph)->status = VX_ERROR_INVALID_VALUE;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_VALUE;
    return nullptr;
  }

  std::vector<int> coef(win_size * win_size, 1);
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix mat = vxCreateMatrix(c, VX_TYPE_INT32, win_size, win_size);
  vxCopyMatrix(mat, (void*)coef.data(), VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_kernel kern =
      vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/nonmax_" +
                     type_str(output) + "_" + type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_MATRIX, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  vxSetParameterByIndex(hn, 2, (vx_reference)mat);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxAndNode(vx_graph graph, vx_image in1,
                                           vx_image in2, vx_image out) {
  if (convert(out)->col != VX_DF_IMAGE_U8 ||
      convert(in1)->col != VX_DF_IMAGE_U8 ||
      convert(in2)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern = vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) +
                                  "/point/bitwiseand_" + type_str(out) + "_" +
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
      convert(in2)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern = vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) +
                                  "/point/bitwiseor_" + type_str(out) + "_" +
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
      convert(in2)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern = vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) +
                                  "/point/bitwisexor_" + type_str(out) + "_" +
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
      convert(input)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern =
      vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/point/bitwisenot_" +
                     type_str(output) + "_" + type_str(input) + ".hpp");
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
       convert(in2)->col != VX_DF_IMAGE_U8)) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  if (convert(in1)->col != VX_DF_IMAGE_U8 &&
      convert(in1)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }
  if (convert(in2)->col != VX_DF_IMAGE_U8 &&
      convert(in2)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }
  if (convert(out)->col != VX_DF_IMAGE_U8 &&
      convert(out)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern = vxHipaccKernel(
      std::string(HIPACC_KERNEL_DIR) + "/point/add_" + policy_str(policy) +
      "_" + type_str(out) + "_" + type_str(in1) + "_" + type_str(in2) + ".hpp");
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
       convert(in2)->col != VX_DF_IMAGE_U8)) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  if (convert(in1)->col != VX_DF_IMAGE_U8 &&
      convert(in1)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }
  if (convert(in2)->col != VX_DF_IMAGE_U8 &&
      convert(in2)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }
  if (convert(out)->col != VX_DF_IMAGE_U8 &&
      convert(out)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern = vxHipaccKernel(
      std::string(HIPACC_KERNEL_DIR) + "/point/sub_" + policy_str(policy) +
      "_" + type_str(out) + "_" + type_str(in1) + "_" + type_str(in2) + ".hpp");
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
  if (convert(input)->col == VX_DF_IMAGE_U8 &&
      convert(output)->col != VX_DF_IMAGE_S16) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }
  if (convert(input)->col == VX_DF_IMAGE_S16 &&
      convert(output)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }
  if (convert(shift)->data_type != VX_TYPE_INT32) return nullptr;
  if (convert(shift)->i32 < 0 || convert(shift)->i32 >= 8) return nullptr;

  // Up conversion ignores the policy
  if (convert(output)->col == VX_DF_IMAGE_S16) {
    vx_kernel kern =
        vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/point/convert_" +
                       type_str(output) + "_" + type_str(input) + ".hpp");
    vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_SCALAR, 0);

    auto hn = vxCreateGenericNode(graph, kern);
    vxSetParameterByIndex(hn, 0, (vx_reference)output);
    vxSetParameterByIndex(hn, 1, (vx_reference)input);
    vxSetParameterByIndex(hn, 2, (vx_reference)shift);
    return hn;
  } else {
    vx_kernel kern =
        vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/point/convert_" +
                       policy_str(policy) + "_" + type_str(output) + "_" +
                       type_str(input) + ".hpp");
    vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_SCALAR, 0);

    auto hn = vxCreateGenericNode(graph, kern);
    vxSetParameterByIndex(hn, 0, (vx_reference)output);
    vxSetParameterByIndex(hn, 1, (vx_reference)input);
    vxSetParameterByIndex(hn, 2, (vx_reference)shift);
    return hn;
  }
}

VX_API_ENTRY vx_node VX_API_CALL vxHarrisCornersNode(
    vx_graph graph, vx_image input, vx_scalar strength_thresh,
    vx_scalar min_distance, vx_scalar sensitivity, vx_int32 gradient_size,
    vx_int32 block_size, vx_array corners, vx_scalar num_corners) {
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;

  vx_matrix sobel_x, sobel_y, window;
  if (gradient_size == 3) {
    const int sobel_x_coefs[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    sobel_x = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
    vxCopyMatrix(sobel_x, (void*)sobel_x_coefs, VX_WRITE_ONLY,
                 VX_MEMORY_TYPE_HOST);
    const int sobel_y_coefs[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    sobel_y = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
    vxCopyMatrix(sobel_y, (void*)sobel_y_coefs, VX_WRITE_ONLY,
                 VX_MEMORY_TYPE_HOST);
  } else if (gradient_size == 5) {
    const int sobel_x_coefs[25] = {-1, -2, 0,   2,  1,  -4, -8, 0,  8,
                                   4,  -6, -12, 0,  12, 6,  -4, -8, 0,
                                   8,  4,  -1,  -2, 0,  2,  1};
    sobel_x = vxCreateMatrix(c, VX_TYPE_INT32, 5, 5);
    vxCopyMatrix(sobel_x, (void*)sobel_x_coefs, VX_WRITE_ONLY,
                 VX_MEMORY_TYPE_HOST);
    const int sobel_y_coefs[25] = {-1, -4, -6, -4, -1, -2, -8, -12, -8,
                                   -2, 0,  0,  0,  0,  0,  2,  8,   12,
                                   8,  2,  1,  4,  6,  4,  1};
    sobel_y = vxCreateMatrix(c, VX_TYPE_INT32, 5, 5);
    vxCopyMatrix(sobel_y, (void*)sobel_y_coefs, VX_WRITE_ONLY,
                 VX_MEMORY_TYPE_HOST);
  } else if (gradient_size == 7) {
    const int sobel_x_coefs[49] = {
        -1, -4,  -5,  0,   5,   4,  1,  -6, -24, -30, 0,    30,  24,
        6,  -15, -60, -75, 0,   75, 60, 15, -20, -80, -100, 0,   100,
        80, 20,  -15, -60, -75, 0,  75, 60, 15,  -6,  -24,  -30, 0,
        30, 24,  6,   -1,  -4,  -5, 0,  5,  4,   1};
    sobel_x = vxCreateMatrix(c, VX_TYPE_INT32, 7, 7);
    vxCopyMatrix(sobel_x, (void*)sobel_x_coefs, VX_WRITE_ONLY,
                 VX_MEMORY_TYPE_HOST);
    const int sobel_y_coefs[49] = {
        -1, -6, -15, -20, -15,  -6,  -1,  -4, -24, -60, -80, -60, -24,
        -4, -5, -30, -75, -100, -75, -30, -5, 0,   0,   0,   0,   0,
        0,  0,  5,   30,  75,   100, 75,  30, 5,   4,   24,  60,  80,
        60, 24, 4,   1,   6,    15,  20,  15, 6,   1};
    sobel_y = vxCreateMatrix(c, VX_TYPE_INT32, 7, 7);
    vxCopyMatrix(sobel_y, (void*)sobel_y_coefs, VX_WRITE_ONLY,
                 VX_MEMORY_TYPE_HOST);
  } else {
    convert(graph)->status = VX_ERROR_INVALID_VALUE;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_VALUE;
    return nullptr;
  }

  if (block_size == 3) {
    const int block_coefs[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    window = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
    vxCopyMatrix(window, (void*)block_coefs, VX_WRITE_ONLY,
                 VX_MEMORY_TYPE_HOST);
  } else if (gradient_size == 5) {
    const int block_coefs[25] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    window = vxCreateMatrix(c, VX_TYPE_INT32, 5, 5);
    vxCopyMatrix(window, (void*)block_coefs, VX_WRITE_ONLY,
                 VX_MEMORY_TYPE_HOST);
  } else if (gradient_size == 7) {
    const int block_coefs[49] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    window = vxCreateMatrix(c, VX_TYPE_INT32, 7, 7);
    vxCopyMatrix(window, (void*)block_coefs, VX_WRITE_ONLY,
                 VX_MEMORY_TYPE_HOST);
  } else {
    convert(graph)->status = VX_ERROR_INVALID_VALUE;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_VALUE;
    return nullptr;
  }

  auto multi_node = new DomVX::MultiNode();

  vx_scalar block_scalar = vxCreateScalar(c, VX_TYPE_INT32, &block_size);

  vx_image sobel_x_image =
      vxCreateImage(c, convert(input)->w, convert(input)->h, VX_TYPE_FLOAT32);
  vx_image sobel_y_image =
      vxCreateImage(c, convert(input)->w, convert(input)->h, VX_TYPE_FLOAT32);
  vx_image trace_image =
      vxCreateImage(c, convert(input)->w, convert(input)->h, VX_TYPE_FLOAT32);
  vx_image det_image =
      vxCreateImage(c, convert(input)->w, convert(input)->h, VX_TYPE_FLOAT32);
  vx_image vc_image =
      vxCreateImage(c, convert(input)->w, convert(input)->h, VX_TYPE_FLOAT32);

  // Gx and Gy
  {
    vx_kernel kern_sobel = vxHipaccKernel(
        std::string(HIPACC_KERNEL_DIR) + "/local/harris/sobel_" +
        type_str(sobel_x_image) + "_" + type_str(input) + ".hpp");
    vxAddParameterToKernel(kern_sobel, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern_sobel, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern_sobel, 2, VX_INPUT, VX_TYPE_MATRIX, 0);
    vxAddParameterToKernel(kern_sobel, 3, VX_INPUT, VX_TYPE_SCALAR, 0);

    auto hn_sobel_x = vxCreateGenericNode(graph, kern_sobel);
    vxSetParameterByIndex(hn_sobel_x, 0, (vx_reference)sobel_x_image);
    vxSetParameterByIndex(hn_sobel_x, 1, (vx_reference)input);
    vxSetParameterByIndex(hn_sobel_x, 2, (vx_reference)sobel_x);
    vxSetParameterByIndex(hn_sobel_x, 3, (vx_reference)block_scalar);
    multi_node->nodes.emplace_back(hn_sobel_x);

    auto hn_sobel_y = vxCreateGenericNode(graph, kern_sobel);
    vxSetParameterByIndex(hn_sobel_y, 0, (vx_reference)sobel_y_image);
    vxSetParameterByIndex(hn_sobel_y, 1, (vx_reference)input);
    vxSetParameterByIndex(hn_sobel_y, 2, (vx_reference)sobel_y);
    vxSetParameterByIndex(hn_sobel_y, 3, (vx_reference)block_scalar);
    multi_node->nodes.emplace_back(hn_sobel_y);
  }
  // Trace and Det
  {
    vx_kernel kern_trace =
        vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/harris/trace_" +
                       type_str(trace_image) + "_" + type_str(sobel_x_image) +
                       "_" + type_str(sobel_y_image) + ".hpp");
    vxAddParameterToKernel(kern_trace, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern_trace, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern_trace, 2, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern_trace, 3, VX_INPUT, VX_TYPE_MATRIX, 0);

    auto hn_trace = vxCreateGenericNode(graph, kern_trace);
    vxSetParameterByIndex(hn_trace, 0, (vx_reference)trace_image);
    vxSetParameterByIndex(hn_trace, 1, (vx_reference)sobel_x_image);
    vxSetParameterByIndex(hn_trace, 2, (vx_reference)sobel_y_image);
    vxSetParameterByIndex(hn_trace, 3, (vx_reference)window);
    multi_node->nodes.emplace_back(hn_trace);

    vx_kernel kern_det =
        vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/harris/det_" +
                       type_str(det_image) + "_" + type_str(sobel_x_image) +
                       "_" + type_str(sobel_y_image) + ".hpp");
    vxAddParameterToKernel(kern_det, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern_det, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern_det, 2, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern_det, 3, VX_INPUT, VX_TYPE_MATRIX, 0);

    auto hn_det = vxCreateGenericNode(graph, kern_det);
    vxSetParameterByIndex(hn_det, 0, (vx_reference)det_image);
    vxSetParameterByIndex(hn_det, 1, (vx_reference)sobel_x_image);
    vxSetParameterByIndex(hn_det, 2, (vx_reference)sobel_y_image);
    vxSetParameterByIndex(hn_det, 3, (vx_reference)window);
    multi_node->nodes.emplace_back(hn_det);
  }
  // Mc and Vc in one kernel
  {
    vx_kernel kern_McVc =
        vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/harris/mcvc_" +
                       type_str(vc_image) + "_" + type_str(det_image) + "_" +
                       type_str(trace_image) + ".hpp");
    vxAddParameterToKernel(kern_McVc, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern_McVc, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern_McVc, 2, VX_INPUT, VX_TYPE_IMAGE, 0);
    vxAddParameterToKernel(kern_McVc, 3, VX_INPUT, VX_TYPE_SCALAR, 0);
    vxAddParameterToKernel(kern_McVc, 4, VX_INPUT, VX_TYPE_SCALAR, 0);

    auto hn_McVc = vxCreateGenericNode(graph, kern_McVc);
    vxSetParameterByIndex(hn_McVc, 0, (vx_reference)vc_image);
    vxSetParameterByIndex(hn_McVc, 1, (vx_reference)det_image);
    vxSetParameterByIndex(hn_McVc, 2, (vx_reference)trace_image);
    vxSetParameterByIndex(hn_McVc, 3, (vx_reference)sensitivity);
    vxSetParameterByIndex(hn_McVc, 4, (vx_reference)strength_thresh);
    multi_node->nodes.emplace_back(hn_McVc);
  }
  vx_node node = new _vx_node();
  node->o = multi_node;
  return node;
}

VX_API_ENTRY vx_node VX_API_CALL vxMedian3x3Node(vx_graph graph, vx_image input,
                                                 vx_image output) {
  if (convert(input)->col != VX_DF_IMAGE_U8 ||
      convert(output)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  const int coef_box[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix box_values = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
  vxCopyMatrix(box_values, (void*)coef_box, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  vx_kernel kern =
      vxHipaccKernel(std::string(HIPACC_KERNEL_DIR) + "/local/median_" +
                     type_str(output) + "_" + type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_MATRIX, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  vxSetParameterByIndex(hn, 2, (vx_reference)box_values);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxFastCornersNode(
    vx_graph graph, vx_image input, vx_scalar strength_thresh,
    vx_bool nonmax_suppression, vx_array corners, vx_scalar num_corners) {
  if (convert(input)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  const int coef[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
  vx_context c = new _vx_context();
  c->o = convert(graph)->context;
  vx_matrix mat = vxCreateMatrix(c, VX_TYPE_INT32, 3, 3);
  vxCopyMatrix(mat, (void*)coef, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);

  DomVX::Bool* b = new DomVX::Bool(nonmax_suppression);
  _vx_bool* adaptor_b = new _vx_bool();
  adaptor_b->o = b;

  vx_kernel kern = vxCppKernel(std::string(CPP_KERNEL_DIR) + "/local/fast9_" +
                               type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_ARRAY, 0);
  vxAddParameterToKernel(kern, 1, VX_OUTPUT, VX_TYPE_SCALAR,
                         VX_PARAMETER_STATE_OPTIONAL);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 3, VX_INPUT, VX_TYPE_SCALAR, 0);  // thresh
  vxAddParameterToKernel(kern, 4, VX_INPUT, VX_TYPE_BOOL,
                         0);  // nonmax_suppression

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)corners);
  vxSetParameterByIndex(hn, 1, (vx_reference)num_corners);
  vxSetParameterByIndex(hn, 2, (vx_reference)input);
  vxSetParameterByIndex(hn, 3, (vx_reference)strength_thresh);
  vxSetParameterByIndex(hn, 4, (vx_reference)adaptor_b);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxEqualizeHistNode(vx_graph graph,
                                                    vx_image input,
                                                    vx_image output) {
  if (convert(input)->col != VX_DF_IMAGE_U8 ||
      convert(output)->col != VX_DF_IMAGE_U8) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern =
      vxCppKernel(std::string(CPP_KERNEL_DIR) + "/global/eq_hist_" +
                  type_str(output) + "_" + type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxIntegralImageNode(vx_graph graph,
                                                     vx_image input,
                                                     vx_image output) {
  if (convert(input)->col != VX_DF_IMAGE_U8 ||
      convert(output)->col != VX_DF_IMAGE_U32) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern =
      vxCppKernel(std::string(CPP_KERNEL_DIR) + "/global/integral_" +
                  type_str(output) + "_" + type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_IMAGE, 0);
  vxAddParameterToKernel(kern, 1, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)output);
  vxSetParameterByIndex(hn, 1, (vx_reference)input);
  return hn;
}

VX_API_ENTRY vx_node VX_API_CALL vxMeanStdDevNode(vx_graph graph,
                                                  vx_image input,
                                                  vx_scalar mean,
                                                  vx_scalar stddev) {
  if (convert(input)->col != VX_DF_IMAGE_U8 ||
      convert(mean)->data_type != VX_TYPE_FLOAT32 ||
      (stddev && convert(stddev)->data_type != VX_TYPE_FLOAT32)) {
    convert(graph)->status = VX_ERROR_INVALID_FORMAT;
    auto vx = new _vx_node();
    vx->o = new DomVX::Object();
    vx->o->status = VX_ERROR_INVALID_FORMAT;
    return nullptr;
  }

  vx_kernel kern =
      vxCppKernel(std::string(CPP_KERNEL_DIR) + "/global/mean_std_dev_" +
                  type_str(input) + ".hpp");
  vxAddParameterToKernel(kern, 0, VX_OUTPUT, VX_TYPE_SCALAR, 0);
  vxAddParameterToKernel(kern, 1, VX_OUTPUT, VX_TYPE_SCALAR,
                         VX_PARAMETER_STATE_OPTIONAL);
  vxAddParameterToKernel(kern, 2, VX_INPUT, VX_TYPE_IMAGE, 0);

  auto hn = vxCreateGenericNode(graph, kern);
  vxSetParameterByIndex(hn, 0, (vx_reference)mean);
  vxSetParameterByIndex(hn, 1, (vx_reference)stddev);
  vxSetParameterByIndex(hn, 2, (vx_reference)input);
  return hn;
}
