#pragma once

#include "../graph.hpp"
#include "../domVX_types.hpp"
#include "../../VX/vx.h"
#include "cassert"

//#define ERROR(cond, msg) std::assert((cond) && (msg));
//#define ERRORM(msg) static_assert(true, msg);
#define ERRORM(msg) throw std::runtime_error((msg));
  

using graphVX::dag;
using graphVX::VertexDesc;
using graphVX::VertexType;
using graphVX::OptGraphT;
using HipaVX::VertexTask;
using HipaVX::ObjectType;

using HipaccNode = HipaVX::Object;
using HipaccImage = HipaVX::Image;

using HipaccDataType = vx_df_image; 

// vx_df_image to HipaccWriter
#define U8  VX_DF_IMAGE_U8
#define S16 VX_DF_IMAGE_S16
#define U16 VX_DF_IMAGE_U16
#define S32 VX_DF_IMAGE_S32
#define U32 VX_DF_IMAGE_U32
#define F32 VX_TYPE_FLOAT32
  
#define HipaccImageE VX_TYPE_IMAGE

struct ROI {
  int x; 
  int y; 
  int width; 
  int height; 
};

class HipaccAccessor {
 public:
  HipaccImage* im;
  std::string name;
  HipaccDataType dtype;

  bool isRoiSet;
  bool isInterpSet;

  void init() {
    isRoiSet = false;
    isInterpSet = false;
    //roi = ROI{ x = 0, u = 0, width = 0, height = 0}; 
  }

  ROI roi;
  int width()  { return im->get_width(); };

  int height() { return im->get_height(); };

  HipaccAccessor( ) {
    init();
  };

};


class hipacc_writer {
 private:
//  std::string define_accessor();
//
//  std::string define_border_handling();
//
//  std::string define_mask();
//
//  std::string define_iteration_space();
//
//  std::string define_kernel();
//
//  std::string define_kernel_call();
//
  
 public:
  std::stringstream ss, ss_im;

  template<class T> std::string name(T* node);

  template<class T> std::string dtype(T* node);

  std::string _dtype(HipaccDataType type, std::string name = "");


  void def(HipaccNode*);

  void def(std::stringstream &ss, HipaccImage* n);

  void def(std::stringstream &ss, HipaccAccessor* n);

  std::string param(HipaccNode);

 public:
  hipacc_writer() {};

  std::string dump_code();

  std::string initial_includes();

//  std::string get_kernel_declaration();

  std::string get_kernel_call() { return "kernel"; };
  

  std::string get_space_definition() { return "kernel"; };

};


// ------------------------ public API ---------------------------------------
std::string hipacc_writer::dump_code() {
  ss << initial_includes();
   return ss.str();
}


// ----------------------- VX to Hipacc ---------------------------------------
void hipacc_writer::def(HipaccNode* hn) {
  switch (hn->type) {
    case HipaccImageE: {
      HipaccImage* n = dynamic_cast<HipaccImage*>(hn);
      def(ss_im, n);
      break;
    }

    // case VX_TYPE_INVALID:
    //   ERRORM("hipacc_writer::def : invalid type");
    //   break;

    // case VX_TYPE_NODE:
    // case VX_TYPE_KERNEL:
    // case VX_TYPE_CONVOLUTION:
    // case VX_TYPE_SCALAR:
    // case VX_TYPE_ARRAY:
    // case VX_TYPE_MATRIX:
    default: {
      ERRORM("hipacc_writer::def : type of the HipaccNode" + hn->get_name() + "is not defined");
      break;
    }
  }
}


// ----------------- conversion to string -------------------------------------
std::string hipacc_writer::_dtype(HipaccDataType type, std::string name) {
  switch (type) {
    case U8:  { return "uchar"; }
    case S16: { return "short"; }
    case U16: { return "unsigned short"; }
    case S32: { return "int"; }
    case U32: { return "uint"; }
    case F32: { return "float"; }
    default: { ERRORM("hipac_writer::dtype, unknown dtype for " + name); }
  }
}

template<class T>
std::string hipacc_writer::dtype(T* n) {
  return _dtype(n->get_dtype(), n->get_name());
}

template<class T>
std::string hipacc_writer::name(T* n) {
  std::string ss = n->get_name();

  if (ss.empty()) ERRORM("hipacc_writer::name : returns an empty string");

  return ss;
}



// ----------------- basic hipacc stuff ---------------------------------------
std::string hipacc_writer::initial_includes() {
  std::string out;

  out += "#include <algorithm>\n";
  out += "#include <cmath>\n";
  out += "#include <cstdlib>\n";
  out += "#include <vector>\n";
  out += "#include \"hipacc.hpp\"\n\n";

  out += "using namespace hipacc;\n";
  out += "using namespace hipacc::math;\n";

  return out;
}

void hipacc_writer::def(std::stringstream &ss, HipaccImage* n) {
  ss << "Image" << "<" << dtype(n) + "> " << name(n)
                << "(" << n->get_width() << ", " << n->get_height() << ");\n";
}

void hipacc_writer::def(std::stringstream &ss, HipaccAccessor* n) {
  std::stringstream params;
  params << n->width() << ", " << n->height();

  if(n->isRoiSet) {
    params << "," << n->roi.x << ", "  << n->roi.y; 
  }

  if(n->isInterpSet) {
    params << "more";
  }

  ss << "Accessor" << "<" << dtype(n) + "> " << name(n) << "(" << params.str() << ");\n";
}






// std::string hipacc_graph_gen::get_kernel_declaration(ObjectType) {
// }


class graph_gen {

 public:
  graph_gen(dag &_g_dag) : g_dag(_g_dag) {};

  void init();

  std::string dump();

  void traverse_nodes();

  VertexType& get_vert(VertexDesc& v) { return (*_g_opt)[v]; }

  VertexTask get_vert_task(VertexDesc& v) { return (*_g_opt)[v].get_task(); }

  ObjectType get_vert_type(VertexDesc& v) { return (*_g_opt)[v].type; }

 private:
    std::string code;

    dag g_dag;
    OptGraphT* _g_opt;

    std::list<VertexDesc>* _verts;
    std::list<VertexDesc> nodes;
    std::list<VertexDesc> spaces;
};

void graph_gen::init() {
  _g_opt = g_dag._g_opt;

  _verts = g_dag.set_order_of_exec();

  for( auto i : *_verts) {
    switch ((*_g_opt)[i].get_task()) {
      case VertexTask::Buffer:
        spaces.push_back(i);
        break;
      case VertexTask::Computation:
        nodes.push_back(i);
        break;
      default:
        std::cout << "Warning: I have reached nodes where VertexTask = None\n";
        break;
    }
  }
}

// std::string graph_gen::dump() {
//   code << initial_includes();
//   return code; 
// }
//

