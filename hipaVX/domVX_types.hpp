#pragma once

#include "../VX/vx.h"
#include "../VX/vx_types.h"
#include "ast.hpp"

#include <string>
#include <vector>

function_ast::Datatype convert_type(vx_df_image type);

namespace HipaVX {

using ObjectType = vx_type_e;

enum class VertexTask { Computation, Buffer, API, Invalid};

VertexTask set_task_from_type(ObjectType type);



class Object {
 public:
  Object() : my_id(next_id++) {};

  bool is_virtual() { return virt; };

  VertexTask get_task() { return task; }

  void set_task(VertexTask _task) { task = _task; }

  void set_task() { task = set_task_from_type(type); }

  void set_name(std::string _name = "Object") {
    name = _name + std::to_string(my_id);
  }

  std::string get_name() {
    return name;
  }

 public:
  // TODO: make these protected
  const int my_id;

  ObjectType type;

 protected:
  static int next_id;

  std::string name;

  VertexTask task = VertexTask::Invalid;

  bool virt;
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
    set_name("Img");
    init();
  };

  Image(std::string _name) {
    set_name(_name);
    init();
  }

  Image(vx_uint32 width, vx_uint32 height, vx_df_image color) :
      w(width), h(height), col(color) {
    init();
  }

  void init() {
    type = VX_TYPE_IMAGE;
    set_task();
  }

  virtual ~Image() = default;
  vx_uint32 w = 0, h = 0;
  vx_df_image col;
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
    kernel = std::make_shared<function_ast::ForEveryPixel>();
    type = VX_TYPE_NODE;
    set_task();
  }

  virtual ~Node() = default;

  std::vector<Node *> subnodes;
  std::vector<Object *> inputs;
  std::vector<Object *> outputs;

  vx_border_e border_mode = VX_BORDER_UNDEFINED;
  std::shared_ptr<function_ast::ForEveryPixel> kernel;

  virtual void build() {}
};


class Convolution : public Object {
 public:
  Convolution() { 
    type = VX_TYPE_CONVOLUTION; 
    set_name("Convolve");
    set_task();
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
