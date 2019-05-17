#pragma once

#include "../dsl/abstractions.hpp"
#include "../dsl/types.hpp"
#include "../graph/graph.hpp"
#include "cassert"
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

using CppImage = DomVX::Image;
using CppDomain = DomVX::Domain;
using CppMask = DomVX::Mask;
using DomVXAcc = DomVX::Acc;

using CppAccessor = DomVXAcc;
using CppIterationSpace = DomVXAcc;

using CppKernel = DomVX::OperatorNode;
using CppPointNode = DomVX::PointOperator;
using CppLocalNode = DomVX::LocalOperator;

using CppDataType = vx_df_image;

#define U8 VX_DF_IMAGE_U8
#define S16 VX_DF_IMAGE_S16
#define U16 VX_DF_IMAGE_U16
#define S32 VX_DF_IMAGE_S32
#define U32 VX_DF_IMAGE_U32
#define F32 VX_TYPE_FLOAT32
#define UNDEF VX_TYPE_DF_IMAGE

#define CppImageE VX_TYPE_IMAGE

enum class CppDefType {
  Hdecl,
  Kdecl,
  Param,
};

namespace cpp_abstraction_code_generator {
std::string dump_code(std::shared_ptr<DomVX::OperatorNode> op,
                      std::vector<CppAccessor*> input_image_accessors,
                      std::vector<CppAccessor*> output_image_is,
                      std::vector<std::string> scalar_names = {},
                      bool use_default_names = false);

std::string dump_code(std::shared_ptr<DomVX::PointOperator> point,
                      std::vector<CppAccessor*> input_image_accessors,
                      std::vector<CppAccessor*> output_image_is,
                      std::vector<std::string> scalar_names = {},
                      bool use_default_names = false);

std::string dump_code(std::shared_ptr<DomVX::LocalOperator> local,
                      std::vector<CppAccessor*> input_image_accessors,
                      std::vector<CppAccessor*> output_image_is,
                      std::vector<std::string> scalar_names = {},
                      bool use_default_names = false);

std::string dump_code(std::shared_ptr<DomVX::GlobalOperator> global,
                      std::vector<CppAccessor*> input_image_accessors,
                      std::vector<std::string> scalar_names = {},
                      bool use_default_names = false);
};  // namespace cpp_abstraction_code_generator

class cpp_writer {
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

  std::string _dtype(CppDataType type, std::string name = "");

  void def(VertexType*);

  void def(std::stringstream& ss, CppImage* n,
           CppDefType deftype = CppDefType::Hdecl);

  void def_acc(std::stringstream& ss, DomVXAcc* n,
               CppDefType deftype = CppDefType::Hdecl);

  void def_is(std::stringstream& ss, DomVXAcc* is,
              CppDefType deftype = CppDefType::Hdecl);

  void def(std::stringstream& ss, DomVXAcc* n,
           CppDefType deftype = CppDefType::Hdecl);

  void def(std::stringstream& ss, CppDomain* n,
           CppDefType deftype = CppDefType::Hdecl);

  void def(std::stringstream& ss, CppMask* mask,
           CppDefType deftype = CppDefType::Hdecl);

  void def(std::stringstream& ss, CppKernel* kern,
           CppDefType deftype = CppDefType::Hdecl,
           std::vector<CppAccessor*> acc_l = {},
           std::vector<CppIterationSpace*> is_l = {},
           std::vector<CppMask*> mask_l = {},
           std::vector<CppDomain*> dom_l = {},
           std::stringstream* execute_ss = nullptr);

  void def(std::stringstream& ss, CppKernel* kern,
           CppDefType deftype = CppDefType::Hdecl,
           std::initializer_list<CppAccessor*> acc_l = {},
           std::initializer_list<CppIterationSpace*> is_l = {},
           std::initializer_list<CppMask*> mask_l = {},
           std::initializer_list<CppDomain*> dom_l = {}) {
    def(ss, kern, deftype, std::vector<CppAccessor*>(acc_l),
        std::vector<CppIterationSpace*>(is_l), std::vector<CppMask*>(mask_l),
        std::vector<CppDomain*>(dom_l));
  }

 public:
  cpp_writer() = default;

  std::string initial_includes();
};

// ----------------- conversion to string -------------------------------------

template <class T>
std::string cpp_writer::dtype(T* n) {
  return _dtype(n->get_dtype(), n->name());
}

template <class T>
std::string cpp_writer::name(T* n) {
  std::string ss = n->name();

  if (ss.empty()) ERRORM("cpp_writer::name : returns an empty string")

  return ss;
}

class cpp_graph_gen {
 public:
  dag g_dag;
  OptGraphT* _g_opt;

  std::list<EdgeDesc> edges;
  std::list<VertexDesc>* _verts;
  std::list<VertexDesc> nodes;
  std::list<VertexDesc> spaces;

 public:
  void init();

  cpp_graph_gen(dag& _g_dag) : g_dag(_g_dag) { init(); }

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

class cpp_gen : public cpp_graph_gen, public cpp_writer {
 protected:
  std::vector<CppIterationSpace> is_l;
  std::vector<CppAccessor> acc_l;

 public:
  cpp_gen(dag& _g_dag) : cpp_graph_gen(_g_dag) {}

  using cpp_writer::def;

  std::stringstream ss_mask;
  std::stringstream ss_dom;
  std::stringstream ss_im;
  std::stringstream ss_acc;
  std::stringstream ss_is;
  std::stringstream ss_kern;
  std::stringstream ss_kern_host;
  std::stringstream ss_execs;

  void def(VertexType* hn);

  void iterate_nodes();
  void iterate_spaces();

  void set_edges();

  void dump_code() {
    ss << initial_includes() << "\n\n";

    ss << "// kernel declarations\n";
    ss << ss_kern.str() << "\n";

    ss << "int main() {\n";

    ss << dind << "// masks\n";
    ss << ss_mask.str() << "\n";

    ss << dind << "// domains\n";
    ss << ss_dom.str() << "\n";

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

    ss << dind << "return 0;\n";
    ss << "}\n";

    std::cout << ss.str();
    // std::ofstream out_file("test.cpp", std::ios::trunc);
    // out_file << ss.str();
  }
};
