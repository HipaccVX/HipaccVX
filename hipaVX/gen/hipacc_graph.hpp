#pragma once

#include "../dsl/abstractions.hpp"
#include "../dsl/types.hpp"
#include "../graph/graph.hpp"
#include "../vx_adaptor.hpp"
#include "cassert"
#include "gen_template.hpp"
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
using HipaccDomain = DomVX::Domain;
using HipaccMask = DomVX::Mask;
using DomVXAcc = DomVX::Acc;

using HipaccAccessor = DomVXAcc;
using HipaccIterationSpace = DomVXAcc;

using HipaccDataType = vx_df_image;

using HipaccKernel = DomVX::HipaccNode;

// vx_df_image to HipaccWriter
#define U8 VX_DF_IMAGE_U8
#define S16 VX_DF_IMAGE_S16
#define U16 VX_DF_IMAGE_U16
#define S32 VX_DF_IMAGE_S32
#define U32 VX_DF_IMAGE_U32
#define RGBA VX_DF_IMAGE_RGBX
#define F32 VX_TYPE_FLOAT32
#define UNDEF VX_TYPE_DF_IMAGE

#define HipaccImageE VX_TYPE_IMAGE
#define HipaccScalarE VX_TYPE_SCALAR
#define HipaccMatrixE VX_TYPE_MATRIX

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

HipaccKernel* obj2node(VertexType* v) {
  // TODO: catch exception to avoid segmentation fault
  HipaccKernel* _node = dynamic_cast<DomVX::HipaccNode*>(v->get());

  if (_node == nullptr)
    ERRORM("graph_gen obj2node, dynamic cast fail for: " + v->name())

  return _node;
}

class hipacc_writer {
 protected:
  std::string hind = " ";
  std::string sind = hind + hind;
  std::string dind = sind + sind;
  std::string tind = dind + sind;

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

  void def_acc(std::stringstream& ss, DomVXAcc* n, DefType deftype,
               std::string dom_name);

  void def_is(std::stringstream& ss, DomVXAcc* is,
              DefType deftype = DefType::Hdecl);

  void def(std::stringstream& ss, HipaccDomain* n,
           DefType deftype = DefType::Hdecl);

  void def(std::stringstream& ss, HipaccMask* mask,
           DefType deftype = DefType::Hdecl);

  void def(std::stringstream& ss, HipaccKernel* kern,
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
      std::string val_name = name(mat) + "_val";
      std::string ind = dind;

      // define an array for the values
      ss << ind << type << " " << val_name << "[" << mat->rows << "]["
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

      // define mask
      ss << ind << "Mask<" << type << "> " << name(mat) << "(" << val_name
         << ");\n";
      ss << ind << "Domain " << name(mat) + "_dom"
         << "(" << name(mat) << ");\n";
      break;
    }
    default: {
      ERRORM("Unsupported deftype @ def(HipaccMatrix)")
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
      ERRORM("Unsupported deftype @ def(image)")
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

void hipacc_writer::def(std::stringstream& ss, HipaccDomain* dom,
                        DefType deftype) {
  std::string dtype = "uchar";

  switch (deftype) {
    case DefType::Hdecl: {
      std::string val_name = name(dom) + "_val";
      ss << dind << dtype << " " << val_name << "[" << dom->height * dom->width
         << "] = {";

      // for(auto &ity : dom->domain) {
      //  std::copy(ity.begin(), ity.end() - 1, std::ostream_iterator<int>(ss,
      //  ", ")); ss << int(ity.back()); if (ity != dom->domain.back()){ ss <<
      //  ", "; }
      //}
      for (unsigned y = 0; y < dom->height; y++) {
        for (unsigned x = 0; x < dom->width - 1; x++) {
          ss << int(dom->domain[y].at(x)) << ", ";
        }
        ss << int(dom->domain[y].back());
        if (y != dom->height - 1) {
          ss << ", ";
        }
      }

      ss << "};\n";
      ss << dind << "Domain " << name(dom) << "<" << dom->width << ", "
         << dom->height << ">(" << val_name << ");\n";
      break;
    }
    case DefType::Kdecl:
    case DefType::Param: {
      ss << "Domain &" << name(dom);
      break;
    }

    default: {
      ERRORM("Unsupported deftype @ def(dom)")
    }
  }
}

void hipacc_writer::def(std::stringstream& ss, HipaccMask* mask,
                        DefType deftype) {
  // fix this, use OpenVX data types similar to Image
  // also union access to mask coefficients should be improved
  std::string dtype = "";
  if (mask->mask_is_int == false) {
    dtype = "float";
  } else {
    dtype = "int";
  }

  switch (deftype) {
    case DefType::Hdecl: {
      std::string val_name = name(mask) + "_val";

      std::string ind = dind;

      // define an array for the values
      ss << ind << dtype << " " << val_name << "[" << mask->height << "]["
         << mask->width << "]"
         << " = {\n";
      for (unsigned y = 0; y < mask->height; y++) {
        ss << ind << sind << "{";
        for (unsigned x = 0; x < mask->width - 1; x++) {
          ss << mask->mask[y].at(x).i << ", ";
        }
        ss << mask->mask[y].back().i << "}";
        if (y != mask->height - 1) {
          ss << ",\n";
        }
      }
      ss << "};\n";

      // define mask
      ss << ind << "Mask<" << dtype << "> " << name(mask) << "(" << val_name
         << ");\n";
      break;
    }

    case DefType::Param:
    case DefType::Kdecl: {
      ss << name(mask) << "<" << dtype << "> &" << name(mask);
      break;
    }

    default: {
      ERRORM("Unsupported deftype @ def(mask)")
    }
  }
}

// TODO: can have a base class for the parameter types, thus one initializer
// list
//       , which shortens the code significantly
void hipacc_writer::def(std::stringstream& ss, HipaccKernel* kern,
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

  std::vector<std::string> already_included_kernels;

  void def(VertexType* hn);

  void iterate_nodes();
  void iterate_spaces();

  void set_edges();

  void dump_code() {
    ss << initial_includes() << "\n\n";

    ss << "// kernel declarations\n";
    ss << ss_kern.str() << "\n";

    ss << "int main() {\n";

    ss << dind << "// scalars\n";
    ss << ss_sc.str() << "\n";

    ss << dind << "// matrix/mask/domain\n";
    ss << ss_mat.str() << "\n";

    ss << dind << "// images\n";
    ss << ss_im.str() << "\n";

    ss << dind << "// accessors\n";
    ss << ss_acc.str() << "\n";

    ss << dind << "// iteration spaces\n";
    ss << ss_is.str() << "\n";

    ss << dind << "// kernels\n";
    ss << ss_kern_host.str() << "\n";

    ss << dind << "// execution\n";
    ss << ss_execs.str() << "\n";

    ss << dind << "// output image writes\n";
    ss << ss_out_im.str() << "\n";

    ss << dind << "return 0;\n";
    ss << "}\n";

    std::cout << ss.str();
    std::ofstream to_file("hipacc_tmp/hipacc_gen.cpp");
    to_file << ss.str();
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

void hipacc_gen::iterate_nodes() {
  for (auto vert : nodes) {
    // here check nodes and print them etc...
    auto v = get_vert(vert);

    auto hn = dynamic_cast<DomVX::HipaccNode*>(v->get());
    if (hn == nullptr)
      ERRORM("iterate_nodes: vertex must be an instance of DomVX::HipaccNope")

    std::cout << hn->kernel->filename << std::endl;
    std::string kernel = read_file(hn->kernel->filename);
    if (kernel == "") ERRORM("Couldn't find file " + hn->kernel->filename)
    size_t class_index = kernel.find("class");
    size_t kernelname_index = kernel.find(" ", class_index) + 1;
    size_t kernelname_end_index = kernel.find(" ", kernelname_index);

    std::string kernel_name = kernel.substr(
        kernelname_index, kernelname_end_index - kernelname_index);

    std::string kernel_instance_name = kernel_name + "_" + hn->id();

    ss_execs << dind << kernel_instance_name << ".execute();" << std::endl;

    std::vector<std::string> param_names;

    bool local = false;
    std::string dom_name = "";
    for (unsigned int i = 0; i < hn->parameters.size(); i++) {
      if (hn->kernel->type[i] == VX_TYPE_MATRIX) {
        local = true;
        dom_name = (name((HipaccMatrix*)hn->parameters[i]->o->get()) + "_dom");
        break;
      }
    }
    for (unsigned int i = 0; i < hn->parameters.size(); i++) {
      if (hn->kernel->direction[i] == VX_INPUT) {
        graphVX::OptGraphInEdgeIter ie, ie_end;
        for (std::tie(ie, ie_end) = boost::in_edges(vert, *_g_opt);
             ie != ie_end; ie++) {
          auto src = &(*_g_opt)[ie->m_source];
          if (src->id() == hn->parameters[i]->o->id()) {
            if (src->type() != hn->kernel->type[i]) ERRORM("type mismatch")
            switch (src->type()) {
              case VX_TYPE_IMAGE: {
                auto edge = &(*_g_opt)[*ie];
                if (edge->isImgSet()) {
                  def_acc(ss_acc, edge, DefType::Hdecl, dom_name);
                }
                param_names.push_back(name(edge));
              } break;
              case VX_TYPE_SCALAR: {
                param_names.push_back(name((HipaccScalar*)src));
              } break;
              case VX_TYPE_MATRIX: {
                param_names.push_back(name((HipaccMatrix*)src) + "_dom");
                param_names.push_back(name((HipaccMatrix*)src));
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
          auto target = &(*_g_opt)[beg->m_target];
          if (target->id() == hn->parameters[i]->o->id()) {
            if (target->type() != hn->kernel->type[i]) ERRORM("type mismatch")
            switch (target->type()) {
              case VX_TYPE_IMAGE: {
                auto edge = &(*_g_opt)[*it];
                if (edge->isImgSet()) {
                  def_is(ss_is, edge, DefType::Hdecl);
                }
                param_names.push_back(name(edge));
              } break;
              case VX_TYPE_SCALAR: {
                param_names.push_back(name((HipaccScalar*)target));
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
                  hn->kernel->filename) == already_included_kernels.cend()) {
      already_included_kernels.emplace_back(hn->kernel->filename);
      ss_kern << kernel;
    }

    ss_kern_host << dind;
    ss_kern_host << kernel_name << " " << kernel_instance_name << "(";
    for (unsigned int i = 0; i < param_names.size(); i++) {
      if (i != 0) ss_kern_host << ", ";
      ss_kern_host << param_names[i];
    }
    ss_kern_host << ");\n";
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
    if (img->get_dtype() == VX_DF_IMAGE_RGBX) {
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
