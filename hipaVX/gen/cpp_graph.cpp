#include "cpp_graph.hpp"
#include "cpp_ast.hpp"
#include "gen_template.hpp"

static CppImage* obj2img(graphVX::VertexType* v) {
  CppImage* _im = dynamic_cast<CppImage*>(v->get());

  if (_im == nullptr)
    ERRORM("graph_gen obj2img, dynamic cast fail for: " + v->name())

  return _im;
}

static CppKernel* obj2node(graphVX::VertexType* v) {
  // TODO: catch exception to avoid segmentation fault
  CppKernel* _node = dynamic_cast<DomVX::Node*>(v->get());

  if (_node == nullptr)
    ERRORM("graph_gen obj2node, dynamic cast fail for: " + v->name())

  return _node;
}

static CppLocalNode* kernel2local(CppKernel* v) {
  auto _kern = dynamic_cast<CppLocalNode*>(v);

  if (_kern == nullptr)
    ERRORM("graph_gen kernel2local, dynamic cast fail for: " + v->name())

  return _kern;
}

std::string cpp_writer::_dtype(CppDataType type, std::string name) {
  switch (type) {
    case U8: {
      return "uint8_t";
    }
    case S16: {
      return "int16_t";
    }
    case U16: {
      return "uint16_t";
    }
    case S32: {
      return "int32_t";
    }
    case U32: {
      return "uint32_t";
    }
    case F32: {
      return "float";
    }
    case UNDEF: {
      return "undef";
    }
    default: {
      ERRORM("hipac_writer::dtype, unknown dtype for " + name)
    }
  }
}

std::string cpp_writer::initial_includes() {
  std::string out;

  out += "#include <algorithm>\n";
  out += "#include <cmath>\n";
  out += "#include <cstdlib>\n";
  out += "#include <vector>\n";
  out += "#include \"cpp_helper.hpp\"\n\n";

  return out;
}

void cpp_writer::def(std::stringstream& ss, CppImage* img, CppDefType deftype) {
  switch (deftype) {
    case CppDefType::Hdecl: {
      ss << dind << "matrix"
         << "<" << dtype(img) + "> " << name(img) << "(" << img->get_width()
         << ", " << img->get_height() << ");\n";
      break;
    }

    default: {
      ERRORM("Unsupported deftype @ def(image)")
    }
  }
}

void cpp_writer::def_is(std::stringstream& ss, DomVXAcc* is,
                        CppDefType deftype) {
  if (is->isImgSet() == false) {
    ERRORM("cpp_writer::def(is) : is" + is->name() + " has no image")
  }

  switch (deftype) {
    case CppDefType::Hdecl: {
      std::stringstream params;
      params << is->im->name();

      if (is->isRoiSet) {
        params << "roi";
      }

      if (is->isInterpSet) {
        params << "more";
      }

      ss << dind << "IterationSpace"
         << "<" << dtype(is->im) + "> " << name(is) << "(" << params.str()
         << ");\n";
      break;
    }

    case CppDefType::Kdecl:
    case CppDefType::Param: {
      ss << "IterationSpace"
         << "<" << dtype(is->im) + "> &" << name(is);
      break;
    }

    default: {
      ERRORM("Unsupported deftype @ def(image)")
    }
  }
}

void cpp_writer::def(std::stringstream& ss, DomVXAcc* in, CppDefType deftype) {
  if (in->is_acc) {
    def_acc(ss, in, deftype);
  } else {
    def_is(ss, in, deftype);
  }
}

void cpp_writer::def(std::stringstream& ss, CppDomain* dom,
                     CppDefType deftype) {
  std::string dtype = "matrix<uint8_t>";

  switch (deftype) {
    case CppDefType::Hdecl: {
      std::stringstream ss_values;
      ss_values << "{";
      for (unsigned y = 0; y < dom->height; y++) {
        for (unsigned x = 0; x < dom->width - 1; x++) {
          ss_values << int(dom->domain[y].at(x)) << ", ";
        }
        ss_values << int(dom->domain[y].back());
        if (y != dom->height - 1) {
          ss_values << ", ";
        }
      }
      ss_values << "}";

      ss << dind << dtype << " " << name(dom) << "(" << dom->width << ", "
         << dom->height << ", " << ss_values.str() << ");\n";
      break;
    }
    case CppDefType::Kdecl:
    case CppDefType::Param: {
      ss << dtype << "& " << name(dom);
      break;
    }

    default: {
      ERRORM("Unsupported deftype @ def(dom)")
    }
  }
}

void cpp_writer::def(std::stringstream& ss, CppMask* mask, CppDefType deftype) {
  // fix this, use OpenVX data types similar to Image
  // also union access to mask coefficients should be improved
  std::string dtype = "";
  if (mask->mask_is_int == false) {
    dtype = "float";
  } else {
    dtype = "int";
  }

  switch (deftype) {
    case CppDefType::Hdecl: {
      std::string val_name = name(mask) + "_val";

      std::string ind = dind;
      std::stringstream values;
      values << "{";

      for (unsigned y = 0; y < mask->height; y++) {
        for (unsigned x = 0; x < mask->width - 1; x++) {
          values << mask->mask[y].at(x).i << ", ";
        }
        values << mask->mask[y].back().i;
        if (y != mask->height - 1) {
          values << ",\n";
        }
      }
      values << "}";

      // define mask
      ss << ind << "matrix<" << dtype << "> " << name(mask) << "("
         << mask->width << ", " << mask->height << ", " << values.str()
         << ");\n";
      break;
    }

    case CppDefType::Param:
    case CppDefType::Kdecl: {
      ss << "matrix<" << dtype << "> &" << name(mask);
      break;
    }

    default: {
      ERRORM("Unsupported deftype @ def(mask)")
    }
  }
}

void cpp_writer::def_acc(std::stringstream& ss, DomVXAcc* acc,
                         CppDefType deftype) {
  if (acc->isImgSet() == false) {
    ERRORM("cpp_writer::def(acc) : acc " + acc->name() + " has no image")
  }

  switch (deftype) {
    case CppDefType::Hdecl: {
      std::stringstream params;
      params << acc->im->name();

      if (acc->isRoiSet) {
        params << "," << acc->roi.x << ", " << acc->roi.y;
      }

      if (acc->isInterpSet) {
        params << "more";
      }

      ss << dind << "Accessor"
         << "<" << dtype(acc->im) + "> " << name(acc) << "(" << params.str()
         << ");\n";
      break;
    }

    case CppDefType::Kdecl:
    case CppDefType::Param: {
      ss << "Accessor"
         << "<" << dtype(acc->im) + "> &" << name(acc);
      break;
    }

    default: {
      ERRORM("Unsupported deftype @ def(image)")
    }
  }
}

void cpp_writer::def(std::stringstream& ss, CppKernel* kern, CppDefType deftype,
                     std::vector<CppAccessor*> acc_l,
                     std::vector<CppIterationSpace*> is_l,
                     std::vector<CppMask*> mask_l,
                     std::vector<CppDomain*> dom_l,
                     std::stringstream* execute_ss_ptr) {
  auto& execute_ss = *execute_ss_ptr;
  if (is_l.size() != 1) {
    ERRORM("Kernel" + name(kern) + " has " + std::to_string(is_l.size()) +
           " iteration spaces")
  }

  switch (deftype) {
    case CppDefType::Hdecl: {
      execute_ss << dind;
      execute_ss << name(kern) << "(";
      execute_ss << name(*is_l.begin());

      for (auto acc : acc_l) {
        execute_ss << ", " << name(acc);
      }

      for (auto mask : mask_l) {
        execute_ss << ", " << name(mask);
      }

      for (auto dom : dom_l) {
        execute_ss << ", " << name(dom);
      }

      // TODO: add other parameters
      execute_ss << ");\n";
      break;
    }
    case CppDefType::Kdecl: {
      std::stringstream kparams;

      auto is = *is_l.begin();

      kparams << dind;
      def(kparams, is, CppDefType::Param);

      for (auto par : acc_l) {
        kparams << "\n" << dind << ",";
        def(kparams, par, CppDefType::Param);
      }

      for (auto par : mask_l) {
        kparams << "\n" << dind << ",";
        def(kparams, par, CppDefType::Param);
      }

      for (auto par : dom_l) {
        kparams << "\n" << dind << ",";
        def(kparams, par, CppDefType::Param);
      }

      ss << "void " << name(kern) << "(\n";
      ss << kparams.str() << ")\n";
      ss << "{\n";
      if (kern->operator_type == DomVX::OperatorType::LocalOperator)
        ss << cpp_abstraction_code_generator::dump_code(
                  std::shared_ptr<DomVX::LocalOperator>(
                      dynamic_cast<DomVX::LocalOperator*>(kern),
                      [](DomVX::LocalOperator*) {}),
                  acc_l, is_l)
           << "\n";
      else if (kern->operator_type == DomVX::OperatorType::PointOperator)
        ss << cpp_abstraction_code_generator::dump_code(
                  std::shared_ptr<DomVX::PointOperator>(
                      dynamic_cast<DomVX::PointOperator*>(kern),
                      [](DomVX::PointOperator*) {}),
                  acc_l, is_l)
           << "\n";
      else
        ERRORM("OperatorType not supported")
      ss << "}\n";
      break;
    }
    default: {
      ERRORM("Unsupported deftype @ def(kern)")
    }
  }
}

void cpp_graph_gen::init() {
  _g_opt = g_dag._g_opt;

  _verts = g_dag.set_order_of_exec();
  // g_dag.print_order_of_exec();

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

void cpp_graph_gen::print_spaces() {
  for (auto i : spaces) std::cout << get_vert(i)->name() << std::endl;
}

void cpp_graph_gen::print_nodes() {
  for (auto i : nodes) std::cout << get_vert(i)->name() << std::endl;
}

void cpp_graph_gen::print_edges() {
  for (auto i : edges) std::cout << get_edge(i)->name() << std::endl;
}

void cpp_gen::set_edges() {
  // set the edges as accessors and iteration spaces
  for (auto e : edges) {
    auto src = source(e);
    auto dst = target(e);
    auto edge = get_edge(e);

    CppImage* _im = nullptr;
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

void cpp_gen::iterate_nodes() {
  for (auto vert : nodes) {
    auto v = get_vert(vert);

    std::vector<CppDomain*> dom_l;
    std::vector<CppMask*> mask_l;
    std::vector<DomVXAcc*> acc_l;
    std::vector<DomVXAcc*> is_l;

    auto kern_ = obj2node(v);
    if (kern_->operator_type == DomVX::OperatorType::LocalOperator) {
      auto local_ = kernel2local(kern_);

      // get domains
      auto dom_binds = local_->domain_bindings;
      for (auto const& domb : dom_binds) {
        // TODO: have better APIs for these type of operations
        dom_l.push_back(domb.second.get());
        def(ss_dom, domb.second.get());
      }

      // get masks
      auto mask_binds = local_->mask_bindings;
      for (auto const& maskbv : mask_binds) {
        // TODO: have better APIs for these type of operations
        auto maskb = maskbv.second;
        for (auto const& mask : maskb) {
          mask_l.push_back(mask.get());
          def(ss_mask, mask.get());
        }
      }
    }

    // iteration spaces
    graphVX::OptGraphOutEdgeIter beg, end;
    std::tie(beg, end) = boost::out_edges(vert, *_g_opt);
    for (auto it = beg; it != end; it++) {
      auto edge = &(*_g_opt)[*it];
      if (edge->isImgSet()) {
        def_is(ss_is, edge, CppDefType::Hdecl);
      }
      is_l.push_back(edge);
    }

    // accessors
    graphVX::OptGraphInEdgeIter ie, ie_end;
    for (std::tie(ie, ie_end) = boost::in_edges(vert, *_g_opt); ie != ie_end;
         ie++) {
      auto edge = &(*_g_opt)[*ie];
      if (edge->isImgSet()) {
        def_acc(ss_acc, edge, CppDefType::Hdecl);
      }
      acc_l.push_back(edge);
    }

    def(ss_kern, kern_, CppDefType::Kdecl, acc_l, is_l, mask_l, dom_l);
    def(ss_kern_host, kern_, CppDefType::Hdecl, acc_l, is_l, mask_l, dom_l,
        &ss_execs);
  }
}

void cpp_gen::iterate_spaces() {
  for (auto v : spaces) {
    DomVX::Object* n = get_vert(v);
    def(n);
  }
}

void cpp_gen::def(graphVX::VertexType* hn) {
  switch (hn->type()) {
    case CppImageE: {
      auto _im = obj2img(hn);
      def(ss_im, _im);
      break;
    }

    default: {
      ERRORM("cpp_writer::def : type of the VertexType" + hn->name() +
             "is not defined")
    }
  }
}

// TODO: merge with something else?
namespace cpp_abstraction_code_generator {
std::string to_string(ast4vx::Datatype d) {
  std::string datatype = "Unsupported Datatype";
  switch (d) {
    case ast4vx::Datatype::FLOAT:
      datatype = "float";
      break;
    case ast4vx::Datatype::UINT8:
      datatype = "unsigned char";
      break;
    case ast4vx::Datatype::INT8:
      datatype = "char";
      break;
    case ast4vx::Datatype::INT16:
      datatype = "short";
      break;
    case ast4vx::Datatype::INT32:
      datatype = "int";
      break;
    default:;
  }
  return datatype;
}

std::string generate_default_name(CppImage* im) { return "Image_" + im->id(); }

std::string generate_default_name(DomVX::Scalar* s) {
  return "Scalar_" + s->id();
}

std::string dump_code(std::shared_ptr<DomVX::OperatorNode> op,
                      std::vector<DomVX::Acc*> input_image_accessors,
                      std::vector<DomVX::Acc*> output_image_is,
                      std::vector<std::string> scalar_names,
                      bool use_default_names) {
  switch (op->operator_type) {
    case DomVX::OperatorType::PointOperator:
      return dump_code(std::dynamic_pointer_cast<DomVX::PointOperator>(op),
                       input_image_accessors, output_image_is, scalar_names,
                       use_default_names);
    case DomVX::OperatorType::LocalOperator:
      return dump_code(std::dynamic_pointer_cast<DomVX::LocalOperator>(op),
                       input_image_accessors, output_image_is, scalar_names,
                       use_default_names);
    case DomVX::OperatorType::GlobalOperator:
      return dump_code(std::dynamic_pointer_cast<DomVX::GlobalOperator>(op),
                       input_image_accessors, scalar_names, use_default_names);
    default:
      ERRORM("dump_code received non OperatorNode")
  }
}

std::string dump_code(std::shared_ptr<DomVX::PointOperator> point,
                      std::vector<DomVX::Acc*> input_image_accessors,
                      std::vector<DomVX::Acc*> output_image_is,
                      std::vector<std::string> scalar_names,
                      bool use_default_names) {
  CPPVisitor ast_visitor;

  ast_visitor.current_output_x = "y_" + point->id();
  ast_visitor.current_output_y = "x_" + point->id();
  std::string outer_loop =
      R"END(for(int @@@Y_NAME@@@ = 0; @@@Y_NAME@@@ < @@@HEIGHT@@@; @@@Y_NAME@@@++)
  {
     for (int @@@X_NAME@@@ = 0; @@@X_NAME@@@ < @@@WIDTH@@@; @@@X_NAME@@@++)
     {
         @@@CODE@@@
     }
  }
  )END";
  outer_loop =
      use_template(outer_loop, "HEIGHT", point->output_pixel_mappings[0]->h);
  outer_loop =
      use_template(outer_loop, "WIDTH", point->output_pixel_mappings[0]->w);
  outer_loop = use_template(outer_loop, "Y_NAME", ast_visitor.current_output_y);
  outer_loop = use_template(outer_loop, "X_NAME", ast_visitor.current_output_x);

  std::vector<std::string> mappings;

  for (auto& im : point->output_pixel_mappings) {
    if (use_default_names) {
      mappings.emplace_back(generate_default_name(im));
    } else {
      auto is_it = std::find_if(output_image_is.begin(), output_image_is.end(),
                                [=](DomVX::Acc* acc) { return acc->im == im; });
      if (is_it == output_image_is.end())
        throw std::runtime_error("Coudln't find IterationSpace for an Image");
      mappings.emplace_back((*is_it)->name());
    }
  }
  for (auto& im : point->input_pixel_mappings) {
    if (use_default_names) {
      mappings.emplace_back(generate_default_name(im));
    } else {
      auto acc_it = std::find_if(
          input_image_accessors.begin(), input_image_accessors.end(),
          [=](DomVX::Acc* acc) { return acc->im == im; });
      if (acc_it == input_image_accessors.end())
        throw std::runtime_error("Coudln't find Accessor for an Image");
      mappings.emplace_back((*acc_it)->name());
    }
  }

  std::vector<std::string> variable_mapping = scalar_names;
  if (use_default_names) {
    variable_mapping.clear();
    for (auto scalar : point->output_variable_mappings)
      variable_mapping.emplace_back(generate_default_name(scalar));
    for (auto scalar : point->input_variable_mappings)
      variable_mapping.emplace_back(generate_default_name(scalar));
  }
  auto old_variable_mapping = ast_visitor.variableaccessor_mapping;
  ast_visitor.variableaccessor_mapping = &variable_mapping;

  ast_visitor.pixelaccessor_mapping = &mappings;
  std::string code = ast_visitor.visit(point->get_statements(), 0);
  ast_visitor.pixelaccessor_mapping = nullptr;
  ast_visitor.variableaccessor_mapping = old_variable_mapping;

  ast_visitor.current_output_y = ast_visitor.current_output_x = "";

  outer_loop = use_template(outer_loop, "CODE", code);
  return outer_loop;
}

std::string dump_code(std::shared_ptr<DomVX::LocalOperator> local,
                      std::vector<DomVX::Acc*> input_image_accessors,
                      std::vector<DomVX::Acc*> output_image_is,
                      std::vector<std::string> scalar_names,
                      bool use_default_names) {
  CPPVisitor ast_visitor;
  ast_visitor.current_output_x = "y_" + local->id();
  ast_visitor.current_output_y = "x_" + local->id();

  std::vector<std::shared_ptr<ast4vx::WindowOperation>> w_ops;
  std::vector<
      std::tuple<DomVX::Image*, std::shared_ptr<ast4vx::WindowDescriptor>>>
      in_images;
  std::vector<DomVX::Image*> output_images;

  std::vector<std::shared_ptr<ast4vx::WindowOperation>> work_list =
      local->operations;

  ast_visitor.desc_to_dom.clear();
  while (!work_list.empty()) {
    auto op = work_list.back();
    work_list.pop_back();
    w_ops.insert(w_ops.begin(), op);

    output_images.insert(output_images.end(), op->out_image_binding.begin(),
                         op->out_image_binding.end());
    std::vector<std::shared_ptr<ast4vx::WindowDescriptor>> in_win =
        op->window_inputs;

    for (auto win : in_win) {
      // Check if it originates from a WindowOperation
      if (win->parent.use_count() > 0) {
        auto parent_op = win->parent.lock();
        bool already_in =
            std::find(w_ops.cbegin(), w_ops.cend(), parent_op) != w_ops.cend();
        already_in |= std::find(work_list.cbegin(), work_list.cend(),
                                parent_op) != work_list.cend();
        if (!already_in) work_list.push_back(parent_op);
      } else if (win->bounded.use_count() > 0) {
        auto image = win->bounded.lock()->im;
        bool already_in =
            std::find_if(
                in_images.cbegin(), in_images.cend(),
                [=](const std::tuple<DomVX::Image*,
                                     std::shared_ptr<ast4vx::WindowDescriptor>>&
                        a) { return std::get<0>(a) == image; }) !=
            in_images.cend();
        if (!already_in) {
          in_images.push_back({image, win});
          ast_visitor.desc_to_dom[win] = win->bounded.lock()->dom;
        }
      } else {
        auto it = std::find_if(
            local->input_descriptor.begin(), local->input_descriptor.end(),
            [=](std::tuple<DomVX::Image*,
                           std::shared_ptr<ast4vx::WindowDescriptor>>
                    item) { return std::get<1>(item) == win; });
        if (it != local->input_descriptor.end())
          in_images.push_back({std::get<0>(*it), win});
        else
          throw std::runtime_error(
              "Couldn't find an Image association to a Window Descriptor");
      }
    }
  }
  for (auto& pair : local->domain_bindings)
    ast_visitor.desc_to_dom[pair.first] = pair.second;

  // Get all the output images
  for (auto images_map : local->operation_output_images) {
    auto images = std::get<1>(images_map);
    output_images.insert(output_images.end(), images.begin(), images.end());
  }
  if (output_images.size() == 0)
    throw std::runtime_error(
        "CPPVisitor: LocalOperation: At least one output image is "
        "expected");

  std::string outer_loop =
      R"END(for(int @@@Y_NAME@@@ = 0; @@@Y_NAME@@@ < @@@HEIGHT@@@; @@@Y_NAME@@@++)
{
   for (int @@@X_NAME@@@ = 0; @@@X_NAME@@@ < @@@WIDTH@@@; @@@X_NAME@@@++)
   {
       @@@CODE@@@
   }
}
)END";
  outer_loop = use_template(outer_loop, "HEIGHT", output_images[0]->h);
  outer_loop = use_template(outer_loop, "WIDTH", output_images[0]->w);
  outer_loop = use_template(outer_loop, "Y_NAME", ast_visitor.current_output_y);
  outer_loop = use_template(outer_loop, "X_NAME", ast_visitor.current_output_x);
  std::string code = "";

  // Create the roi from the input images
  for (unsigned int i = 0; i < in_images.size(); i++) {
    auto& in_image = std::get<0>(in_images[i]);
    auto& in_desc = std::get<1>(in_images[i]);

    auto name = generate_default_name(in_image);
    if (!use_default_names) {
      auto acc_it = std::find_if(
          input_image_accessors.begin(), input_image_accessors.end(),
          [=](DomVX::Acc* acc) { return acc->im == in_image; });
      if (acc_it == input_image_accessors.end())
        throw std::runtime_error("Coudln't find Accessor for an Image");
      name = (*acc_it)->name();
    }

    std::string input_matrix_name =
        "temp_window_" + std::to_string(in_desc->id);

    std::string copy =
        "matrix<@@@DATATYPE@@@> @@@COPY_NAME@@@ = "
        "@@@ACCESSOR_NAME@@@.copy_roi<@@@DATATYPE@@@>(@@@X@@@, @@@Y@@@, "
        "@@@W@@@, @@@H@@@);\n";
    copy = use_template(copy, "COPY_NAME", input_matrix_name);
    copy = use_template(copy, "ACCESSOR_NAME", name);
    copy = use_template(copy, "DATATYPE", to_string(in_desc->output_datatype));
    copy = use_template(copy, "X",
                        ast_visitor.current_output_x + "-(" +
                            std::to_string(in_desc->width / 2) + ")");
    copy = use_template(copy, "Y",
                        ast_visitor.current_output_y + "-(" +
                            std::to_string(in_desc->height / 2) + ")");
    copy = use_template(copy, "W", std::to_string(in_desc->width));
    copy = use_template(copy, "H", std::to_string(in_desc->height));

    code += copy;
    ast_visitor.desc_to_name[in_desc] = input_matrix_name;
  }

  code += "\n";

  for (unsigned int i = 0; i < w_ops.size(); i++) {
    auto& op = w_ops[i];

    if (op->output != nullptr) {
      auto& in_desc = op->output;
      std::string matrix_name = "temp_window_" + std::to_string(in_desc->id);

      std::string copy =
          "matrix<@@@DATATYPE@@@> @@@COPY_NAME@@@ = "
          "@@@ORIG_NAME@@@.copy_roi<@@@DATATYPE@@@>(0, 0, @@@W@@@, "
          "@@@H@@@);\n";
      copy = use_template(copy, "COPY_NAME", matrix_name);
      copy = use_template(copy, "ORIG_NAME",
                          ast_visitor.desc_to_name[op->window_inputs[0]]);
      copy =
          use_template(copy, "DATATYPE", to_string(in_desc->output_datatype));
      copy = use_template(copy, "W", std::to_string(in_desc->width));
      copy = use_template(copy, "H", std::to_string(in_desc->height));

      code += copy;
      ast_visitor.desc_to_name[in_desc] = matrix_name;
    }

    std::vector<std::string> pixel_mappings;
    std::vector<std::string> window_mappings;
    std::vector<std::string> mask_mappings;

    if (local->mask_bindings.count(op) != 0)
      for (auto& mask : local->mask_bindings[op])
        mask_mappings.push_back(mask->name());
    for (auto& mask : op->mask_binding) mask_mappings.push_back(mask->name());

    // Setup all mappings
    if (op->current_state == ast4vx::WindowOperation::State::ToPixel ||
        op->current_state == ast4vx::WindowOperation::State::Reduce) {
      // Bind an actual output image is only relevant for ToPixel and Reduce
      for (auto& out_image : local->operation_output_images[op]) {
        auto im = out_image;
        auto name = generate_default_name(im);
        if (!use_default_names) {
          auto is_it =
              std::find_if(output_image_is.begin(), output_image_is.end(),
                           [=](DomVX::Acc* acc) { return acc->im == im; });
          if (is_it == output_image_is.end())
            throw std::runtime_error(
                "Coudln't find IterationSpace for an Image");
          name = (*is_it)->name();
        }
        pixel_mappings.emplace_back(name);
      }
      for (auto& out_image : op->out_image_binding) {
        auto im = out_image;
        auto name = generate_default_name(im);
        if (!use_default_names) {
          auto is_it =
              std::find_if(output_image_is.begin(), output_image_is.end(),
                           [=](DomVX::Acc* acc) { return acc->im == im; });
          if (is_it == output_image_is.end())
            throw std::runtime_error(
                "Coudln't find IterationSpace for an Image");
          name = (*is_it)->name();
        }
        pixel_mappings.emplace_back(name);
      }
      for (auto& windesc : op->window_inputs)
        window_mappings.emplace_back(ast_visitor.desc_to_name[windesc]);
      ast_visitor.windowdescriptor_mapping = &window_mappings;
    } else if (op->current_state == ast4vx::WindowOperation::State::At ||
               op->current_state == ast4vx::WindowOperation::State::Forall) {
      pixel_mappings.emplace_back(ast_visitor.desc_to_name[op->output]);
      for (auto& map : op->window_inputs) {
        pixel_mappings.emplace_back(ast_visitor.desc_to_name[map]);
      }
    }

    std::vector<std::string> variable_mapping = scalar_names;
    if (use_default_names) {
      variable_mapping.clear();
      for (auto scalar : local->operation_variables[op])
        variable_mapping.emplace_back(generate_default_name(scalar));
    }
    auto old_variable_mapping = ast_visitor.variableaccessor_mapping;
    ast_visitor.variableaccessor_mapping = &variable_mapping;

    auto old_mapping = ast_visitor.pixelaccessor_mapping;
    ast_visitor.pixelaccessor_mapping = &pixel_mappings;
    ast_visitor.maskaccessor_mapping = &mask_mappings;

    code += ast_visitor.visit(op) + "\n";

    ast_visitor.maskaccessor_mapping = nullptr;
    ast_visitor.pixelaccessor_mapping = old_mapping;
    ast_visitor.windowdescriptor_mapping = nullptr;
    ast_visitor.variableaccessor_mapping = old_variable_mapping;

    // If the output window has no domain set, copy it from the input window
    // descriptor
    if (op->output != nullptr && ast_visitor.desc_to_dom.find(op->output) ==
                                     ast_visitor.desc_to_dom.end()) {
      if (op->window_inputs.size() == 0)
        throw std::runtime_error(
            "CPPVisitor: if operation output window has no domain set "
            "explicitly and should inherit the domain from the input, an "
            "input window has to be given");
      ast_visitor.desc_to_dom[op->output] =
          ast_visitor.desc_to_dom[op->window_inputs[0]];
    }
  }

  ast_visitor.desc_to_dom.clear();

  ast_visitor.current_output_y = ast_visitor.current_output_x = "";
  outer_loop = use_template(outer_loop, "CODE", code);
  return outer_loop;
}

std::string dump_code(std::shared_ptr<DomVX::GlobalOperator> global,
                      std::vector<CppAccessor*> input_image_accessors,
                      std::vector<std::string> scalar_names,
                      bool use_default_names) {
  if (global->input_pixel_mappings.size() == 0)
    throw std::runtime_error(
        "CPPVisitor: GlobalOperation: At least one input image is "
        "expected");

  CPPVisitor ast_visitor;
  ast_visitor.current_output_x = "y_" + global->id();
  ast_visitor.current_output_y = "x_" + global->id();

  std::string pre_loop = "";

  std::string accum_var_name = "accum_" + global->id();

  // Setup the accumulator variable
  auto accum_var = std::make_shared<ast4vx::Variable>();
  if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<int>>(
          global->reduction->initial))
    accum_var->datatype = ast4vx::Datatype::INT32;
  else if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<float>>(
               global->reduction->initial))
    accum_var->datatype = ast4vx::Datatype::FLOAT;
  else if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<unsigned char>>(
               global->reduction->initial))
    accum_var->datatype = ast4vx::Datatype::UINT8;
  else if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<unsigned int>>(
               global->reduction->initial))
    accum_var->datatype = ast4vx::Datatype::UINT32;
  else
    throw std::runtime_error(
        "CPPVisitor: GlobalOperation: reduce: could not determine the "
        "constants type");
  accum_var->name = accum_var_name;

  pre_loop += ast_visitor.visit(
                  std::make_shared<ast4vx::VariableDeclaration>(accum_var)) +
              ";\n";
  pre_loop +=
      ast_visitor.visit(assign(accum_var, global->reduction->initial)) + ";\n";

  ast_visitor.accumulator_string = accum_var_name;

  auto acc_name = generate_default_name(global->input_pixel_mappings[0]);
  if (!use_default_names) {
    auto acc_it =
        std::find_if(input_image_accessors.begin(), input_image_accessors.end(),
                     [=](DomVX::Acc* acc) {
                       return acc->im == global->input_pixel_mappings[0];
                     });
    if (acc_it == input_image_accessors.end())
      throw std::runtime_error("Coudln't find Accessor for the Image");
    acc_name = (*acc_it)->name();
  }

  std::vector<std::string> pixel_mappings;
  pixel_mappings.emplace_back(acc_name);
  auto old_pixel_mapping = ast_visitor.pixelaccessor_mapping;
  ast_visitor.pixelaccessor_mapping = &pixel_mappings;

  std::string reduction_body = ast_visitor.visit(global->reduction);

  ast_visitor.pixelaccessor_mapping = old_pixel_mapping;

  std::string outer_loop =
      R"END(for(int @@@Y_NAME@@@ = 0; @@@Y_NAME@@@ < @@@HEIGHT@@@; @@@Y_NAME@@@++)
  {
     for (int @@@X_NAME@@@ = 0; @@@X_NAME@@@ < @@@WIDTH@@@; @@@X_NAME@@@++)
     {
         @@@CODE@@@
     }
  }
  )END";
  outer_loop =
      use_template(outer_loop, "HEIGHT", global->input_pixel_mappings[0]->h);
  outer_loop =
      use_template(outer_loop, "WIDTH", global->input_pixel_mappings[0]->w);
  outer_loop = use_template(outer_loop, "Y_NAME", ast_visitor.current_output_y);
  outer_loop = use_template(outer_loop, "X_NAME", ast_visitor.current_output_x);

  std::vector<std::string> variable_mapping = scalar_names;
  if (use_default_names) {
    variable_mapping.clear();
    variable_mapping.emplace_back(generate_default_name(global->reduction_out));
  }
  auto old_variable_mapping = ast_visitor.variableaccessor_mapping;
  ast_visitor.variableaccessor_mapping = &variable_mapping;

  auto p2p = create_p2p(1, 0);
  std::string post_loop = ast_visitor.visit(
      p2p << assign(std::make_shared<ast4vx::VariableAccessor>(0), accum_var));
  ast_visitor.variableaccessor_mapping = old_variable_mapping;

  outer_loop = use_template(outer_loop, "CODE", reduction_body);
  return pre_loop + "\n" + outer_loop + "\n" + post_loop + "\n";
}
};  // namespace cpp_abstraction_code_generator
