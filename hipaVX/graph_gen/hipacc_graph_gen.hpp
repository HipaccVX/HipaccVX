#pragma once

#include "../graph.hpp"
#include "../domVX_types.hpp"
#include "../abstractions.hpp"
#include "../../VX/vx.h"
#include "cassert"
//#include <iterator>
//#include "infix_iterator.h"

//#define ERROR(cond, msg) std::assert((cond) && (msg));
//#define ERRORM(msg) static_assert(true, msg);
#define ERRORM(msg) throw std::runtime_error((msg));


using graphVX::dag;
using graphVX::VertexDesc;
using graphVX::VertexType;
using graphVX::EdgeDesc;
using graphVX::EdgeType;
using graphVX::OptGraphT;
using HipaVX::VertexTask;
using HipaVX::ObjectType;

using HipaccImage = HipaVX::Image;
using HipaccDomain = DomVX::Domain;
using HipaccMask = DomVX::Mask;
using DomVXAcc = HipaVX::Acc;

using HipaccKernel = DomVX::OperatorNode;
using HipaccPointNode = DomVX::Map;
using HipaccLocalNode = DomVX::LocalOperation;

using HipaccDataType = vx_df_image;




// vx_df_image to HipaccWriter
#define U8  VX_DF_IMAGE_U8
#define S16 VX_DF_IMAGE_S16
#define U16 VX_DF_IMAGE_U16
#define S32 VX_DF_IMAGE_S32
#define U32 VX_DF_IMAGE_U32
#define F32 VX_TYPE_FLOAT32
#define UNDEF VX_TYPE_DF_IMAGE

#define HipaccImageE VX_TYPE_IMAGE

enum class DefType{
  Hdecl,
  Kdecl,
  Param,
};

// pointer casts
HipaccImage* obj2img(VertexType* v) {
  HipaccImage* _im = dynamic_cast<HipaccImage*>(v->_obj);

  if(_im == NULL)
    ERRORM("graph_gen obj2img, dynamic cast fail for: " + v->get_name());

  return _im;
}

// TODO: simplify this after fixing the data structures
HipaccKernel* obj2kernel(VertexType* v) {
  auto _node = dynamic_cast<HipaVX::Node*>(v->_obj);

  if(_node == NULL)
    ERRORM("graph_gen obj2kernel, dynamic cast fail for: " + v->get_name());

  return _node->kernel.get();
}

HipaccLocalNode* kernel2local(HipaccKernel* v) {
  auto _kern = dynamic_cast<HipaccLocalNode*>(v);

  if(_kern == NULL)
    ERRORM("graph_gen kernel2local, dynamic cast fail for: " + v->get_name());

  return _kern;
}

// TODO: Merge Accessor and IterationSpace
class HipaccAccessor : public DomVXAcc {
 public:
   void set_name(std::string _name = "_acc"){
     name = im->get_name() + _name + std::to_string(my_id);
   };

   HipaccAccessor(){};

   HipaccAccessor(HipaccImage* _im) {
    set_img(_im);
    set_name();
   };
};

class HipaccIterationSpace : public DomVXAcc {
 public:
   void set_name(std::string _name = "_is"){
     name = im->get_name() + _name + std::to_string(my_id);
   };

   HipaccIterationSpace(){};

   HipaccIterationSpace(HipaccImage* _im){
    set_img(_im);
    set_name();
   };
};

class hipacc_writer {
 protected:
  std::string hind = " ";
  std::string sind = hind + hind;
  std::string dind = sind + sind;
  std::string tind = dind + sind;

 public:
  std::stringstream ss;

  template<class T> std::string name(T* node);

  template<class T> std::string dtype(T* node);

  std::string _dtype(HipaccDataType type, std::string name = "");


  void def(VertexType*);

  void def(std::stringstream &ss, HipaccImage* n, DefType deftype = DefType::Hdecl);

  void def_acc(std::stringstream &ss, DomVXAcc* n, DefType deftype = DefType::Hdecl);

  void def_is(std::stringstream &ss, DomVXAcc* is, DefType deftype = DefType::Hdecl);

  void def(std::stringstream &ss, DomVXAcc* n, DefType deftype = DefType::Hdecl);

  void def(std::stringstream &ss, HipaccDomain* n, DefType deftype = DefType::Hdecl);

  void def(std::stringstream &ss, HipaccMask* mask, DefType deftype = DefType::Hdecl);

  void def(std::stringstream &ss, HipaccKernel* kern, DefType deftype = DefType::Hdecl,
           std::initializer_list<HipaccAccessor*> acc_l = {},
           std::initializer_list<HipaccIterationSpace*> is_l = {},
           std::initializer_list<HipaccMask*> mask_l = {},
           std::initializer_list<HipaccDomain*> dom_l = {});

 public:
  hipacc_writer() {};

  std::string initial_includes();
};


// ----------------- conversion to string -------------------------------------
std::string hipacc_writer::_dtype(HipaccDataType type, std::string name) {
  switch (type) {
    case U8:  { return "uchar"; }
    case S16: { return "short"; }
    case U16: { return "unsigned short"; }
    case S32: { return "int"; }
    case U32: { return "uint"; }
    case F32: { return "float"; }
    case UNDEF: { return "undef"; }
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

void hipacc_writer::def(std::stringstream &ss, HipaccImage* img, DefType deftype) {
  switch(deftype) {
    case DefType::Hdecl: {
      ss << dind << "Image" << "<" << dtype(img) + "> " << name(img)
                    << "(" << img->get_width() << ", " << img->get_height() << ");\n";
      break;
    }

    default : {
      ERRORM("Unsupported deftype @ def(image)");
    }
  }
}

void hipacc_writer::def_acc(std::stringstream &ss, DomVXAcc* acc, DefType deftype) {
  if (acc->isImgSet() == false) {
    ERRORM("hipacc_writer::def(acc) : acc " + acc->get_name() + " has no image");
  }

  switch(deftype) {
    case DefType::Hdecl: {
      std::stringstream params;
      params << acc->width() << ", " << acc->height();

      if(acc->isRoiSet) {
        params << "," << acc->roi.x << ", "  << acc->roi.y;
      }

      if(acc->isInterpSet) {
        params << "more";
      }

      ss << dind << "Accessor" << "<" << dtype(acc->im) + "> " << name(acc) << "(" << params.str() << ");\n";
      break;
    }

    case DefType::Kdecl:
    case DefType::Param:
    {
      ss << "Accessor" << "<" << dtype(acc->im) + "> &" << name(acc);
      break;
    }

    default : {
      ERRORM("Unsupported deftype @ def(image)");
    }
  }
}

void hipacc_writer::def_is(std::stringstream &ss, DomVXAcc* is, DefType deftype) {
  if (is->isImgSet() == false) {
    ERRORM("hipacc_writer::def(is) : is" + is->get_name() + " has no image");
  }

  switch(deftype) {
    case DefType::Hdecl: {
      std::stringstream params;
      params << is->width() << ", " << is->height();

      if(is->isRoiSet) {
        params << "," << is->roi.x << ", "  << is->roi.y;
      }

      if(is->isInterpSet) {
        params << "more";
      }

      ss << dind << "IterationSpace" << "<" << dtype(is->im) + "> " << name(is) << "(" << params.str() << ");\n";
      break;
    }

    case DefType::Kdecl:
    case DefType::Param:
    {
      ss << "IterationSpace" << "<" << dtype(is->im) + "> &" << name(is);
      break;
    }

    default : {
      ERRORM("Unsupported deftype @ def(image)");
    }
  }
}

void hipacc_writer::def(std::stringstream &ss, DomVXAcc* in, DefType deftype) {
  if (in->is_acc) {
    def_acc(ss, in, deftype);
  } else {
    def_is(ss, in, deftype);
  }
}

void hipacc_writer::def(std::stringstream &ss, HipaccDomain* dom, DefType deftype) {
  std::string dtype = "uchar";

  switch(deftype) {
    case DefType::Hdecl:  {
      std::string val_name = name(dom) + "_val";
      ss << dind << dtype << " " <<  val_name << "[" << dom->height * dom->width << "] = {";

      //for(auto &ity : dom->domain) {
      //  std::copy(ity.begin(), ity.end() - 1, std::ostream_iterator<int>(ss, ", "));
      //  ss << int(ity.back());
      //  if (ity != dom->domain.back()){ ss << ", "; }
      //}
      for(unsigned y = 0; y < dom->height; y++) {
        for(unsigned x = 0; x < dom->width - 1; x++) {
          ss << int(dom->domain[y].at(x)) << ", ";
        }
        ss << int(dom->domain[y].back());
        if(y != dom->height - 1) { ss << ", "; };
      }

      ss << "};\n";
      ss << dind << "Domain " << name(dom) << "<" << dom->width << ", " << dom->height << ">(" << val_name << ");\n";
      break;
    }
    case DefType::Kdecl:
    case DefType::Param:
    {
      ss << "Domain &" << name(dom);
      break;
    }

    default : {
      ERRORM("Unsupported deftype @ def(dom)");
    }
  }
}

void hipacc_writer::def(std::stringstream &ss, HipaccMask* mask, DefType deftype) {
  // fix this, use OpenVX data types similar to Image
  // also union access to mask coefficients should be improved
  std::string dtype = "";
  if(mask->mask_is_int == false) {
    dtype = "float";
  } else {
    dtype = "int";
  }

  switch(deftype) {
    case DefType::Hdecl:  {
      std::string val_name = name(mask) + "_val";

      std::string ind = dind;

      // define an array for the values
      ss << ind << dtype << " " << val_name << "[" << mask->height << "][" <<  mask->width << "]" << " = {\n";
      for(unsigned y = 0; y < mask->height; y++) {
        ss << ind << sind << "{";
        for(unsigned x = 0; x < mask->width - 1; x++) {
          ss << mask->mask[y].at(x).i << ", ";
        }
        ss << mask->mask[y].back().i << "}";
        if(y != mask->height - 1) { ss << ",\n"; };
      }
      ss << "};\n";

      // define mask
      ss << ind << "Mask<" << dtype << "> " << name(mask) << "(" << val_name << ");\n";
      break;
    }

    case DefType::Param:
    case DefType::Kdecl:  {
      ss << name(mask) << "<" << dtype << "> &" << name(mask);
      break;
    }

    default : {
      ERRORM("Unsupported deftype @ def(mask)");
    }
  }
}

// TODO: can have a base class for the parameter types, thus one initializer list
//       , which shortens the code significantly
void hipacc_writer::def(std::stringstream &ss, HipaccKernel* kern, DefType deftype,
      std::initializer_list<HipaccAccessor*> acc_l,
      std::initializer_list<HipaccIterationSpace*> is_l,
      std::initializer_list<HipaccMask*> mask_l,
      std::initializer_list<HipaccDomain*> dom_l) {
  if (is_l.size() != 1) {
    ERRORM("Hipacc Kernel" + name(kern) + " has " + std::to_string(is_l.size()) + " iteration spaces");
  }

  if (acc_l.size() < 1) {
    ERRORM("Hipacc Kernel" + name(kern) + " has " + std::to_string(acc_l.size()) + " accesors");
  }

  switch(deftype) {
    case DefType::Hdecl:  {
      ss << dind;
      ss << name(kern) << "(";
      ss << name(*is_l.begin());

      for(auto acc : acc_l){
        ss << ", " << name(acc);
      }

      for(auto mask : mask_l){
        ss << ", " << name(mask);
      }

      for(auto dom : dom_l){
        ss << ", " << name(dom);
      }

      // TODO: add other parameters
      ss << ");\n";
      break;
    }
    case DefType::Kdecl: {
      std::stringstream kdecls;
      std::stringstream kparams;
      std::stringstream kinits;
      std::stringstream kconstb;

      auto is = *is_l.begin();

      kparams << dind; def(kparams, is, DefType::Param);
      kinits << dind << "Kernel(" << name(is) << ")\n";

      for(auto par : acc_l) {
        kdecls << sind; def(kdecls, par, DefType::Kdecl); kdecls << ";\n";
        kparams << "\n" << dind << ","; def(kparams, par, DefType::Param);
        kinits << dind << "," << name(par) << "(" << name(par) << ")\n";
        kconstb << dind << "add_accessor(" << name(par)  << ");\n";
      }

      for(auto par : mask_l) {
        kdecls << sind; def(kdecls, par, DefType::Kdecl); kdecls << ";\n";
        kparams << "\n" << dind << ","; def(kparams, par, DefType::Param);
        kinits << dind << "," << name(par) << "(" << name(par) << ")\n";
      }

      for(auto par : dom_l) {
        kdecls << sind; def(kdecls, par, DefType::Kdecl); kdecls << ";\n";
        kparams << "\n" << dind << ","; def(kparams, par, DefType::Param);
        kinits << dind << "," << name(par) << "(" << name(par) << ")\n";
      }

      ss << "class " << name(kern) << " : public Kernel<" << dtype(is->im) << "> {\n";
      ss << hind << "private:\n";
      ss << kdecls.str();

      // Constructor
      ss << "\n" << hind << "public:\n";
      ss << sind << name(kern) << "(\n";
      ss << kparams.str();

      ss << "\n" << sind << ") : \n";
      ss << kinits.str();

      ss << sind << "{ \n";
      ss << kconstb.str();

      ss << sind << "}\n";

      // Kernel
      ss << sind << "\n\n";
      ss << sind << "void kernel() {\n";
      ss << dind << "visit(" << name(kern) << ");\n";
      ss << sind << "}\n";

      ss << "};\n\n";
      break;
    }
    default : {
      ERRORM("Unsupported deftype @ def(kern)");
    }
  }
}

#include <boost/graph/adjacency_list.hpp>

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

  graph_gen(dag &_g_dag) : g_dag(_g_dag) {
    init();
  };

  VertexType* get_vert(VertexDesc& v) { return &((*_g_opt)[v]); }

  VertexTask get_vert_task(VertexDesc& v) { return get_vert(v)->get_task(); }

  ObjectType get_vert_type(VertexDesc& v) { return get_vert(v)->get_type(); }

  EdgeType& get_edge(EdgeDesc& e) { return (*_g_opt)[e]; }

  VertexDesc source(EdgeDesc& e) { return boost::source(e, *_g_opt); }

  VertexDesc target(EdgeDesc& e) { return boost::target(e, *_g_opt); }

  ObjectType source_t (EdgeDesc& e) {
    auto v = source(e);
    return get_vert_type(v);
  }

  ObjectType target_t (EdgeDesc& e) {
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

  OptGraphT::edge_iterator beg, end;
  std::tie(beg, end) = boost::edges(*_g_opt);
  std::for_each(beg, end, [this](EdgeDesc it) { edges.push_back(it); });
}

void graph_gen::print_nodes() {
    for(auto i : nodes)
      std::cout << get_vert(i)->get_name() << std::endl;
};

void graph_gen::print_spaces() {
  for(auto i : spaces)
    std::cout << get_vert(i)->get_name() << std::endl;
};

void graph_gen::print_edges() {
  for(auto i : edges)
    std::cout << get_edge(i).get_name() << std::endl;
};



class hipacc_gen : public graph_gen, public hipacc_writer {
 protected:
   std::vector<HipaccIterationSpace> is_l;
   std::vector<HipaccAccessor> acc_l;

 public:
  hipacc_gen(dag &_g_dag) :graph_gen(_g_dag) {};

  using hipacc_writer::def;

  std::stringstream ss_mask;
  std::stringstream ss_dom;
  std::stringstream ss_im;
  std::stringstream ss_acc;
  std::stringstream ss_is;
  std::stringstream ss_kern;
  std::stringstream ss_execs;

  std::stringstream ss;

  void def(VertexType* hn);

  void iterate_nodes();
  void iterate_spaces();

  void set_edges();

  void dump_code() {
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
    ss << ss_kern.str() << "\n";

    ss << dind << "// execution\n";
    ss << ss_execs.str() << "\n";

    ss << dind << "return 0;\n";
    ss << "}\n";

    std::cout << ss.str();
  }
};

void hipacc_gen::set_edges() {
  // set the edges as accessors and iteration spaces
  for(auto e : edges) {
    auto src = source(e);
    auto dst = target(e);
    auto edge = get_edge(e);

    HipaccImage* _im = NULL;
    if(get_vert_type(src) == VX_TYPE_IMAGE &&
        get_vert_type(dst) == VX_TYPE_NODE) {
      edge.is_acc = true;
      _im = obj2img(get_vert(src));
    } else if (get_vert_type(src) == VX_TYPE_NODE &&
               get_vert_type(dst) == VX_TYPE_IMAGE) {
      edge.is_acc = false;
      _im = obj2img(get_vert(dst));
    }

    if(_im) {
      edge.set_img(_im);
      edge.set_name();

      if( edge.is_acc == true ) {
        def_acc(ss_acc, &edge, DefType::Hdecl);
      } else {
        def_is(ss_is, &edge, DefType::Hdecl);
      }
    }
  }
}

void hipacc_gen::iterate_nodes() {
    for(auto vert : nodes) {
      auto v = get_vert(vert);
      ss_execs << dind << v->get_name() << ".execute()" << std::endl;

      // get domains
      auto kern_ = obj2kernel(v);
      if(kern_->operator_type == DomVX::OperatorType::LocalOperation) {
        auto local_ = kernel2local(kern_);

        auto doms = local_->domain_bindings;
      }

      // TODO: kernel defs and calls
      //def(ss_kern, v, DefType deftype,
      //std::initializer_list<HipaccAccessor*> acc_l,
      //std::initializer_list<HipaccIterationSpace*> is_l,
      //std::initializer_list<HipaccMask*> mask_l,
      //std::initializer_list<HipaccDomain*> dom_l) {
    }
}

void hipacc_gen::iterate_spaces() {
  for(auto v : spaces) {
      HipaVX::Object *n = get_vert(v);
      def(n);
  }
}

void hipacc_gen::def(VertexType* hn) {
  switch (hn->type) {
    case HipaccImageE: {
      auto _im = obj2img(hn);
      def(ss_im, _im);
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
      ERRORM("hipacc_writer::def : type of the VertexType" + hn->get_name() + "is not defined");
      break;
    }
  }
}
