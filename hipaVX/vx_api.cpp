#include <cstring>
#include <exception>

#include "../VX/vx.h"
#include "../VX/vx_compatibility.h"
#include "dsl/abstractions.hpp"
#include "gen/hipacc_graph.hpp"
#include "graph/graph.hpp"
#include "vx_adaptor.hpp"

// TODO: Consider individual files for the API set of the following objects:
//          Object: Reference
//          Object: Context
//          Object: Graph
//          Object: Node
//          Object: Array
//          Object: Convolution
//          Object: Distribution
//          Object: Image
//          Object: LUT
//          Object: Matrix
//          Object: Pyramid
//          Object: Remap
//          Object: Scalar
//          Object: Threshold
//          Object: ObjectArray
//          Object: Tensor

// Error Checking
VX_API_ENTRY vx_status VX_API_CALL vxGetStatus(vx_reference reference) {
  return VX_SUCCESS;
}

// Object: Context
VX_API_ENTRY vx_context VX_API_CALL vxCreateContext(void) {
  auto vx = new _vx_context();
  vx->o = new DomVX::Context();
  return vx;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseContext(vx_context *context) {
  return 0;
}

// Object: Graph
VX_API_ENTRY vx_graph VX_API_CALL vxCreateGraph(vx_context context) {
  auto graph = new DomVX::Graph();
  graph->dag.reset(new graphVX::dag());
  graph->context = (DomVX::Context *)(context->o);
  auto vx = new _vx_graph();
  vx->o = graph;
  ((DomVX::Context *)(context->o))->graphs.emplace_back(graph);
  return vx;
}

VX_API_ENTRY vx_status VX_API_CALL vxVerifyGraph(vx_graph graph) {
  ((DomVX::Graph *)(graph->o))->build();

  // TODO: Consider having a generator class for every backend
  // process_graph(((DomVX::Graph *)(graph->o)));
  auto &dag = ((DomVX::Graph *)(graph->o))->dag;
  //dag->dont_eliminate_dead_nodes();
  dag->dump_graph("graph");
  dag->set_io_nodes();
  dag->eliminate_dead_nodes();
  dag->dump_optimized("optimized");

  return VX_SUCCESS;
}

VX_API_ENTRY vx_status VX_API_CALL vxProcessGraph(vx_graph graph) {
  // 1. Get the base main function
  // First, declare the images
  auto &dag = ((DomVX::Graph *)(graph->o))->dag;

  for (auto out : ((DomVX::Graph *)(graph->o))->write_after_completion)
    dag->space_to_file[out.first] = out.second;

  hipacc_gen gen(*dag);
  gen.set_edges();
  gen.iterate_spaces();
  gen.iterate_nodes();
  gen.dump_code();
  return 0;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseGraph(vx_graph *graph) { return 0; }

// Object: Node
VX_API_ENTRY vx_status VX_API_CALL vxSetNodeAttribute(vx_node node,
                                                      vx_enum attribute,
                                                      void *ptr, vx_size size) {
  if (attribute == VX_NODE_BORDER) {
    ((DomVX::Node *)(node->o))->border_mode = *((vx_border_e *)ptr);
    return VX_SUCCESS;
  }
  return VX_ERROR_NOT_IMPLEMENTED;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseNode(vx_node *node) { return 0; }

// Object: Array
VX_API_ENTRY vx_array VX_API_CALL vxCreateArray(vx_context context,
                                                vx_enum data_type,
                                                vx_size size) {
  if (data_type != VX_TYPE_KEYPOINT)
    throw std::runtime_error(
        "vx_array: Only VX_TYPE_KEYPOINT is currently supported");

  // vx_keypoint_t has 7 32bit members
  DomVX::Array *arr = new DomVX::Array(data_type, size);
  auto vx = new _vx_array();
  vx->o = arr;
  ((DomVX::Context *)(context->o))->arrays.emplace_back(arr);
  return vx;
}

// Object: Convolution
VX_API_ENTRY vx_convolution VX_API_CALL vxCreateConvolution(vx_context context,
                                                            vx_size columns,
                                                            vx_size rows) {
  DomVX::Convolution *conv = new DomVX::Convolution();
  auto vx = new _vx_convolution();
  vx->o = conv;
  conv->columns = columns;
  conv->rows = rows;
  conv->coefficients.resize(conv->columns * conv->rows);
  return vx;
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyConvolutionCoefficients(
    vx_convolution conv, void *user_ptr, vx_enum usage, vx_enum user_mem_type) {
  if (usage != VX_WRITE_ONLY && user_mem_type != VX_MEMORY_TYPE_HOST)
    return VX_FAILURE;

  vx_int16 *int16_ptr = (vx_int16 *)user_ptr;
  for (unsigned int i = 0;
       i < ((DomVX::Convolution *)(conv->o))->coefficients.size(); i++) {
    ((DomVX::Convolution *)(conv->o))->coefficients[i] = int16_ptr[i];
  }

  return VX_SUCCESS;
}

VX_API_ENTRY vx_status VX_API_CALL vxSetConvolutionAttribute(
    vx_convolution conv, vx_enum attribute, const void *ptr, vx_size size) {
  if (attribute != VX_CONVOLUTION_SCALE) return VX_FAILURE;

  ((DomVX::Convolution *)(conv->o))->scale = *((vx_uint32 *)ptr);

  return VX_SUCCESS;
}

// Object: Image
VX_API_ENTRY vx_image VX_API_CALL vxCreateImage(vx_context context,
                                                vx_uint32 width,
                                                vx_uint32 height,
                                                vx_df_image color) {
  DomVX::Image *image = new DomVX::Image(width, height, color);
  auto vx = new _vx_image();
  vx->o = image;
  ((DomVX::Context *)(context->o))->images.emplace_back(image);
  return vx;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseImage(vx_image *image) { return 0; }

// Object: Matrix
VX_API_ENTRY vx_matrix VX_API_CALL vxCreateMatrix(vx_context c,
                                                  vx_enum data_type,
                                                  vx_size columns,
                                                  vx_size rows) {
  if (data_type != VX_TYPE_UINT8 && data_type != VX_TYPE_INT16 &&
      data_type != VX_TYPE_INT32 && data_type != VX_TYPE_FLOAT32)
    return nullptr;

  DomVX::VX_Matrix *mat = new DomVX::VX_Matrix();
  auto vx = new _vx_matrix();
  vx->o = mat;

  mat->data_type = data_type;
  mat->columns = columns;
  mat->rows = rows;
  mat->mat.resize(sizeof(int32_t) * columns *
                  rows);  // Optimization available: Only resize to what you
                          // actually need (e.g. in uint8 case)

  return vx;
}

VX_API_ENTRY vx_status VX_API_CALL vxCopyMatrix(vx_matrix mat, void *user_ptr,
                                                vx_enum usage,
                                                vx_enum user_mem_type) {
  if (usage != VX_WRITE_ONLY) return VX_FAILURE;
  if (user_mem_type != VX_MEMORY_TYPE_HOST) return VX_FAILURE;

  DomVX::VX_Matrix *matrix = ((DomVX::VX_Matrix *)(mat->o));

  switch (matrix->data_type) {
    case VX_TYPE_UINT8:
      std::memcpy(matrix->mat.data(), user_ptr, matrix->columns * matrix->rows);
      break;
    case VX_TYPE_INT16:
      std::memcpy(matrix->mat.data(), user_ptr,
                  matrix->columns * matrix->rows * 2);
      break;
    case VX_TYPE_INT32:
    case VX_TYPE_FLOAT32:
      std::memcpy(matrix->mat.data(), user_ptr,
                  matrix->columns * matrix->rows * 4);
      break;
    default:
      return VX_FAILURE;
  }

  return VX_SUCCESS;
}

// Object: Scalar
VX_API_ENTRY vx_scalar VX_API_CALL vxCreateScalar(vx_context context,
                                                  vx_enum data_type,
                                                  const void *ptr) {
  if (data_type <= VX_TYPE_INVALID || data_type >= VX_TYPE_SCALAR)
    return nullptr;
  auto scalar = new DomVX::Scalar((DomVX::ObjectType)data_type, ptr);
  auto vx = new _vx_scalar();
  vx->o = scalar;
  return vx;
}

VX_API_ENTRY vx_status VX_API_CALL vxReleaseScalar(vx_scalar *scalar) {
  return 0;
}

// Object: Threshold
VX_API_ENTRY vx_threshold VX_API_CALL
vxCreateThresholdForImage(vx_context context, vx_enum thresh_type,
                          vx_df_image input_format, vx_df_image output_format) {
  DomVX::Threshold *t = new DomVX::Threshold;
  auto vx = new _vx_threshold();
  vx->o = t;

  t->threshold_type = (vx_threshold_type_e)thresh_type;
  t->input_format = input_format;
  t->output_format = output_format;

  return vx;
}

VX_API_ENTRY vx_status VX_API_CALL vxSetThresholdAttribute(vx_threshold th,
                                                           vx_enum attribute,
                                                           const void *ptr,
                                                           vx_size size) {
  vx_int32 *int32_ptr = (vx_int32 *)ptr;

  auto thresh = (DomVX::Threshold *)th->o;
  switch (attribute) {
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

VX_API_ENTRY vx_status VX_API_CALL vxAddParameterToKernel(vx_kernel kernel,
                                                          vx_uint32 index,
                                                          vx_enum dir,
                                                          vx_enum data_type,
                                                          vx_enum state) {
  DomVX::HipaccKernel *hk = ((DomVX::HipaccKernel *)(kernel->o));
  if (index >= hk->direction.size()) {
    hk->direction.resize(index + 1);
    hk->type.resize(index + 1);
  }

  hk->direction[index] = (vx_direction_e)dir;
  hk->type[index] = (vx_type_e)data_type;
  return VX_SUCCESS;
}

VX_API_ENTRY vx_status VX_API_CALL vxFinalizeKernel(vx_kernel kernel) {}

VX_API_ENTRY vx_node VX_API_CALL vxCreateGenericNode(vx_graph graph,
                                                     vx_kernel kernel) {
  DomVX::CustomKernel *ck = dynamic_cast<DomVX::CustomKernel *>(kernel->o);
  if (ck == nullptr) return nullptr;
  auto cn = new DomVX::CustomNode();
  auto vx = new _vx_node();
  vx->o = cn;
  cn->graph = dynamic_cast<DomVX::Graph *>(graph->o);
  cn->graph->refs[(DomVX::Node *)cn] =
      cn->graph->dag->add_vertex(*(DomVX::Node *)vx->o);
  cn->kernel = ck;
  cn->parameters.resize(ck->direction.size());
  return vx;
}
VX_API_ENTRY vx_status VX_API_CALL vxSetParameterByIndex(vx_node node,
                                                         vx_uint32 index,
                                                         vx_reference value) {
  DomVX::CustomNode *hn = dynamic_cast<DomVX::CustomNode *>(node->o);
  if (hn == nullptr) return VX_FAILURE;
  if (index >= hn->kernel->direction.size()) return VX_FAILURE;

  hn->parameters[index] = value;
  DomVX::Node *hn_node = (DomVX::Node *)hn;
  DomVX::Node *value_node = (DomVX::Node *)value->o;
  if (hn->graph->refs.find(value_node) == hn->graph->refs.end())
    hn->graph->refs[value_node] = hn->graph->dag->add_vertex(*value->o);

  if (hn->kernel->direction[index] == VX_INPUT)
    hn->graph->dag->add_edge(hn->graph->refs[value_node],
                             hn->graph->refs[hn_node]);
  else
    hn->graph->dag->add_edge(hn->graph->refs[hn_node],
                             hn->graph->refs[value_node]);

  // small hack
  //  if (index == 0)
  //    hn->graph->dag->outputs.push_back(hn->graph->refs[value_node]);
  //  else if (index == 1)
  //    hn->graph->dag->inputs.push_back(hn->graph->refs[value_node]);

  return VX_SUCCESS;
}
