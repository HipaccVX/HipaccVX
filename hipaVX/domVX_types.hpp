#pragma once

#include "../VX/vx.h"
#include "../VX/vx_types.h"
#include "ast.hpp"

#include <string>
#include <vector>

ast4vx::Datatype convert_type(vx_df_image type);

namespace DomVX {
/// @brief Operator Types
enum class OperatorType {
  None,
  PointOperator,
  LocalOperator,
  GlobalOperator,
};

enum class ObjectTask {
  Computation,
  Buffer,
  API,
  Invalid
};

using ObjectType = vx_type_e;

ObjectTask set_task_from_type(ObjectType type);


class Object {
 public:
  Object() : my_id(next_id++){}

  virtual ~Object() = default;

  Object* get() { return _obj; }

  std::string id() { return std::to_string(my_id); }

  std::string get_name() { return this->name; }

  ObjectType type() { return this->obj_type; }

  ObjectTask task() { return obj_task; }

  bool is_virtual() { return virt; }

  bool is_alive() { return alive; }

  void is_virtual(bool _virt) { virt = _virt; }

  void is_alive(bool _alive) { alive = _alive; }

  void set_name(std::string _name = "Object") {
    name = _name + std::to_string(my_id);
  }

 protected:
  Object* _obj;

  static int next_id;

  int my_id;

  std::string name;

  bool virt = false;

  bool alive = false;

  ObjectType obj_type;

  ObjectTask obj_task = ObjectTask::Invalid;

  void set_type(ObjectType _obj_type) { obj_type = _obj_type; }

  void set_task() { obj_task = set_task_from_type(type()); }

  void bind(Object* _ptr) { _obj = _ptr; }
};


class Node : public Object {
 protected:
 public:
  std::string node_name;  // remove me

  Node() {
    set_name("Node");
    init();
  }

  Node(std::string _name) {
    set_name(_name);
    init();
  }

  void init() {
    set_type(VX_TYPE_NODE);
    set_task();
    bind(this);
  }

  virtual ~Node() = default;

  std::vector<Node *> subnodes;
  std::vector<Object *> inputs;
  std::vector<Object *> outputs;

  vx_border_e border_mode = VX_BORDER_UNDEFINED;
  std::shared_ptr<DomVX::Node> kernel = nullptr;

  DomVX::OperatorType operator_type = DomVX::OperatorType::None;

  virtual void build() {}
};


class Convolution : public Object {
 public:
  Convolution() {
    set_type(VX_TYPE_CONVOLUTION);
    set_name("Convolve");
    set_task();
    bind(this);
  }

  std::vector<vx_int16> coefficients;
  vx_size rows;
  vx_size columns;
  vx_uint32 scale;
};


class Image : public Object {
 public:
  Image() { init(); }

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
    set_type(VX_TYPE_IMAGE);
    col = VX_TYPE_DF_IMAGE;
    set_task();
    set_name("Img");
    bind(this);
  }

  vx_uint32 get_width() { return w; }
  vx_uint32 get_height() { return h; }

  virtual ~Image() = default;

  vx_uint32 w, h;
  vx_df_image col;

  vx_df_image get_dtype() { return col; }

  void set_dtype(vx_df_image type) { col = type; }
};


class Scalar : public Object {
 public:
  Scalar(ObjectType t, const void *ptr) : data_type(t) {
    set_name("Scalar");
    init();
    set_value(ptr);
  }

  void init() {
    set_type(VX_TYPE_SCALAR);
    set_task();
    bind(this);
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


class Array : public Object {
 public:
  Array(vx_enum _dtype, vx_size _size, vx_size _rows)
      : data_type(_dtype), size(_size), rows(_rows) {
    init();
    set_name("Arr");
  }

  void init() {
    set_type(VX_TYPE_ARRAY);
    set_task();
  }

  virtual ~Array() = default;
  vx_enum data_type;
  vx_size size;
  vx_size rows;
};


class Threshold : public Object {
 public:
  Threshold() {
    set_type(VX_TYPE_THRESHOLD);
    set_name("Threshold");
    set_task();
    bind(this);
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
    set_type(VX_TYPE_MATRIX);
    set_name("Matrix");
    set_task();
    bind(this);
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
  std::vector<Image*> images;
  std::vector<Array*> arrays;
  std::vector<Graph*> graphs;
};

// ------------   TODO: fix these acccording to OpenVX definition
struct ROI {
  int x;
  int y;
  int width;
  int height;
};

enum class BorderM { Undef, Constant, Replicate };

class Acc {
 public:
  int my_id;
  Image *im;
  std::string name;

  bool is_acc;  // TODO: change this with type IS or ACC
  bool is_is;
  bool isRoiSet;
  bool isInterpSet;

  ROI roi;
  BorderM brdr;

 public:
  void set_name() {
    std::string _name;
    if (is_acc == true) {
      _name = "_acc_";
    } else if (is_is == true) {
      _name = "_is_";
    } else {
      _name = "_undef_";
    };

    name = im->get_name() + _name + std::to_string(my_id);
  }

  void init() {
    isRoiSet = false;
    isInterpSet = false;
    roi = ROI{0, 0, 0, 0};
    brdr = BorderM::Undef;
    im = NULL;
    name = std::to_string(my_id) + "_Acc_undef";
  }

  Acc() : my_id(next_id++) { init(); }

  Acc(Image *_im, bool _is_acc) : my_id(next_id++) {
    init();
    set_img(_im);
    is_acc = _is_acc;
    is_is = !_is_acc;
  }

  std::string get_name() { return name; }

  bool isImgSet() { return !(im == NULL); }

  void set_img(Image *_im) { im = _im; }

  void set_as_is() {
    is_is = true;
    is_acc = false;
  }

  void set_as_acc() {
    is_is = false;
    is_acc = true;
  }

  unsigned width() { return im->get_width(); }

  unsigned height() { return im->get_height(); }

  static int next_id;
};

}  // namespace DomVX

struct _vx_reference {
  DomVX::Object *o;
};

struct _vx_scalar {
  DomVX::Object *o;
};

struct _vx_kernel {
  DomVX::Object *o;
};

struct _vx_node {
  DomVX::Object *o;
};

struct _vx_graph {
  DomVX::Object *o;
};

struct _vx_context {
  DomVX::Object *o;
};

struct _vx_matrix {
  DomVX::Object *o;
};

struct _vx_threshold {
  DomVX::Object *o;
};

struct _vx_convolution {
  DomVX::Object *o;
};

struct _vx_array {
  DomVX::Object *o;
};

struct _vx_image {
  DomVX::Object *o;
};
