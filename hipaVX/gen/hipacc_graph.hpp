#pragma once

#include "../dsl/abstractions.hpp"
#include "../dsl/types.hpp"
#include "../graph/graph.hpp"
#include "../vx_adaptor.hpp"
#include "gen_template.hpp"
#include <cassert>
//#include <iterator>
//#include "infix_iterator.h"

//#define ERROR(cond, msg) std::assert((cond) && (msg));
//#define ERRORM(msg) static_assert(true, msg);
#define ERRORM(msg) throw std::runtime_error((msg));

using DomVX::ObjectTask;
using DomVX::ObjectType;
using graphVX::dag;
using graphVX::EdgeDesc;
using graphVX::EdgeType;
using graphVX::OptGraphT;
using graphVX::VertexDesc;
using graphVX::VertexType;

using HipaccImage = DomVX::Image;
using HipaccScalar = DomVX::Scalar;
using HipaccMatrix = DomVX::VX_Matrix;
using Array = DomVX::Array;
using Bool = DomVX::Bool;
using DomVXAcc = DomVX::Acc;

using HipaccAccessor = DomVXAcc;
using HipaccIterationSpace = DomVXAcc;

using HipaccDataType = vx_df_image;

using CustomNode = DomVX::CustomNode;

// vx_df_image to HipaccWriter
#define U8 VX_DF_IMAGE_U8
#define S16 VX_DF_IMAGE_S16
#define U16 VX_DF_IMAGE_U16
#define S32 VX_DF_IMAGE_S32
#define U32 VX_DF_IMAGE_U32
#define RGBA VX_DF_IMAGE_RGBX
#define UYVY VX_DF_IMAGE_UYVY
#define F32 VX_TYPE_FLOAT32
#define UNDEF VX_TYPE_DF_IMAGE

#define HipaccImageE VX_TYPE_IMAGE
#define HipaccScalarE VX_TYPE_SCALAR
#define HipaccMatrixE VX_TYPE_MATRIX
#define HipaccArrayE VX_TYPE_ARRAY
#define HipaccBoolE VX_TYPE_BOOL

enum class DefType {
  Hdecl,
  Kdecl,
  Param,
};

// pointer casts
HipaccImage* obj2img(VertexType* v) {
  HipaccImage* _im = dynamic_cast<HipaccImage*>(v->get());

  if (_im == nullptr)
    ERRORM("graph_gen obj2img, dynamic cast fail for: " + v->name())

  return _im;
}
// pointer casts
HipaccScalar* obj2scalar(VertexType* v) {
  HipaccScalar* _sc = dynamic_cast<HipaccScalar*>(v->get());

  if (_sc == nullptr)
    ERRORM("graph_gen obj2scalar, dynamic cast fail for: " + v->name())

  return _sc;
}
// pointer casts
HipaccMatrix* obj2matrix(VertexType* v) {
  HipaccMatrix* _mat = dynamic_cast<HipaccMatrix*>(v->get());

  if (_mat == nullptr)
    ERRORM("graph_gen obj2matrix, dynamic cast fail for: " + v->name())

  return _mat;
}
// pointer casts
Array* obj2array(VertexType* v) {
  Array* _arr = dynamic_cast<Array*>(v->get());

  if (_arr == nullptr)
    ERRORM("graph_gen obj2array, dynamic cast fail for: " + v->name())

  return _arr;
}

class hipacc_writer {
 protected:
  std::string hind = " ";
  std::string sind = hind + hind;
  std::string dind = sind + sind;
  std::string tind = dind + sind;

  // Set to keep track if domains or masks need to be created or if an "old" one
  // can be used instead
  std::set<std::string> domains;
  std::set<std::string> masks;

 public:
  std::stringstream ss;

  template <class T>
  std::string name(T* node);

  template <class T>
  std::string dtype(T* node);

  std::string _dtype(HipaccDataType type, std::string name = "");

  void def(VertexType*);

  void def(std::stringstream& ss, HipaccImage* n,
           DefType deftype = DefType::Hdecl);

  void def(std::stringstream& ss, HipaccScalar* n,
           DefType deftype = DefType::Hdecl);

  void def(std::stringstream& ss, HipaccMatrix* n,
           DefType deftype = DefType::Hdecl);

  void def(std::stringstream& ss, Array* n, DefType deftype = DefType::Hdecl);

  void def_acc(std::stringstream& ss, DomVXAcc* n, DefType deftype,
               std::string dom_name);

  void def_is(std::stringstream& ss, DomVXAcc* is,
              DefType deftype = DefType::Hdecl);

  void def_dom(std::stringstream& ss, HipaccMatrix* is,
               DefType deftype = DefType::Hdecl);

  void def_mask(std::stringstream& ss, HipaccMatrix* is,
                DefType deftype = DefType::Hdecl);

  void def(std::stringstream& ss, CustomNode* kern,
           DefType deftype = DefType::Hdecl,
           std::vector<HipaccAccessor*> acc_l = {},
           std::vector<HipaccIterationSpace*> is_l = {},
           std::vector<HipaccMatrix*> matrix_l = {},
           std::vector<HipaccScalar*> scalar_l = {});

 public:
  hipacc_writer() = default;

  std::string initial_includes();
};

// ----------------- conversion to string -------------------------------------
std::string hipacc_writer::_dtype(HipaccDataType type, std::string name) {
  switch (type) {
    case U8: {
      return "uchar";
    }
    case S16: {
      return "short";
    }
    case U16: {
      return "unsigned short";
    }
    case S32: {
      return "int";
    }
    case U32: {
      return "uint";
    }
    case F32: {
      return "float";
    }
    case RGBA: {
      return "uchar4";
    }
    case UYVY: {
      return "uchar4";
    }
    case UNDEF: {
      return "undef";
    }
    default: {
      ERRORM("hipac_writer::dtype, unknown dtype for " + name)
    }
  }
}

template <class T>
std::string hipacc_writer::dtype(T* n) {
  return _dtype(n->get_dtype(), n->name());
}

template <class T>
std::string hipacc_writer::name(T* n) {
  std::string ss = n->name();

  if (ss.empty()) ERRORM("hipacc_writer::name : returns an empty string")

  return ss;
}

// ----------------- basic hipacc stuff ---------------------------------------
std::string hipacc_writer::initial_includes() {
  std::string out;

  out += "#include <algorithm>\n";
  out += "#include <cmath>\n";
  out += "#include <cstdlib>\n";
  out += "#include <vector>\n";
  out += "#include \"hipacc_helper.hpp\"\n";
  out += "#include \"hipacc.hpp\"\n\n";

  out += "using namespace hipacc;\n";
  out += "using namespace hipacc::math;\n";
  out += "\n";

  out += R"(
struct Keypoint {
    int32_t x;
    int32_t y;
    float strength;
    float scale;
    float orientation;
    int32_t tracking_status;
    float error;
};
)";

  return out;
}

void hipacc_writer::def(std::stringstream& ss, HipaccImage* img,
                        DefType deftype) {
  switch (deftype) {
    case DefType::Hdecl: {
      auto in_image = dynamic_cast<DomVX::FileinputImage*>(img);
      if (in_image != nullptr) {
        if (in_image->col == VX_DF_IMAGE_RGBX) {
          ss << dind << dtype(img) << " *" << name(img) << "_input = ("
             << dtype(img) << "*) load_data<uchar>(" << img->get_width() << ", "
             << img->get_height() << ", 4, \"" << in_image->file << "\");\n";
        } else if (in_image->col == VX_DF_IMAGE_UYVY) {
          ss << dind << dtype(img) << " *" << name(img) << "_input = ("
             << dtype(img) << "*) load_data<uchar>(" << img->get_width() << ", "
             << img->get_height() << ", 4, \"" << in_image->file << "\");\n";
        } else {
          ss << dind << dtype(img) << " *" << name(img) << "_input = load_data<"
             << dtype(img) << ">(" << img->get_width() << ", "
             << img->get_height() << ", 1, \"" << in_image->file << "\");\n";
        }
        ss << dind << "Image"
           << "<" << dtype(img) + "> " << name(img) << "(" << img->get_width()
           << ", " << img->get_height() << ", " << name(img) << "_input);\n";
      } else {
        ss << dind << "Image"
           << "<" << dtype(img) + "> " << name(img) << "(" << img->get_width()
           << ", " << img->get_height() << ");\n";
      }
      break;
    }

    default: {
      ERRORM("Unsupported deftype @ def(HipaccImage)")
    }
  }
}

void hipacc_writer::def(std::stringstream& ss, HipaccScalar* sc,
                        DefType deftype) {
  switch (deftype) {
    case DefType::Hdecl: {
      std::string type;
      std::string value;
      switch (sc->data_type) {
        case VX_TYPE_INT32:
          type = "int";
          value = std::to_string(sc->i32);
          break;
        case VX_TYPE_FLOAT32:
          type = "float";
          value = std::to_string(sc->f32);
          break;
        case VX_TYPE_UINT8:
          type = "unsigned char";
          value = std::to_string(sc->ui8);
          break;
        case VX_TYPE_UINT32:
          type = "unsigned int";
          value = std::to_string(sc->ui32);
          break;
        default:
          ERRORM("Unsupported data_type @ def(HipaccScalar)")
      }
      ss << dind << type << " " << name(sc) << " = " << value << ";\n";
      break;
    }

    default: {
      ERRORM("Unsupported deftype @ def(image)")
    }
  }
}

void hipacc_writer::def(std::stringstream& ss, HipaccMatrix* mat,
                        DefType deftype) {
  switch (deftype) {
    case DefType::Hdecl: {
      std::string type;
      switch (mat->data_type) {
        case VX_TYPE_INT32:
          type = "int";
          break;
        case VX_TYPE_FLOAT32:
          type = "float";
          break;
        case VX_TYPE_UINT8:
          type = "unsigned char";
          break;
        case VX_TYPE_INT16:
          type = "short";
          break;
        default:
          ERRORM("Unsupported data_type @ def(HipaccMatrix)")
      }
      std::string mat_name = name(mat);
      std::string ind = dind;

      // define an array for the values
      ss << ind << type << " " << mat_name << "[" << mat->rows << "]["
         << mat->columns << "]"
         << " = {\n";
      for (unsigned y = 0; y < mat->rows; y++) {
        ss << ind << sind << "{";
        for (unsigned x = 0; x < mat->columns; x++) {
          switch (mat->data_type) {
            case VX_TYPE_INT32:
              ss << *reinterpret_cast<int32_t*>(mat->mat.data() +
                                                (4 * (y * mat->columns + x)));
              break;
            case VX_TYPE_FLOAT32:
              ss << *reinterpret_cast<float*>(mat->mat.data() +
                                              (4 * (y * mat->columns + x)));
              break;
            case VX_TYPE_UINT8:
              ss << *reinterpret_cast<unsigned char*>(
                  mat->mat.data() + (1 * y * mat->columns + x));
              break;
            case VX_TYPE_INT16:
              ss << *reinterpret_cast<short*>(mat->mat.data() +
                                              (2 * (y * mat->columns + x)));
              break;
            default:
              ERRORM("Unsupported data_type @ def(HipaccMatrix)")
          }
          if (x == mat->columns - 1)
            ss << "}";
          else
            ss << ",";
        }
        if (y != mat->rows - 1) ss << ",\n";
      }
      ss << "};\n";

      break;
    }
    default: {
      ERRORM("Unsupported deftype @ def(HipaccMatrix)")
    }
  }
}

void hipacc_writer::def(std::stringstream& ss, Array* arr, DefType deftype) {
  switch (deftype) {
    case DefType::Hdecl: {
      std::string type;
      switch (arr->data_type) {
        case VX_TYPE_KEYPOINT:
          type = "Keypoint";
          break;
        default:
          ERRORM("Unsupported data_type @ def(Array)")
      }
      // define an array for the values
      ss << dind << type << " *" << name(arr) << " = new " << type << "["
         << arr->size << "];\n";
      break;
    }
    default: {
      ERRORM("Unsupported deftype @ def(Array)")
    }
  }
}

void hipacc_writer::def_acc(std::stringstream& ss, DomVXAcc* acc,
                            DefType deftype, std::string dom_name) {
  if (acc->isImgSet() == false) {
    ERRORM("hipacc_writer::def(acc) : acc " + acc->name() + " has no image")
  }

  switch (deftype) {
    case DefType::Hdecl: {
      // Not a local operation
      if (dom_name == "") {
        ss << dind << "Accessor"
           << "<" << dtype(acc->im) + "> " << name(acc) << "("
           << acc->im->name() << ");\n";
      } else {
        ss << dind << "BoundaryCondition"
           << "<" << dtype(acc->im) + "> " << name(acc) << "_bc"
           << "(" << acc->im->name() << "," << dom_name << ",Boundary::CLAMP"
           << ");\n";

        ss << dind << "Accessor"
           << "<" << dtype(acc->im) + "> " << name(acc) << "(" << name(acc)
           << "_bc"
           << ");\n";
      }
    } break;

    default: {
      ERRORM("Unsupported deftype @ def(acc)")
    }
  }
}

void hipacc_writer::def_is(std::stringstream& ss, DomVXAcc* is,
                           DefType deftype) {
  if (is->isImgSet() == false) {
    ERRORM("hipacc_writer::def(is) : is" + is->name() + " has no image")
  }

  switch (deftype) {
    case DefType::Hdecl: {
      std::stringstream params;
      params << is->im->name() << ", ";
      params << is->width() << ", " << is->height();

      if (is->isRoiSet) {
        params << "," << is->roi.x << ", " << is->roi.y;
      }

      if (is->isInterpSet) {
        params << "more";
      }

      ss << dind << "IterationSpace"
         << "<" << dtype(is->im) + "> " << name(is) << "(" << params.str()
         << ");\n";
      break;
    }

    case DefType::Kdecl:
    case DefType::Param: {
      ss << "IterationSpace"
         << "<" << dtype(is->im) + "> &" << name(is);
      break;
    }

    default: {
      ERRORM("Unsupported deftype @ def(image)")
    }
  }
}

// TODO: can have a base class for the parameter types, thus one initializer
// list
//       , which shortens the code significantly
void hipacc_writer::def(std::stringstream& ss, CustomNode* kern,
                        DefType deftype, std::vector<HipaccAccessor*> acc_l,
                        std::vector<HipaccIterationSpace*> is_l,
                        std::vector<HipaccMatrix*> matrix_l,
                        std::vector<HipaccScalar*> scalar_l) {
  if (is_l.size() != 1) {
    ERRORM("Hipacc Kernel" + name(kern) + " has " +
           std::to_string(is_l.size()) + " iteration spaces")
  }

  if (acc_l.size() < 1) {
    ERRORM("Hipacc Kernel" + name(kern) + " has " +
           std::to_string(acc_l.size()) + " accesors")
  }

  switch (deftype) {
    case DefType::Hdecl: {
      ss << dind;
      ss << name(kern) << "(";
      ss << name(*is_l.begin());

      for (auto acc : acc_l) {
        ss << ", " << name(acc);
      }

      for (auto mat : matrix_l) {
        ss << ", " << name(mat);
      }

      for (auto scalar : scalar_l) {
        ss << ", " << name(scalar);
      }

      // TODO: add other parameters
      ss << ");\n";
      break;
    }
    case DefType::Kdecl:
    default: {
      ERRORM("Unsupported deftype @ def(kern)")
    }
  }
}

void hipacc_writer::def_dom(std::stringstream& ss, HipaccMatrix* mat,
                            DefType deftype) {
  std::string dom_name = name(mat) + "_dom";
  if (domains.find(dom_name) != domains.end()) return;
  domains.insert(dom_name);

  switch (deftype) {
    case DefType::Hdecl: {
      std::string type;
      switch (mat->data_type) {
        case VX_TYPE_INT32:
          type = "int";
          break;
        case VX_TYPE_FLOAT32:
          type = "float";
          break;
        case VX_TYPE_UINT8:
          type = "unsigned char";
          break;
        case VX_TYPE_INT16:
          type = "short";
          break;
        default:
          ERRORM("Unsupported data_type @ def(HipaccMatrix)")
      }
      std::string mat_name = name(mat) + "_dom_val";
      std::string ind = dind;

      // define an array for the values
      ss << ind << "uchar " << mat_name << "[" << mat->rows << "]["
         << mat->columns << "]"
         << " = {\n";
      for (unsigned y = 0; y < mat->rows; y++) {
        ss << ind << sind << "{";
        for (unsigned x = 0; x < mat->columns; x++) {
          switch (mat->data_type) {
            case VX_TYPE_INT32:
              ss << (((*reinterpret_cast<int32_t*>(
                          mat->mat.data() + (4 * (y * mat->columns + x)))) == 0)
                         ? 0
                         : 1);
              break;
            case VX_TYPE_FLOAT32:
              ss << (((*reinterpret_cast<float*>(
                          mat->mat.data() + (4 * (y * mat->columns + x)))) == 0)
                         ? 0
                         : 1);
              break;
            case VX_TYPE_UINT8:
              ss << (((*reinterpret_cast<unsigned char*>(
                          mat->mat.data() + (1 * y * mat->columns + x))) == 0)
                         ? 0
                         : 1);
              break;
            case VX_TYPE_INT16:
              ss << (((*reinterpret_cast<short*>(
                          mat->mat.data() + (2 * (y * mat->columns + x)))) == 0)
                         ? 0
                         : 1);
              break;
            default:
              ERRORM("Unsupported data_type @ def(HipaccMatrix)")
          }
          if (x == mat->columns - 1)
            ss << "}";
          else
            ss << ",";
        }
        if (y != mat->rows - 1) ss << ",\n";
      }
      ss << "};\n";

      ss << dind << "Domain " << dom_name << "(" << mat_name << ");\n";
      break;
    }
    default: {
      ERRORM("Unsupported deftype @ def_dom(HipaccMatrix)")
    }
  }
}

void hipacc_writer::def_mask(std::stringstream& ss, HipaccMatrix* mat,
                             DefType deftype) {
  std::string mask_name = name(mat) + "_mask";
  if (masks.find(mask_name) != masks.end()) return;
  masks.insert(mask_name);

  switch (deftype) {
    case DefType::Hdecl: {
      std::string type;
      switch (mat->data_type) {
        case VX_TYPE_INT32:
          type = "int";
          break;
        case VX_TYPE_FLOAT32:
          type = "float";
          break;
        case VX_TYPE_UINT8:
          type = "unsigned char";
          break;
        case VX_TYPE_INT16:
          type = "short";
          break;
        default:
          ERRORM("Unsupported data_type @ def_mask(HipaccMatrix)")
      }

      ss << dind << "Mask<" << type << "> " << mask_name << "(" << name(mat)
         << ");\n";
      break;
    }
    default: {
      ERRORM("Unsupported deftype @ def_mask(HipaccMatrix)")
    }
  }
}

class graph_gen {
 public:
  dag g_dag;
  OptGraphT* _g_opt;

  std::list<EdgeDesc> edges;
  std::list<VertexDesc>* _verts;
  std::list<VertexDesc> nodes;
  std::list<VertexDesc> spaces;

 public:
  void init();

  graph_gen(dag& _g_dag) : g_dag(_g_dag) { init(); }

  VertexType* get_vert(VertexDesc& v) { return &((*_g_opt)[v]); }

  ObjectTask get_vert_task(VertexDesc& v) { return get_vert(v)->task(); }

  ObjectType get_vert_type(VertexDesc& v) { return get_vert(v)->type(); }

  EdgeType* get_edge(EdgeDesc& e) { return &(*_g_opt)[e]; }

  VertexDesc source(EdgeDesc& e) { return boost::source(e, *_g_opt); }

  VertexDesc target(EdgeDesc& e) { return boost::target(e, *_g_opt); }

  ObjectType source_t(EdgeDesc& e) {
    auto v = source(e);
    return get_vert_type(v);
  }

  ObjectType target_t(EdgeDesc& e) {
    auto v = target(e);
    return get_vert_type(v);
  }

  // debugging functions
  void print_nodes();

  void print_spaces();

  void print_edges();
};

void graph_gen::init() {
  _g_opt = g_dag._g_opt;

  _verts = g_dag.set_order_of_exec();
  g_dag.print_order_of_exec();

  for (auto i : *_verts) {
    switch ((*_g_opt)[i].task()) {
      case ObjectTask::Buffer:
        spaces.push_back(i);
        break;
      case ObjectTask::Computation:
        nodes.push_back(i);
        break;
      default:
        std::cout << "Warning: I have reached nodes where VertexTask = None\n";
        break;
    }
  }

  OptGraphT::edge_iterator beg, end;
  std::tie(beg, end) = boost::edges(*_g_opt);
  std::for_each(beg, end, [this](EdgeDesc it) { edges.push_back(it); });
}

void graph_gen::print_nodes() {
  for (auto i : nodes) std::cout << get_vert(i)->name() << std::endl;
};

void graph_gen::print_spaces() {
  for (auto i : spaces) std::cout << get_vert(i)->name() << std::endl;
};

void graph_gen::print_edges() {
  for (auto i : edges) std::cout << get_edge(i)->name() << std::endl;
};

class hipacc_gen : public graph_gen, public hipacc_writer {
 protected:
  std::vector<HipaccIterationSpace> is_l;
  std::vector<HipaccAccessor> acc_l;

 public:
  hipacc_gen(dag& _g_dag) : graph_gen(_g_dag) {}

  using hipacc_writer::def;

  std::stringstream ss_im;
  std::stringstream ss_acc;
  std::stringstream ss_is;
  std::stringstream ss_kern;
  std::stringstream ss_kern_host;
  std::stringstream ss_execs;
  std::stringstream ss_out_im;
  // new
  std::stringstream ss_sc;
  std::stringstream ss_mat;
  std::stringstream ss_mask;
  std::stringstream ss_dom;
  std::stringstream ss_array;

  std::vector<std::string> already_included_kernels;

  template <typename ParameterList, typename NodeType, typename EdgeType>
  void add_output_to_kernelparams(ParameterList& list, NodeType node,
                                  EdgeType ie);

  template <typename ParameterList, typename NodeType, typename EdgeDesc>
  void add_input_to_kernelparams(ParameterList& list, NodeType node,
                                 EdgeDesc ie, std::string dom_name);

  void def(VertexType* hn);

  void iterate_nodes();
  void iterate_spaces();

  void set_edges();

  void dump_code() {
    ss << initial_includes() << "\n\n";

    ss << "// benchmarking\n";
    ss << "#ifndef BENCH_ITER_COUNT\n";
    ss << dind << "#define BENCH_ITER_COUNT 1\n";
    ss << "#endif\n";

    ss << "// kernel declarations\n";
    ss << ss_kern.str() << "\n";

    ss << "int main() {\n";

    ss << dind << "// scalars\n";
    ss << ss_sc.str() << "\n";

    ss << dind << "// matrix\n";
    ss << ss_mat.str() << "\n";

    ss << dind << "// domains\n";
    ss << ss_dom.str() << "\n";

    ss << dind << "// masks\n";
    ss << ss_mask.str() << "\n";

    ss << dind << "// arrays\n";
    ss << ss_array.str() << "\n";

    ss << dind << "// images\n";
    ss << ss_im.str() << "\n";

    ss << dind << "// accessors\n";
    ss << ss_acc.str() << "\n";

    ss << dind << "// iteration spaces\n";
    ss << ss_is.str() << "\n";

    ss << dind << "// kernels\n";
    ss << ss_kern_host.str() << "\n";

    ss << dind << "// execution\n";
    ss << dind << "std::vector<double> timing_vec(BENCH_ITER_COUNT);\n";
    ss << dind << "for(int i = 0; i < BENCH_ITER_COUNT; i++) {\n";
    ss <<           ss_execs.str();
    ss << tind <<   "timing_vec[i] = timing;\n";
    ss << dind << "}\n";
    ss << dind << std::endl;

    ss << dind << "// performance\n";
    ss << dind << "std::sort(timing_vec.begin(), timing_vec.end());\n";
    ss << dind << "fprintf(stdout,\"Number of iterations: %d\\n\", BENCH_ITER_COUNT);\n";
    ss << dind << "fprintf(stdout,\"<HIPACC:> Overall time min: %f(ms)\\n\", timing_vec[0]);\n";
    ss << dind << "fprintf(stdout,\"<HIPACC:> Overall time med: %f(ms)\\n\", timing_vec[(BENCH_ITER_COUNT - 1) /2]);\n";
    ss << dind << "fprintf(stdout,\"<HIPACC:> Overall time max: %f(ms)\\n\", timing_vec[BENCH_ITER_COUNT - 1]);\n";

    ss << dind << std::endl;
    ss << dind << "// output image writes\n";
    ss << dind <<  ss_out_im.str() << "\n";

    ss << dind << "return 0;\n";
    ss << "}\n";

    auto code = ss.str();
    std::cout << code;
    std::ofstream to_file(hipaVX_output_filename);
    to_file << code;
  }
};

void hipacc_gen::set_edges() {
  // set the edges as accessors and iteration spaces
  for (auto e : edges) {
    auto src = source(e);
    auto dst = target(e);
    auto edge = get_edge(e);

    HipaccImage* _im = nullptr;
    if (get_vert_type(src) == VX_TYPE_IMAGE &&
        get_vert_type(dst) == VX_TYPE_NODE) {
      edge->set_as_acc();
      _im = obj2img(get_vert(src));
    } else if (get_vert_type(src) == VX_TYPE_NODE &&
               get_vert_type(dst) == VX_TYPE_IMAGE) {
      edge->set_as_is();
      _im = obj2img(get_vert(dst));
    }

    if (_im) {
      edge->set_img(_im);
      edge->set_name();
    }
  }
}

template <typename ParameterList, typename NodeType, typename NodeId>
size_t get_parameter_index(const ParameterList& list, NodeType n, NodeId id) {
  // TODO: make n->parameters a hashlist based on its id
  // TODO: fix: multiple edges from same image to same Node
  //            -> have the same ids at n->parameters.
  //            -> different ids at edge objects
  //            make it more cleaver and use edge ids
  size_t i = 0;
  for (; i < n->parameters.size() + 1; i++) {
    if (id == n->parameters[i]->o->id()) {
      if (list.at(i) == "") break;
    }
    if (i == n->parameters.size())
      ERRORM("attempt to add a wrong parameter to kernel parameterlist")
  }
  return i;
}

template <typename ParameterList, typename NodeType, typename EdgeDesc>
void hipacc_gen::add_input_to_kernelparams(ParameterList& list, NodeType node,
                                           EdgeDesc ie, std::string dom_name) {
  // TODO: get rid of dom_name here
  auto src = &(*_g_opt)[ie->m_source];
  auto index = get_parameter_index(list, node, src->id());
  // if (node->kernel->direction[i] != VX_INPUT) error
  bool hipacc_param = false;
  DomVX::HipaccKernel* hk;
  if (src->type() != node->kernel->type[index]) {
    hk = dynamic_cast<DomVX::HipaccKernel*>(node->kernel->get());
    if (!hk || index >= hk->hipacc_type.size() ||
        hk->hipacc_type[index] == HipaccParameterType::None)
      ERRORM("type mismatch")
    else
      hipacc_param = true;
  }

  if (hipacc_param) {
    switch (hk->hipacc_type[index]) {
      case HipaccParameterType::Accessor: {
        auto edge = &(*_g_opt)[*ie];
        if (edge->isImgSet()) {
          def_acc(ss_acc, edge, DefType::Hdecl, dom_name);
        }
        list.at(index) = name(edge);
      } break;
      case HipaccParameterType::Mask: {
        def_mask(ss_mask, (HipaccMatrix*)src->get());
        std::string mask_name = name((HipaccMatrix*)src) + "_mask";
        list.at(index) = mask_name;
      } break;
      case HipaccParameterType::Domain: {
        def_dom(ss_dom, (HipaccMatrix*)src->get());
        std::string dom_name = name((HipaccMatrix*)src) + "_dom";
        list.at(index) = dom_name;
      } break;
      default:
        ERRORM("no case for hk->hipacc_type[index]")
    }
  } else {
    switch (src->type()) {
      case VX_TYPE_SCALAR: {
        list.at(index) = name((HipaccScalar*)src);
      } break;
      default:
        ERRORM("no case for src->type()")
    }
  }
}

template <typename ParameterList, typename NodeType, typename EdgeType>
void hipacc_gen::add_output_to_kernelparams(ParameterList& list, NodeType node,
                                            EdgeType ie) {
  auto target = &(*_g_opt)[ie->m_target];
  auto index = get_parameter_index(list, node, target->id());
  // if (node->kernel->direction[i] != VX_OUTPUT) error
  bool hipacc_param = false;
  DomVX::HipaccKernel* hk;
  if (target->type() != node->kernel->type[index]) {
    hk = dynamic_cast<DomVX::HipaccKernel*>(node->kernel->get());
    if (!hk || index >= hk->hipacc_type.size() ||
        hk->hipacc_type[index] == HipaccParameterType::None)
      ERRORM("type mismatch")
    else
      hipacc_param = true;
  }

  if (hipacc_param) {
    switch (hk->hipacc_type[index]) {
      case HipaccParameterType::IterationSpace: {
        auto edge = &(*_g_opt)[*ie];
        if (edge->isImgSet()) {
          def_is(ss_is, edge, DefType::Hdecl);
        }
        list.at(index) = name(edge);
      } break;
      default:
        ERRORM("no case for hk->hipacc_type[index]")
    }
  } else {
    switch (target->type()) {
      case VX_TYPE_SCALAR: {
        list.at(index) = name((HipaccScalar*)target);
      } break;
      default:
        ERRORM("no case for target->type()")
    }
  }
}

std::string get_kernel_name(std::string const& file,
                            std::string const& filename) {
  if (file == "") ERRORM("Couldn't find file " + filename)
  size_t class_index = file.find("class");
  size_t filename_index = file.find(" ", class_index) + 1;
  size_t filename_end_index = file.find(" ", filename_index);

  std::string file_name =
      file.substr(filename_index, filename_end_index - filename_index);

  return file_name;
}

// TODO: Please simplify this function
// TODO: we can use namespaces for different components
// (hVX::template::read_file)
// TODO: Please use get functions instead of A->b->((*somecast)C)
void hipacc_gen::iterate_nodes() {
  ss_execs << tind << "double timing = 0.0;\n" << std::endl;
  for (auto vert : nodes) {
    // here check nodes and print them etc...
    auto v = get_vert(vert);

    auto cn = dynamic_cast<DomVX::CustomNode*>(v->get());
    // assert(false && "check for cpp node aswell");
    if (cn == nullptr)
      ERRORM("iterate_nodes: vertex must be an instance of DomVX::CustomNode")

    auto hk = dynamic_cast<DomVX::HipaccKernel*>(cn->kernel);
    auto ck = dynamic_cast<DomVX::CppKernel*>(cn->kernel);

    if (hk) {
      // TODO: use get functions
      // read hipacc kernel from the corresponding file
      std::cout << hk->filename << std::endl;
      std::string file = read_file(hk->filename);

      std::string kernel_name = get_kernel_name(file, hk->filename);
      std::string kernel_instance_name = kernel_name + "_" + cn->id();
      ss_execs << tind << kernel_instance_name << ".execute();" << std::endl;
      ss_execs << tind << "timing += hipacc_last_kernel_timing();" << std::endl;

      std::vector<std::string> param_names(cn->parameters.size());

      //bool local = false;
      std::string dom_name = "";
      for (unsigned int i = 0; i < hk->hipacc_type.size(); i++) {
        if (hk->hipacc_type[i] == HipaccParameterType::Domain) {
          //local = true;
          dom_name =
              (name((HipaccMatrix*)cn->parameters[i]->o->get()) + "_dom");
          break;
        }
      }

      // input parameters: accessor etc
      graphVX::OptGraphInEdgeIter ie, ie_end;
      for (std::tie(ie, ie_end) = boost::in_edges(vert, *_g_opt); ie != ie_end;
           ie++) {
        add_input_to_kernelparams(param_names, cn, ie, dom_name);
      }

      // output parameters: iteration space etc
      graphVX::OptGraphOutEdgeIter it, it_end;
      for (std::tie(it, it_end) = boost::out_edges(vert, *_g_opt); it != it_end;
           it++) {
        add_output_to_kernelparams(param_names, cn, it);
      }

      if (std::find(already_included_kernels.cbegin(),
                    already_included_kernels.cend(),
                    hk->filename) == already_included_kernels.cend()) {
        already_included_kernels.emplace_back(hk->filename);
        ss_kern << file;
      }

      ss_kern_host << dind;
      ss_kern_host << kernel_name << " " << kernel_instance_name << "(";
      for (unsigned int i = 0; i < param_names.size(); i++) {
        if (i != 0) ss_kern_host << ", ";
        ss_kern_host << param_names[i];
      }
      ss_kern_host << ");\n";
    } else if (ck) {
      std::cout << ck->filename << std::endl;
      std::string kernel = read_file(ck->filename);
      if (kernel == "") ERRORM("Couldn't find file " + ck->filename)
      size_t void_index = kernel.find("void");
      size_t methodname_index = kernel.find(" ", void_index) + 1;
      size_t methodname_end_index_space = kernel.find(" ", methodname_index);
      size_t methodname_end_index_brace = kernel.find("(", methodname_index);
      size_t methodname_end_index =
          (methodname_end_index_space < methodname_end_index_brace)
              ? methodname_end_index_space
              : methodname_end_index_brace;

      std::string method_name = kernel.substr(
          methodname_index, methodname_end_index - methodname_index);

      std::vector<std::string> param_names;

      //bool local = false;
      std::string dom_name = "";
      for (unsigned int i = 0; i < cn->parameters.size(); i++) {
        if (cn->kernel->type[i] == VX_TYPE_MATRIX) {
          //local = true;
          dom_name =
              (name((HipaccMatrix*)cn->parameters[i]->o->get()) + "_dom");
          break;
        }
      }
      for (unsigned int i = 0; i < cn->parameters.size(); i++) {
        if (cn->kernel->direction[i] == VX_INPUT) {
          graphVX::OptGraphInEdgeIter ie, ie_end;
          for (std::tie(ie, ie_end) = boost::in_edges(vert, *_g_opt);
               ie != ie_end; ie++) {
            auto src = &(*_g_opt)[ie->m_source];
            if (src->id() == cn->parameters[i]->o->id()) {
              if (src->type() != cn->kernel->type[i]) ERRORM("type mismatch")
              switch (src->type()) {
                case VX_TYPE_IMAGE: {
                  auto edge = &(*_g_opt)[*ie];
                  if (edge->isImgSet()) {
                    std::string image_name = name(edge->im) + "_" + cn->id() +
                                             "_" + std::to_string(i);

                    ss_execs << dind << dtype(edge->im) << " *" << image_name
                             << " = " << name(edge->im) << ".data();\n";

                    param_names.push_back(image_name);
                    param_names.push_back(std::to_string(edge->im->w));
                    param_names.push_back(std::to_string(edge->im->h));
                  }
                } break;
                case VX_TYPE_SCALAR: {
                  param_names.push_back(name((HipaccScalar*)src));
                } break;
                case VX_TYPE_MATRIX: {
                  DomVX::VX_Matrix* mat = ((HipaccMatrix*)src->get());
                  std::string mat_name =
                      name(mat) + "_" + cn->id() + "_" + std::to_string(i);

                  // define an array for the values
                  ss_execs << dind << "int " << mat_name << "["
                           << mat->rows * mat->columns << "]"
                           << " = {\n"
                           << tind;
                  for (unsigned y = 0; y < mat->rows; y++) {
                    for (unsigned x = 0; x < mat->columns; x++) {
                      switch (mat->data_type) {
                        case VX_TYPE_INT32:
                          ss_execs << *reinterpret_cast<int32_t*>(
                              mat->mat.data() + (4 * (y * mat->columns + x)));
                          break;
                        case VX_TYPE_FLOAT32:
                          ss_execs << *reinterpret_cast<float*>(
                              mat->mat.data() + (4 * (y * mat->columns + x)));
                          break;
                        case VX_TYPE_UINT8:
                          ss_execs << *reinterpret_cast<unsigned char*>(
                              mat->mat.data() + (1 * y * mat->columns + x));
                          break;
                        case VX_TYPE_INT16:
                          ss_execs << *reinterpret_cast<short*>(
                              mat->mat.data() + (2 * (y * mat->columns + x)));
                          break;
                        default:
                          ERRORM(
                              "Unsupported data_type @ "
                              "hipacc_gen::iterate_nodes()")
                      }
                      if (x != mat->columns - 1 || y != mat->rows - 1)
                        ss_execs << ",";
                    }
                  }
                  ss_execs << "};\n";

                  param_names.push_back(mat_name);
                  param_names.push_back(std::to_string(mat->columns));
                  param_names.push_back(std::to_string(mat->rows));
                } break;
                case VX_TYPE_BOOL: {
                  param_names.push_back((((Bool*)src->get())->value) ? "true"
                                                                     : "false");
                } break;
                default:
                  ERRORM("no case for src->type()")
              }
            }
          }
        } else {
          graphVX::OptGraphOutEdgeIter beg, end;
          std::tie(beg, end) = boost::out_edges(vert, *_g_opt);
          for (auto it = beg; it != end; it++) {
            auto target = &(*_g_opt)[it->m_target];
            if (target->id() == cn->parameters[i]->o->id()) {
              if (target->type() != cn->kernel->type[i]) ERRORM("type mismatch")
              switch (target->type()) {
                case VX_TYPE_IMAGE: {
                  auto edge = &(*_g_opt)[*it];
                  if (edge->isImgSet()) {
                    std::string array_name = name(edge->im) + "_out_" +
                                             cn->id() + "_" + std::to_string(i);

                    ss_execs << dind << dtype(edge->im) << " *" << array_name
                             << " = new " << dtype(edge->im) << "["
                             << std::to_string(edge->im->w * edge->im->h)
                             << "];\n";

                    param_names.push_back(array_name);
                    param_names.push_back(std::to_string(edge->im->w));
                    param_names.push_back(std::to_string(edge->im->h));
                  }
                } break;
                case VX_TYPE_ARRAY: {
                  param_names.push_back(name((Array*)target));
                  param_names.push_back(
                      std::to_string(((Array*)target->get())->size));
                } break;
                case VX_TYPE_SCALAR: {
                  param_names.push_back("&" + name((HipaccScalar*)target));
                } break;
                default:
                  ERRORM("no case for src->type()")
              }
            }
          }
        }
      }

      if (std::find(already_included_kernels.cbegin(),
                    already_included_kernels.cend(),
                    ck->filename) == already_included_kernels.cend()) {
        already_included_kernels.emplace_back(ck->filename);
        ss_kern << kernel;
      }

      ss_execs << dind;
      ss_execs << method_name << "(";
      for (unsigned int i = 0; i < param_names.size(); i++) {
        if (i != 0) ss_execs << ", ";
        ss_execs << param_names[i];
      }
      ss_execs << ");\n";

      // Write back the software images to hipacc images

      for (unsigned int i = 0; i < cn->parameters.size(); i++) {
        if (cn->kernel->direction[i] == VX_OUTPUT) {
          graphVX::OptGraphOutEdgeIter beg, end;
          std::tie(beg, end) = boost::out_edges(vert, *_g_opt);
          for (auto it = beg; it != end; it++) {
            auto target = &(*_g_opt)[beg->m_target];
            if (target->id() == cn->parameters[i]->o->id()) {
              if (target->type() != cn->kernel->type[i]) ERRORM("type mismatch")
              switch (target->type()) {
                case VX_TYPE_IMAGE: {
                  auto edge = &(*_g_opt)[*it];
                  if (edge->isImgSet()) {
                    def_is(ss_is, edge, DefType::Hdecl);

                    std::string array_name = name(edge->im) + "_out_" +
                                             cn->id() + "_" + std::to_string(i);

                    ss_execs << dind << name(edge->im) << " = "
                             << array_name + ";\n";
                    ss_execs << dind << "delete[] " << array_name + ";\n";
                  }
                } break;
                case VX_TYPE_SCALAR: {
                } break;
                case VX_TYPE_ARRAY: {
                } break;
                default:
                  ERRORM("no case for src->type()")
              }
            }
          }
        }
      }
    }
  }
}

void hipacc_gen::iterate_spaces() {
  for (auto v : spaces) {
    DomVX::Object* n = get_vert(v);
    def(n);
  }

  for (auto output_space : g_dag.space_to_file) {
    auto img = obj2img(output_space.first);

    int channels = 1;
    std::string conversion = "";
    if (img->get_dtype() == VX_DF_IMAGE_RGBX ||
        img->get_dtype() == VX_DF_IMAGE_UYVY) {
      channels = 4;
      conversion = "(uchar*) ";
    }

    std::string data_name = name(img) + "_data";
    ss_out_im << dind << dtype(img) << " *" << data_name << " = " << name(img)
              << ".data();\n";
    ss_out_im << dind << "save_data(" << img->w << ", " << img->h << ", "
              << channels << ", " << conversion << data_name << ", \""
              << output_space.second << "\");\n";
  }
}

void hipacc_gen::def(VertexType* hn) {
  switch (hn->type()) {
    case HipaccImageE: {
      auto _im = obj2img(hn);
      def(ss_im, _im);
      break;
    }
    case HipaccScalarE: {
      auto _sc = obj2scalar(hn);
      def(ss_sc, _sc);
      break;
    }
    case HipaccMatrixE: {
      auto _mat = obj2matrix(hn);
      def(ss_mat, _mat);
      break;
    }
    case HipaccArrayE: {
      auto _arr = obj2array(hn);
      def(ss_array, _arr);
      break;
    }
    case HipaccBoolE: {
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
      ERRORM("hipacc_writer::def : type of the VertexType" + hn->name() +
             "is not defined")
    }
  }
}
