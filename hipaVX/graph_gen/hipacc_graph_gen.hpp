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
using graphVX::OptGraphT;
using HipaVX::VertexTask;
using HipaVX::ObjectType;

using HipaccNode = HipaVX::Object;
using HipaccImage = HipaVX::Image;
using HipaccDomain = ast4vx::WindowDescriptor;
using HipaccMask = DomVX::Mask;

using HipaccKernel = DomVX::AbstractionNode;
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
  
#define HipaccImageE VX_TYPE_IMAGE

enum class DefType{
  Hdecl,
  Kdecl,
  Param,
};

struct ROI {
  int x; 
  int y; 
  int width; 
  int height; 
};

// TODO: Merge Accessor and IterationSpace
class HipaccAccessor {
 public:
  HipaccImage* im;
  std::string name;

  bool isRoiSet;
  bool isInterpSet;

  ROI roi;

  void init() {
    isRoiSet = false;
    isInterpSet = false;
    roi = ROI{ 0, 0, 0, 0}; 
  }

  bool isImgSet() { return !(im == NULL); };

  int width()  { return im->get_width(); };

  int height() { return im->get_height(); };

  std::string get_name() { return name; };

  HipaccAccessor( ) {
    init();
    im = NULL;
  };

  HipaccAccessor( HipaccImage* _im ) : im(_im) {
    init();
    // TODO: make sure that this is unique
    name = im->get_name() + "_acc";
  };
};

class HipaccIterationSpace {
 public:
  HipaccImage* im;
  std::string name;

  bool isRoiSet;
  bool isInterpSet;

  ROI roi;

  void init() {
    isRoiSet = false;
    isInterpSet = false;
    roi = ROI{ 0, 0, 0, 0}; 
  }

  bool isImgSet() { return !(im == NULL); };

  int width()  { return im->get_width(); };

  int height() { return im->get_height(); };

  std::string get_name() { return name; };

  HipaccIterationSpace( ) {
    init();
    im = NULL;
  };

  HipaccIterationSpace(HipaccImage* _im ) : im(_im) {
    init();
    // TODO: make sure that this is unique
    name = im->get_name() + "_is";
  };

};


class hipacc_writer {
 private:
  std::string hind = " ";
  std::string ind = hind + hind;
  std::string dind = ind + ind;
  std::string tind = dind + ind;
  
 public:
  std::stringstream ss, ss_im;

  template<class T> std::string name(T* node);

  template<class T> std::string dtype(T* node);

  std::string _dtype(HipaccDataType type, std::string name = "");


  void def(HipaccNode*);

  void def(std::stringstream &ss, HipaccImage* n, DefType deftype = DefType::Hdecl);

  void def(std::stringstream &ss, HipaccAccessor* n, DefType deftype = DefType::Hdecl);

  void def(std::stringstream &ss, HipaccIterationSpace* is, DefType deftype = DefType::Hdecl);

  void def(std::stringstream &ss, HipaccDomain* n, DefType deftype = DefType::Hdecl);

  void def(std::stringstream &ss, HipaccMask* mask, DefType deftype = DefType::Hdecl);

  void def(std::stringstream &ss, HipaccKernel* kern, DefType deftype = DefType::Hdecl,
           std::initializer_list<HipaccAccessor*> acc_l = {},
           std::initializer_list<HipaccIterationSpace*> is_l = {},
           std::initializer_list<HipaccMask*> mask_l = {},
           std::initializer_list<HipaccDomain*> dom_l = {});

  std::string param(HipaccNode);

 public:
  hipacc_writer() {};

  std::string dump_code();

  std::string initial_includes();
};


// ------------------------ public API ---------------------------------------
std::string hipacc_writer::dump_code() {
  ss << initial_includes() << std::endl;
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

void hipacc_writer::def(std::stringstream &ss, HipaccImage* img, DefType deftype) {
  switch(deftype) {
    case DefType::Hdecl: {
      ss << "Image" << "<" << dtype(img) + "> " << name(img)
                    << "(" << img->get_width() << ", " << img->get_height() << ");\n";
      break;
    }

    default : {
      ERRORM("Unsupported deftype @ def(image)");
    }
  }
}

void hipacc_writer::def(std::stringstream &ss, HipaccAccessor* acc, DefType deftype) {
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

      ss << "Accessor" << "<" << dtype(acc->im) + "> " << name(acc) << "(" << params.str() << ");\n";
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

void hipacc_writer::def(std::stringstream &ss, HipaccIterationSpace* is, DefType deftype) {
  if (is->isImgSet() == false) {
    ERRORM("hipacc_writer::def(is) : is" + is->get_name() + " has no image");
  }

  std::stringstream params;
  params << is->width() << ", " << is->height();

  if(is->isRoiSet) {
    params << "," << is->roi.x << ", "  << is->roi.y; 
  }

  if(is->isInterpSet) {
    params << "more";
  }

  ss << "IterationSpace" << "<" << dtype(is->im) + "> " << name(is) << "(" << params.str() << ")";
}

void hipacc_writer::def(std::stringstream &ss, HipaccDomain* dom, DefType deftype) {
  std::string dtype = "uchar";

  switch(deftype) {
    case DefType::Hdecl:  { 
      std::string val_name = name(dom) + "_val";
      ss << dtype << " " <<  val_name << "[" << dom->height * dom->width << "] = {";
    
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
      ss << "Domain " << name(dom) << "<" << dom->width << ", " << dom->height << ">(" << val_name << ");\n";
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

      // define an array for the values
      ss << dtype << " " << val_name << "[" << mask->height << "][" <<  mask->width << "]" << " = {\n";
      for(unsigned y = 0; y < mask->height; y++) {
        ss << "  {";
        for(unsigned x = 0; x < mask->width - 1; x++) {
          ss << mask->mask[y].at(x).i << ", ";
        }
        ss << mask->mask[y].back().i << "}";
        if(y != mask->height - 1) { ss << ",\n"; };
      }
      ss << "};\n";

      // define mask
      ss << "Mask<" << dtype << "> " << name(mask) << "(" << val_name << ");\n";
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
        kdecls << ind; def(kdecls, par, DefType::Kdecl); kdecls << ";\n";
        kparams << "\n" << dind << ","; def(kparams, par, DefType::Param);
        kinits << dind << "," << name(par) << "(" << name(par) << ")\n";
        kconstb << dind << "add_accessor(" << name(par)  << ");\n";
      }

      for(auto par : mask_l) {
        kdecls << ind; def(kdecls, par, DefType::Kdecl); kdecls << ";\n";
        kparams << "\n" << dind << ","; def(kparams, par, DefType::Param);
        kinits << dind << "," << name(par) << "(" << name(par) << ")\n";
      }

      for(auto par : dom_l) {
        kdecls << ind; def(kdecls, par, DefType::Kdecl); kdecls << ";\n";
        kparams << "\n" << dind << ","; def(kparams, par, DefType::Param);
        kinits << dind << "," << name(par) << "(" << name(par) << ")\n";
      }

      ss << "class " << name(kern) << " : public Kernel<" << dtype(is->im) << "> {\n";
      ss << hind << "private:\n";
      ss << kdecls.str();

      // Constructor
      ss << "\n" << hind << "public:\n";
      ss << ind << name(kern) << "(\n";
      ss << kparams.str();

      ss << "\n" << ind << ") : \n";
      ss << kinits.str();

      ss << ind << "{ \n";
      ss << kconstb.str();

      ss << ind << "}\n";

      // Kernel 
      ss << ind << "\n\n";
      ss << ind << "void kernel() {\n";
      ss << dind << "visit(" << name(kern) << ");\n";
      ss << ind << "}\n";

      ss << "};\n\n";
      break;
    }
    default : {
      ERRORM("Unsupported deftype @ def(kern)");
    }
  }
}


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

