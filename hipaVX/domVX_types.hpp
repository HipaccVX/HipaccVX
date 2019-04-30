#pragma once

#include "../VX/vx.h"
#include "../VX/vx_types.h"
#include "ast.hpp"

#include <string>
#include <vector>

ast4vx::Datatype convert_type(vx_df_image type);

namespace DomVX {
/**
 * @brief The different types of Abstractio Nodes in DomVX
 */
enum class OperatorType
{
    None,
    Map,
    LocalOperation,
    GlobalOperation,
};

/**
 * @brief The fundamental DomVX AbstractionNode class
 */
class AbstractionNode
{
public:
  int my_id; /**< This gets set when constructing a new Node Instance. This is read only */
  AbstractionNode() {
      static int next_id = 0;
      my_id = next_id++;
      set_name();
  }

  std::string name;

  void set_name(std::string _name = "Object") {
    name = _name + std::to_string(my_id);
  }

  std::string get_name() {
    return this->name;
  }

  virtual ~AbstractionNode() = default;
};

class OperatorNode : public AbstractionNode {
  public:
  DomVX::OperatorType operator_type = DomVX::OperatorType::None;

  void init() {
    set_name("operator");
  }

  OperatorNode() {
    init();
  };
};

}



namespace HipaVX {

using ObjectType = vx_type_e;

enum class VertexTask { Computation, Buffer, API, Invalid};

VertexTask set_task_from_type(ObjectType type);

class Object {
 public:
  Object() : my_id(next_id++) {};

  virtual ~Object() = default;

  bool is_virtual() { return virt; };

  VertexTask get_task() { return task; }

  void set_task(VertexTask _task) { task = _task; }

  void set_task() { task = set_task_from_type(type); }

  void set_name(std::string _name = "Object") {
    name = _name + std::to_string(my_id);
  }

  std::string get_name() {
    return this->name;
  }

  ObjectType get_type() {
    return this->type;
  };

 public: // TODO: make these protected
  int my_id;

  ObjectType type;

  Object* _obj;

  bool virt;

  bool alive = false; // after optimizations

  std::string name;

  VertexTask task = VertexTask::Invalid;

 protected:
  static int next_id;
};


class Scalar : public Object {
 public:
  Scalar(ObjectType t, const void *ptr) : data_type(t) {
    set_name("Scalar");
    init();
    set_value(ptr);
  }

  void init() {
    type = VX_TYPE_SCALAR;
    set_task();
    _obj = this;
  }

  void set_value(const void *ptr) {
    switch (data_type) {
      case VX_TYPE_INT32:
        i32 = *((vx_int32 *)ptr);
        break;
      case VX_TYPE_FLOAT32:
        f32 = *((vx_float32 *)ptr);
        break;
      default:
        throw std::runtime_error("Unsupported vx_type_e in Scalar constructor");
    }
  }

  ObjectType data_type;
  union {
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


class Image : public Object {
 public:
  Image(){
    init();
  };

  Image(std::string _name) {
    set_name(_name);
    init();
  }

  Image(vx_uint32 width, vx_uint32 height) : w(width), h(height) {
    init();
    w = width;
    h = height;
  }

  Image(vx_uint32 width, vx_uint32 height, vx_df_image color) {
    init();
    w = width;
    h = height;
    col = color;
  }

  void init() {
    type = VX_TYPE_IMAGE;
    col = VX_TYPE_DF_IMAGE;
    set_task();
    set_name("Img");
    _obj = this;
  }

  vx_uint32 get_width()  { return w;};
  vx_uint32 get_height() { return h;};

  virtual ~Image() = default;

  vx_uint32 w, h;
  vx_df_image col;

  vx_df_image get_dtype() { return col; }

  void set_dtype(vx_df_image type) { col = type; }
};


class Array : public Image {
 public:
  Array(vx_enum item_type, vx_size cap, vx_size rows) :
      Image(rows, cap, VX_DF_IMAGE_S32), type(item_type), capacity(cap) {
    init();
    set_name("Arr");
  }

  void init() {
    type = VX_TYPE_ARRAY;
    set_task();
  }

  virtual ~Array() = default;
  vx_enum type;
  vx_size capacity;
};


class Node : public Object {
 protected:

 public:
  std::string node_name; //remove me

  Node() {
    set_name("Node");
    init();
  }

  Node(std::string _name) {
    set_name(_name);
    init();
  }

  void init() {
    kernel = std::make_shared<DomVX::OperatorNode>();
    type = VX_TYPE_NODE;
    set_task();
    _obj = this;
  }

  virtual ~Node() = default;

  std::vector<Node *> subnodes;
  std::vector<Object *> inputs;
  std::vector<Object *> outputs;

  vx_border_e border_mode = VX_BORDER_UNDEFINED;
  std::shared_ptr<DomVX::OperatorNode> kernel;

  virtual void build() {}
};


class Convolution : public Object {
 public:
  Convolution() {
    type = VX_TYPE_CONVOLUTION;
    set_name("Convolve");
    set_task();
    _obj = this;
  }

  std::vector<vx_int16> coefficients;
  vx_size rows;
  vx_size columns;
  vx_uint32 scale;
};


class Threshold : public Object {
 public:
  Threshold() {
    type = VX_TYPE_THRESHOLD;
    set_name("Threshold");
    set_task();
    _obj = this;
  }

  vx_threshold_type_e threshold_type;

  vx_int32 value;
  vx_int32 lower;
  vx_int32 upper;

  vx_int32 true_value = 255;
  vx_int32 false_value = 0;

  vx_df_image input_format;
  vx_df_image output_format;
};


class VX_Matrix : public Object {
 public:
  VX_Matrix() {
    type = VX_TYPE_MATRIX;
    set_name("Matrix");
    set_task();
    _obj = this;
  }

  vx_enum data_type;
  vx_size rows;
  vx_size columns;
  std::vector<u_char> mat;
};


class Graph : public Object {
 public:
  std::vector<Node *> graph;
  bool built = false;

  void build();
};


class Context : public Object {
 public:
  std::vector<Image *> images;
  std::vector<Graph *> graphs;
};


// ------------   TODO: fix these acccording to OpenVX definition
struct ROI {
  int x;
  int y;
  int width;
  int height;
};

enum class BorderM {
  Undef,
  Constant,
  Replicate
};

class Acc {
 public:
  int my_id;
  Image* im;
  std::string name;

  bool is_acc; // TODO: change this with type IS or ACC
  bool isRoiSet;
  bool isInterpSet;

  ROI roi;
  BorderM brdr;

 public:
  void set_name() {
    std::string _name;
    if (is_acc == true) { _name = "_acc_"; }
    else { _name = "_is_"; };

    name = im->get_name() + _name + std::to_string(my_id);
  };

  void init() {
    isRoiSet = false;
    isInterpSet = false;
    roi = ROI{ 0, 0, 0, 0};
    brdr = BorderM::Undef;
    im = NULL;
    name = std::to_string(my_id) + "_Acc_undef" ;
  }

  Acc() : my_id(next_id++) {
    init();
  };

  std::string get_name() { return name; };

  bool isImgSet() { return !(im == NULL); };

  void set_img(Image* _im) { im = _im; };

  int width()  { return im->get_width(); };

  int height() { return im->get_height(); };

  static int next_id;
};

}  // namespace HipaVX



struct _vx_reference {
  HipaVX::Object *o;
};

struct _vx_scalar {
  HipaVX::Object *o;
};

struct _vx_kernel {
  HipaVX::Object *o;
};

struct _vx_node {
  HipaVX::Object *o;
};

struct _vx_graph {
  HipaVX::Object *o;
};

struct _vx_context {
  HipaVX::Object *o;
};

struct _vx_matrix {
  HipaVX::Object *o;
};

struct _vx_threshold {
  HipaVX::Object *o;
};

struct _vx_convolution {
  HipaVX::Object *o;
};

struct _vx_array {
  HipaVX::Object *o;
};

struct _vx_image {
  HipaVX::Object *o;
};
