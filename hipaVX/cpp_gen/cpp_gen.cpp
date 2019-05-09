#include "cpp_gen.hpp"

// TODO: use tuples
string generate_image_name(DomVX::Image* image) {
  return string("Image_") + image->id();
}
string generate_scalar_name(DomVX::Scalar* scalar) {
  return string("Scalar_") + scalar->id();
}

namespace generator {
string node_generator(DomVX::WriteImageNode* n, Type t) {
  if (t == Type::Definition) {
    return "";
  } else if (t == Type::Call) {
    string s =
        "\t@@@IMAGE_DATATYPE@@@ *data_@@@IMAGE@@@_@@@ID@@@ = "
        "@@@IMAGE@@@.data.data();\n";
    s += "\tsave_data(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, "
         "@@@IMAGE_CHANNELS@@@, @@@CONVERSION@@@ data_@@@IMAGE@@@_@@@ID@@@, "
         "\"@@@FILENAME@@@\");\n";
    s += "\n";

    s = use_template(s, "ID", n->id());
    s = use_template(s, "IMAGE", generate_image_name(n->in));
    s = use_template(s, "IMAGE_DATATYPE", VX_DF_IMAGE_to_cpp[n->in->col]);

    s = use_template(s, "IMAGE_WIDTH", n->in->w);
    s = use_template(s, "IMAGE_HEIGHT", n->in->h);
    if (n->in->col == VX_DF_IMAGE_RGBX) {
      s = use_template(s, "IMAGE_CHANNELS", 4);
      s = use_template(s, "CONVERSION", "(unsigned char*)");
    } else {
      s = use_template(s, "IMAGE_CHANNELS", 1);  // TODO
      s = use_template(s, "CONVERSION", "");
    }
    s = use_template(s, "FILENAME", n->out_file);

    return s;
  }
  return "SOMETHING IS WRONG";
}

}  // namespace generator

namespace ast4vx {

std::string to_string(Datatype d) {
  std::string datatype = "Unsupported Datatype";
  switch (d) {
    case Datatype::FLOAT:
      datatype = "float";
      break;
    case Datatype::UINT8:
      datatype = "unsigned char";
      break;
    case Datatype::INT8:
      datatype = "char";
      break;
    case Datatype::INT16:
      datatype = "short";
      break;
    case Datatype::INT32:
      datatype = "int";
      break;
  }
  return datatype;
}

}  // namespace ast4vx

// move this to domVX_main.cpp
std::vector<DomVX::Image*> get_all_images(DomVX::Graph* g) {
  std::vector<DomVX::Image*> images;
  std::vector<DomVX::Node*> nodes = g->graph;

  while (!nodes.empty()) {
    auto node = nodes[nodes.size() - 1];
    nodes.pop_back();

    for (auto ref : node->inputs) {
      if (ref->type() == VX_TYPE_IMAGE) images.push_back((DomVX::Image*)ref);
    }
    for (auto ref : node->outputs) {
      if (ref->type() == VX_TYPE_IMAGE) images.push_back((DomVX::Image*)ref);
    }

    auto subnodes = node->subnodes;
    nodes.insert(nodes.end(), subnodes.begin(), subnodes.end());
  }

  std::sort(images.begin(), images.end());
  images.erase(std::unique(images.begin(), images.end()), images.end());

  return images;
}

std::string generate_source_recursive(std::vector<DomVX::Node*> nodes,
                                      const generator::Type t) {
  if (t == generator::Type::Definition) return "";

  string sources = "";
  for (auto node : nodes) {
    if (dynamic_cast<DomVX::WriteImageNode*>(node) != nullptr) {
      if (t == generator::Type::Call)
        sources += generator::node_generator(
            dynamic_cast<DomVX::WriteImageNode*>(node), t);
      continue;
    }
    auto subnodes = node->subnodes;
    if (subnodes.empty())  // we are at a fundamental node
    {
      CPPVisitor v;
      // VISIT the NODE "node"
    } else {
      sources += generate_source_recursive(subnodes, t) + '\n';
    }
  }
  return sources;
}

void process_graph(DomVX::Graph* graph) {
  string main = read_file(hipaVX_folder + "/templates/cpp_main.templ");

  string images;
  const string image_decl_template =
      "\tmatrix<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, "
      "@@@IMAGE_HEIGHT@@@);\n";

  auto used_images = get_all_images(graph);
  for (const auto image : used_images) {
    string image_name = ::generate_image_name(image);

    string temp = image_decl_template;

    // BAD HACKs
    const DomVX::FileinputImage* fim =
        dynamic_cast<const DomVX::FileinputImage*>(image);
    const DomVX::Array* arr = dynamic_cast<const DomVX::Array*>(image);
    if (fim) {
      string fim_decl_template;
      if (fim->col == VX_DF_IMAGE_RGBX) {
        fim_decl_template =
            "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = (@@@DATATYPE@@@*) "
            "load_data<unsigned char>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, "
            "4, \"@@@FILENAME@@@\");\n"
            "\tmatrix<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, "
            "@@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
      } else {
        fim_decl_template =
            "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = "
            "load_data<@@@DATATYPE@@@>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, "
            "1, \"@@@FILENAME@@@\");\n"
            "\tmatrix<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, "
            "@@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
      }

      temp = fim_decl_template;
      temp = use_template(temp, "FILENAME", fim->file);
    } else if (arr) {
      continue;  // Currently arr is only used in Harris corner
      // There is a bug where reads of images after writes are not transpiled
      // correctly Therefore i write to this array when constructing ->
      // constructor has to be called later
    }

    temp = use_template(temp, "DATATYPE", VX_DF_IMAGE_to_cpp[image->col]);
    temp = use_template(temp, "IMAGE_NAME", image_name);
    temp = use_template(temp, "IMAGE_WIDTH", image->w);
    temp = use_template(temp, "IMAGE_HEIGHT", image->h);

    images += temp;
  }

  main = use_template(main, "IMAGE_DEFINITIONS", images);

  string node_definitions =
      generate_source_recursive(graph->graph, generator::Type::Definition);

  main = use_template(main, "KERNEL_DEFINITIONS", node_definitions);

  string node_calls =
      generate_source_recursive(graph->graph, generator::Type::Call);

  main = use_template(main, "KERNEL_CALLS", node_calls);

  _write_to_file("main.hipaVX.cpp", main);
}

std::string CPPVisitor::visit(std::shared_ptr<ast4vx::Node> n, int i) {
  switch (n->type) {
    case ast4vx::NodeType::None:
      throw std::runtime_error("CPPVisitor visited None");
    case ast4vx::NodeType::Add:
    case ast4vx::NodeType::Sub:
    case ast4vx::NodeType::Mul:
    case ast4vx::NodeType::Div:
    case ast4vx::NodeType::ShiftLeft:
    case ast4vx::NodeType::ShiftRight:
    case ast4vx::NodeType::Less:
    case ast4vx::NodeType::LessEquals:
    case ast4vx::NodeType::Equals:
    case ast4vx::NodeType::GreaterEquals:
    case ast4vx::NodeType::Greater:
    case ast4vx::NodeType::Unequals:
    case ast4vx::NodeType::And:
    case ast4vx::NodeType::Or:
    case ast4vx::NodeType::Xor:
    case ast4vx::NodeType::BitwiseAnd:
    case ast4vx::NodeType::BitwiseOr:
    case ast4vx::NodeType::BitwiseXor: {
      auto s = std::dynamic_pointer_cast<ast4vx::SimpleBinaryNode>(n);
      std::string op;

      switch (s->type) {
        case ast4vx::NodeType::Add:
          op = "+";
          break;
        case ast4vx::NodeType::Sub:
          op = "-";
          break;
        case ast4vx::NodeType::Mul:
          op = "*";
          break;
        case ast4vx::NodeType::Div:
          op = "/";
          break;
        case ast4vx::NodeType::ShiftLeft:
          op = "<<";
          break;
        case ast4vx::NodeType::ShiftRight:
          op = ">>";
          break;
        case ast4vx::NodeType::And:
          op = "&&";
          break;
        case ast4vx::NodeType::Or:
          op = "||";
          break;
        case ast4vx::NodeType::Xor:
          throw std::runtime_error(
              "std::string generate(SimpleBinaryNode *s): No logical XOR "
              "available");
        case ast4vx::NodeType::BitwiseAnd:
          op = "&";
          break;
        case ast4vx::NodeType::BitwiseOr:
          op = "|";
          break;
        case ast4vx::NodeType::BitwiseXor:
          op = "^";
          break;
        case ast4vx::NodeType::Less:
          op = "<";
          break;
        case ast4vx::NodeType::LessEquals:
          op = "<=";
          break;
        case ast4vx::NodeType::Equals:
          op = "==";
          break;
        case ast4vx::NodeType::GreaterEquals:
          op = ">=";
          break;
        case ast4vx::NodeType::Greater:
          op = ">";
          break;
        case ast4vx::NodeType::Unequals:
          op = "!=";
          break;
      }

      auto left = this->visit(s->subnodes[0], 0);
      auto right = this->visit(s->subnodes[1], 0);

      return "(" + left + " " + op + " " + right + ")";
    }

    case ast4vx::NodeType::BitwiseNot:
    case ast4vx::NodeType::Not:
    case ast4vx::NodeType::Sqrt:
    case ast4vx::NodeType::Exp:
    case ast4vx::NodeType::Conversion:
    case ast4vx::NodeType::Abs:
    case ast4vx::NodeType::Atan2: {
      auto s = std::dynamic_pointer_cast<ast4vx::SimpleUnaryFunctionNode>(n);
      std::string func;

      switch (s->type) {
        case ast4vx::NodeType::Sqrt:
          func = "sqrt";
          break;
        case ast4vx::NodeType::Exp:
          func = "exp";
          break;
        case ast4vx::NodeType::Atan2:
          func = "atan2";
          break;
        case ast4vx::NodeType::Abs:
          func = "abs";
          break;
        case ast4vx::NodeType::Not:
          func = "!";
          break;
        case ast4vx::NodeType::BitwiseNot:
          func = "~";
          break;
        case ast4vx::NodeType::Conversion:
          func =
              "(" +
              to_string(std::dynamic_pointer_cast<ast4vx::Conversion>(s)->to) +
              ")";
          break;
      }

      auto argument = this->visit(s->subnodes[0], 0);

      return func + "(" + argument + ")";
    }

    case ast4vx::NodeType::Constant: {
      if (auto c = dynamic_cast<ast4vx::Constant<float>*>(n.get())) {
        return std::to_string(c->value);
      } else if (auto c =
                     dynamic_cast<ast4vx::Constant<unsigned char>*>(n.get())) {
        return std::to_string(c->value);
      } else if (auto c =
                     dynamic_cast<ast4vx::Constant<unsigned int>*>(n.get())) {
        return std::to_string(c->value);
      } else if (auto c = dynamic_cast<ast4vx::Constant<int>*>(n.get())) {
        return std::to_string(c->value);
      }
      return "CPP Generate: Constant type fail";
    }

    case ast4vx::NodeType::Vect4: {
      auto s = std::dynamic_pointer_cast<ast4vx::Vect4>(n);
      std::string func = "";

      switch (s->to_dtype) {
        case ast4vx::Datatype::UCHAR4:
          func = "uchar4";
          break;
        case ast4vx::Datatype::UINT4:
          func = "uint4";
          break;
        default:
          std::cerr << "AST: Vect4 is called with an unsupported datatype: "
                    << std::endl;
          exit(1);
          break;
      }

      auto argument = this->visit(s->subnodes[0], 0) + ", " +
                      this->visit(s->subnodes[1], 0) + ", " +
                      this->visit(s->subnodes[2], 0) + ", " +
                      this->visit(s->subnodes[3], 0);
      return func + "(" + argument + ")";
    }

    case ast4vx::NodeType::Extract4: {
      auto s = std::dynamic_pointer_cast<ast4vx::Extract4>(n);
      string channel;
      switch (s->channel) {
        case ast4vx::VectChannelType::CHANNEL0:
          channel = "0";
          break;
        case ast4vx::VectChannelType::CHANNEL1:
          channel = "1";
          break;
        case ast4vx::VectChannelType::CHANNEL2:
          channel = "2";
          break;
        case ast4vx::VectChannelType::CHANNEL3:
          channel = "3";
          break;
        default:
          throw std::runtime_error(
              "AST: Vect4 is called with an unsupported datatype");
      }

      auto argument = this->visit(s->subnodes[0], 0);
      return argument + ".arr[" + channel + "]";
    }

    case ast4vx::NodeType::Variable: {
      return std::dynamic_pointer_cast<ast4vx::Variable>(n)->name;
    }

    case ast4vx::NodeType::VariableDeclaration: {
      auto s = std::dynamic_pointer_cast<ast4vx::VariableDeclaration>(n);
      std::string datatype =
          to_string(std::dynamic_pointer_cast<ast4vx::Variable>(s->subnodes[0])
                        ->datatype);
      return datatype + " " + this->visit(s->subnodes[0], 0);
    }

    case ast4vx::NodeType::Assignment: {
      auto s = std::dynamic_pointer_cast<ast4vx::Assignment>(n);

      if (s->subnodes[0]->type == ast4vx::NodeType::PixelAccessor) {
        auto accessor =
            std::dynamic_pointer_cast<ast4vx::PixelAccessor>(s->subnodes[0]);

        std::string name = "PixelAccessor_" + std::to_string(accessor->num);
        std::string x = "x";
        std::string y = "y";
        if (pixelaccessor_mapping) {
          name = pixelaccessor_mapping->at(accessor->num);
        }
        if (current_output_x != "") x = current_output_x;
        if (current_output_y != "") y = current_output_y;

        if (accumulator_string == "" || name != accumulator_string)
          return name + ".write(" + x + ", " + y + ", " +
                 this->visit(s->subnodes[1], 0) + ")";
        // else: fall through
      }

      auto left = this->visit(s->subnodes[0], 0);
      auto right = this->visit(s->subnodes[1], 0);

      return left + " = " + right;
    }

    case ast4vx::NodeType::If: {
      auto s = std::dynamic_pointer_cast<ast4vx::If>(n);
      std::string to_return;

      to_return += "if (" + this->visit(s->condition, 0) + ")\n";
      to_return += "{\n";
      to_return += this->visit(s->body, 0);
      to_return += "}\n";

      return to_return;
    }

    case ast4vx::NodeType::Else: {
      auto s = std::dynamic_pointer_cast<ast4vx::Else>(n);
      std::string to_return;

      to_return += "else\n";
      to_return += "{\n";
      to_return += this->visit(s->body, 0);
      to_return += "}\n";

      return to_return;
    }

    case ast4vx::NodeType::Statements: {
      auto s = std::dynamic_pointer_cast<ast4vx::Statements>(n);
      std::string to_return;

      for (auto statement : s->statements) {
        to_return += this->visit(statement, i);
        if (statement->type != ast4vx::NodeType::If &&
            statement->type != ast4vx::NodeType::Else)
          to_return += ";\n";
      }

      return to_return;
    }

    case ast4vx::NodeType::VariableAccessor: {
      auto s = std::dynamic_pointer_cast<ast4vx::VariableAccessor>(n);

      if (variableaccessor_mapping == nullptr)
        return "Variable_Accessor_" + std::to_string(s->id);
      return variableaccessor_mapping->at(s->num);
    }

    case ast4vx::NodeType::PixelAccessor: {
      auto s = std::dynamic_pointer_cast<ast4vx::PixelAccessor>(n);

      std::string name = "PixelAccessor_" + std::to_string(s->num);
      std::string x = "x";
      std::string y = "y";
      if (pixelaccessor_mapping) name = pixelaccessor_mapping->at(s->num);
      if (current_output_x != "") x = current_output_x;
      if (current_output_y != "") y = current_output_y;

      if (accumulator_string != "" && name == accumulator_string) return name;

      return name + ".get(" + x + ", " + y + ")";
    }

    case ast4vx::NodeType::MaskAccessor: {
      auto s = std::dynamic_pointer_cast<ast4vx::MaskAccessor>(n);

      std::string name = maskaccessor_mapping->at(s->num);
      std::string x = current_output_x;
      std::string y = current_output_y;

      return name + ".get(" + x + ", " + y + ")";
    }

    case ast4vx::NodeType::WindowOperation: {
      auto s = std::dynamic_pointer_cast<ast4vx::WindowOperation>(n);

      if (s->current_state == ast4vx::WindowOperation::State::ToPixel)
        return visit(s->ltp_statement);

      std::string code = "";

      std::shared_ptr<ast4vx::Variable> accum_var;
      if (s->current_state == ast4vx::WindowOperation::State::Reduce) {
        // Setup the accumulator variable
        accum_var = std::make_shared<ast4vx::Variable>();
        if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<int>>(
                s->reduction_statement->initial))
          accum_var->datatype = ast4vx::Datatype::INT32;
        else if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<float>>(
                     s->reduction_statement->initial))
          accum_var->datatype = ast4vx::Datatype::FLOAT;
        else if (auto c =
                     std::dynamic_pointer_cast<ast4vx::Constant<unsigned char>>(
                         s->reduction_statement->initial))
          accum_var->datatype = ast4vx::Datatype::UINT8;
        else if (auto c =
                     std::dynamic_pointer_cast<ast4vx::Constant<unsigned int>>(
                         s->reduction_statement->initial))
          accum_var->datatype = ast4vx::Datatype::UINT32;
        else
          throw std::runtime_error(
              "CPPVisitor: WindowOperation: reduce: could not determine the "
              "constants type");
        accumulator_string =
            "accumulator_" + std::to_string(s->reduction_statement->id);
        accum_var->name = accumulator_string;

        code +=
            visit(std::make_shared<ast4vx::VariableDeclaration>(accum_var)) +
            ";\n";
        code +=
            visit(assign(accum_var, s->reduction_statement->initial)) + ";\n";
      }

      // TODO check window_inputs if same domain
      auto domain = desc_to_dom[s->window_inputs[i]];

      for (unsigned int y = 0; y < domain->height; y++) {
        for (unsigned int x = 0; x < domain->width; x++) {
          if (domain->domain[y][x] == 0) continue;
          if (s->current_state == ast4vx::WindowOperation::State::At) {
            if (s->statements[y][x].get() == nullptr) continue;
            if (s->statements[y][x]->out_pixel_mappings.size() != 1)
              throw std::runtime_error(
                  "CPPVisitor: WindowOperation: At operations requires exactly "
                  "one output mapping");
          }
          auto old_x = current_output_x;
          auto old_y = current_output_y;
          current_output_x = std::to_string(x);
          current_output_y = std::to_string(y);

          if (s->current_state == ast4vx::WindowOperation::State::At)
            code += visit(s->statements[y][x]);
          else if (s->current_state == ast4vx::WindowOperation::State::Forall)
            code += visit(s->forall_statement);
          else if (s->current_state == ast4vx::WindowOperation::State::Reduce)
            code += visit(s->reduction_statement);

          current_output_x = old_x;
          current_output_y = old_y;
        }
      }

      // Write back the Accumulator
      if (s->current_state == ast4vx::WindowOperation::State::Reduce) {
        code += visit(
            assign(std::make_shared<ast4vx::PixelAccessor>(0), accum_var));
      }

      return code;
    }

    case ast4vx::NodeType::LocalToPixel: {
      auto s = std::dynamic_pointer_cast<ast4vx::LocalToPixel>(n);
      std::string to_return;
      for (auto statement : s->statements) {
        to_return += this->visit(statement, i);
        if (statement->type != ast4vx::NodeType::If &&
            statement->type != ast4vx::NodeType::Else)
          to_return += ";\n";
      }
      return to_return;
    }

    case ast4vx::NodeType::MaskPixelToPixel: {
      auto s = std::dynamic_pointer_cast<ast4vx::MaskPixelToPixel>(n);
      std::string to_return;
      for (auto statement : s->statements) {
        to_return += this->visit(statement, i);
        if (statement->type != ast4vx::NodeType::If &&
            statement->type != ast4vx::NodeType::Else)
          to_return += ";\n";
      }
      return to_return;
    }

    case ast4vx::NodeType::Reduction: {
      auto s = std::dynamic_pointer_cast<ast4vx::Reduction>(n);
      std::string to_return;

      std::vector<std::string> mapping;

      mapping.emplace_back(accumulator_string);
      mapping.emplace_back(accumulator_string);
      if (windowdescriptor_mapping != nullptr)
        mapping.emplace_back(windowdescriptor_mapping->at(0));
      else
        mapping.emplace_back(pixelaccessor_mapping->at(0));

      auto old_pixel_mapping = pixelaccessor_mapping;
      pixelaccessor_mapping = &mapping;

      for (auto statement : s->statements) {
        to_return += this->visit(statement, i);
        if (statement->type != ast4vx::NodeType::If &&
            statement->type != ast4vx::NodeType::Else)
          to_return += ";\n";
      }

      pixelaccessor_mapping = old_pixel_mapping;

      return to_return;
    }

    case ast4vx::NodeType::WindowAccessorPosition: {
      auto s = std::dynamic_pointer_cast<ast4vx::WindowAccessor::Position>(n);
      auto parent = s->parent.lock();

      if (windowdescriptor_mapping == nullptr)
        return "Window" + std::to_string(parent->num) + "(" +
               std::to_string(s->x) + ", " + std::to_string(s->y) + ")";

      return windowdescriptor_mapping->at(parent->num) + ".get(" +
             std::to_string(s->x) + ", " + std::to_string(s->y) + ")";
    }
  }
  throw std::runtime_error("CPP Generate: reached end of switch case");
}

std::string create_matrix_mask_def(std::shared_ptr<DomVX::Mask> mask,
                                   std::string name) {
  std::string values = "{";
  std::string vec;
  unsigned int i = 0;
  unsigned long size = mask->mask.size() * mask->mask[0].size();
  for (auto& line : mask->mask) {
    for (auto value : line) {
      if (mask->mask_is_int)
        values += std::to_string(value.i);
      else
        values += std::to_string(value.f);
      if (i != size - 1) values += ", ";
      i++;
    }
  }
  values += "}";

  std::string mask_def =
      "std::matrix<@@@DATATYPE@@@> @@@MASK_NAME@@@(@@@W@@@, @@@H@@@, "
      "@@@VALUES@@@);\n";
  mask_def = use_template(mask_def, "MASK_NAME", name);
  mask_def = use_template(mask_def, "VALUES", values);
  mask_def =
      use_template(mask_def, "DATATYPE", (mask->mask_is_int) ? "int" : "float");
  mask_def = use_template(mask_def, "W", std::to_string(mask->mask.size()));
  mask_def = use_template(mask_def, "H", std::to_string(mask->mask[0].size()));

  return mask_def;
}
std::string create_matrix_def(std::string type, std::string name, std::string w,
                              std::string h) {
  std::string matrix_def =
      "std::matrix<@@@DATATYPE@@@> @@@MATRIXNAME@@@(@@@WIDTH@@@, "
      "@@@HEIGHT@@@);\n";
  matrix_def = use_template(matrix_def, "DATATYPE", type);
  matrix_def = use_template(matrix_def, "MATRIXNAME", name);
  matrix_def = use_template(matrix_def, "WIDTH", w);
  matrix_def = use_template(matrix_def, "HEIGHT", h);
  return matrix_def;
}
std::string create_matrix_def(std::string type, std::string name,
                              unsigned int w, unsigned int h) {
  return create_matrix_def(type, name, std::to_string(w), std::to_string(h));
}

std::string CPPVisitor::setup_outer_loop(std::shared_ptr<DomVX::PointOperator> m) {
  std::string y_index_name = "y_" + m->id();
  std::string x_index_name = "x_" + m->id();
  std::string templ =
      R"END(for(int @@@Y_NAME@@@ = 0; @@@Y_NAME@@@ < @@@HEIGHT@@@; @@@Y_NAME@@@++)
{
    for (int @@@X_NAME@@@ = 0; @@@X_NAME@@@ < @@@WIDTH@@@; @@@X_NAME@@@++)
    {
        @@@CODE@@@
    }
}
)END";
  templ = use_template(templ, "HEIGHT", m->output_pixel_mappings[0]->h);
  templ = use_template(templ, "WIDTH", m->output_pixel_mappings[0]->w);
  templ = use_template(templ, "Y_NAME", y_index_name);
  templ = use_template(templ, "X_NAME", x_index_name);

  current_output_y = y_index_name;
  current_output_x = x_index_name;

  return templ;
}
std::string CPPVisitor::setup_outer_loop(
    std::shared_ptr<DomVX::LocalOperator> l,
    const std::vector<DomVX::Image*>& out) {
  std::string y_index_name = "y_" + l->id();
  std::string x_index_name = "x_" + l->id();
  std::string templ =
      R"END(for(int @@@Y_NAME@@@ = 0; @@@Y_NAME@@@ < @@@HEIGHT@@@; @@@Y_NAME@@@++)
{
    for (int @@@X_NAME@@@ = 0; @@@X_NAME@@@ < @@@WIDTH@@@; @@@X_NAME@@@++)
    {
        @@@CODE@@@
    }
}
)END";
  templ = use_template(templ, "HEIGHT", out[0]->h);
  templ = use_template(templ, "WIDTH", out[0]->w);
  templ = use_template(templ, "Y_NAME", y_index_name);
  templ = use_template(templ, "X_NAME", x_index_name);

  current_output_y = y_index_name;
  current_output_x = x_index_name;

  return templ;
}
std::string CPPVisitor::setup_outer_loop(
    std::shared_ptr<DomVX::GlobalOperator> g,
    const std::vector<DomVX::Image*>& in) {
  std::string y_index_name = "y_" + g->id();
  std::string x_index_name = "x_" + g->id();
  std::string templ =
      R"END(for(int @@@Y_NAME@@@ = 0; @@@Y_NAME@@@ < @@@HEIGHT@@@; @@@Y_NAME@@@++)
{
    for (int @@@X_NAME@@@ = 0; @@@X_NAME@@@ < @@@WIDTH@@@; @@@X_NAME@@@++)
    {
        @@@CODE@@@
    }
}
)END";
  templ = use_template(templ, "HEIGHT", in[0]->h);
  templ = use_template(templ, "WIDTH", in[0]->w);
  templ = use_template(templ, "Y_NAME", y_index_name);
  templ = use_template(templ, "X_NAME", x_index_name);

  current_output_y = y_index_name;
  current_output_x = x_index_name;

  return templ;
}

std::string CPPVisitor::visit(std::shared_ptr<DomVX::AbstractionNode> _n,
                              int i) {
  auto n = std::dynamic_pointer_cast<DomVX::OperatorNode>(_n);
  switch (n->operator_type) {
    case DomVX::OperatorType::PointOperator: {
      auto s = std::dynamic_pointer_cast<DomVX::PointOperator>(n);

      std::string outer_loop = setup_outer_loop(s);

      std::vector<std::string> mappings;

      for (auto& im : s->output_pixel_mappings)
        mappings.emplace_back(generate_image_name(im));
      for (auto& im : s->input_pixel_mappings)
        mappings.emplace_back(generate_image_name(im));

      std::vector<std::string> variable_mappings;
      for (auto& im : s->output_variable_mappings)
        variable_mappings.emplace_back(generate_scalar_name(im));
      for (auto& im : s->input_variable_mappings)
        variable_mappings.emplace_back(generate_scalar_name(im));
      auto old_variable_mapping = variableaccessor_mapping;
      variableaccessor_mapping = &variable_mappings;

      pixelaccessor_mapping = &mappings;
      std::string code = visit(s->get_statements(), 0);
      pixelaccessor_mapping = nullptr;
      variableaccessor_mapping = old_variable_mapping;

      current_output_y = current_output_x = "";

      outer_loop = use_template(outer_loop, "CODE", code);
      return outer_loop;
    }
    case DomVX::OperatorType::LocalOperator: {
      auto s = std::dynamic_pointer_cast<DomVX::LocalOperator>(n);

      std::vector<DomVX::Image*> output_images;

      // Get all the output images
      for (auto images : s->operation_output_images) {
        output_images.insert(output_images.end(), images.begin(), images.end());
      }
      if (output_images.size() == 0)
        throw std::runtime_error(
            "CPPVisitor: LocalOperation: At least one output image is "
            "expected");
      std::string outer_loop = setup_outer_loop(s, output_images);
      std::string code = "";

      // Create the roi from the input images
      for (unsigned int i = 0; i < s->input_descriptor.size(); i++) {
        auto& in_image = std::get<0>(s->input_descriptor[i]);
        auto& in_desc = std::get<1>(s->input_descriptor[i]);
        std::string input_matrix_name =
            "temp_window_" + std::to_string(in_desc->id);

        std::string copy =
            "std::matrix<@@@DATATYPE@@@> @@@COPY_NAME@@@ = "
            "@@@IMAGE_NAME@@@.copy_roi<@@@DATATYPE@@@>(@@@X@@@, @@@Y@@@, "
            "@@@W@@@, @@@H@@@);\n";
        copy = use_template(copy, "COPY_NAME", input_matrix_name);
        copy = use_template(copy, "IMAGE_NAME", generate_image_name(in_image));
        copy =
            use_template(copy, "DATATYPE", to_string(in_desc->output_datatype));
        copy = use_template(
            copy, "X",
            current_output_x + "-(" + std::to_string(in_desc->width / 2) + ")");
        copy = use_template(copy, "Y",
                            current_output_y + "-(" +
                                std::to_string(in_desc->height / 2) + ")");
        copy = use_template(copy, "W", std::to_string(in_desc->width));
        copy = use_template(copy, "H", std::to_string(in_desc->height));

        code += copy;
        desc_to_name[in_desc] = input_matrix_name;
      }

      code += "\n";

      desc_to_dom.clear();
      // Add the manual set domains to the map
      for (auto binding : s->domain_bindings)
        desc_to_dom[binding.first] = binding.second;

      for (unsigned int i = 0; i < s->operations.size(); i++) {
        auto& op = s->operations[i];

        if (op->output != nullptr) {
          auto& in_desc = op->output;
          std::string matrix_name =
              "temp_window_" + std::to_string(in_desc->id);

          std::string copy =
              "std::matrix<@@@DATATYPE@@@> @@@COPY_NAME@@@ = "
              "@@@ORIG_NAME@@@.copy_roi<@@@DATATYPE@@@>(0, 0, @@@W@@@, "
              "@@@H@@@);\n";
          copy = use_template(copy, "COPY_NAME", matrix_name);
          copy = use_template(copy, "ORIG_NAME",
                              desc_to_name[op->window_inputs[0]]);
          copy = use_template(copy, "DATATYPE",
                              to_string(in_desc->output_datatype));
          copy = use_template(copy, "W", std::to_string(in_desc->width));
          copy = use_template(copy, "H", std::to_string(in_desc->height));

          code += copy;
          desc_to_name[in_desc] = matrix_name;
        }

        std::vector<std::string> pixel_mappings;
        std::vector<std::string> window_mappings;
        std::vector<std::string> mask_mappings;

        // Create mask definition if there are mask bindings for this operation
        if (s->mask_bindings.count(op) != 0) {
          auto mask_bindings = s->mask_bindings[op];

          for (auto& mask : mask_bindings) {
            std::string name = "mask_" + std::to_string(op->id);
            code += create_matrix_mask_def(mask, name);
            mask_mappings.push_back(name);
          }
          code += "\n";
        }

        // Setup all mappings
        if (op->current_state == ast4vx::WindowOperation::State::ToPixel ||
            op->current_state == ast4vx::WindowOperation::State::Reduce) {
          // Bind an actual output image is only relevant for ToPixel and Reduce
          for (auto& out_images : s->operation_output_images[i])
            pixel_mappings.emplace_back(generate_image_name(out_images));
          for (auto& windesc : op->window_inputs)
            window_mappings.emplace_back(desc_to_name[windesc]);
          windowdescriptor_mapping = &window_mappings;
        } else if (op->current_state == ast4vx::WindowOperation::State::At ||
                   op->current_state ==
                       ast4vx::WindowOperation::State::Forall) {
          pixel_mappings.emplace_back(desc_to_name[op->output]);
          for (auto& map : op->window_inputs) {
            pixel_mappings.emplace_back(desc_to_name[map]);
          }
        }

        std::vector<std::string> variable_mappings;
        for (auto& im : s->operation_variables[i])
          variable_mappings.emplace_back(generate_scalar_name(im));
        auto old_variable_mapping = variableaccessor_mapping;
        variableaccessor_mapping = &variable_mappings;

        auto old_mapping = pixelaccessor_mapping;
        pixelaccessor_mapping = &pixel_mappings;
        maskaccessor_mapping = &mask_mappings;

        code += visit(op) + "\n";

        maskaccessor_mapping = nullptr;
        pixelaccessor_mapping = old_mapping;
        windowdescriptor_mapping = nullptr;
        variableaccessor_mapping = old_variable_mapping;

        // If the output window has no domain set, copy it from the input window
        // descriptor
        if (op->output != nullptr &&
            desc_to_dom.find(op->output) == desc_to_dom.end()) {
          if (op->window_inputs.size() == 0)
            throw std::runtime_error(
                "CPPVisitor: if operation output window has no domain set "
                "explicitly and should inherit the domain from the input, an "
                "input window has to be given");
          desc_to_dom[op->output] = desc_to_dom[op->window_inputs[0]];
        }
      }

      desc_to_dom.clear();

      current_output_y = current_output_x = "";
      outer_loop = use_template(outer_loop, "CODE", code);
      return outer_loop;
    }
    case DomVX::OperatorType::GlobalOperator: {
      auto s = std::dynamic_pointer_cast<DomVX::GlobalOperator>(n);

      if (s->input_pixel_mappings.size() == 0)
        throw std::runtime_error(
            "CPPVisitor: GlobalOperation: At least one input image is "
            "expected");

      std::string pre_loop = "";

      std::string accum_var_name = "accum_" + n->id();

      // Setup the accumulator variable
      auto accum_var = std::make_shared<ast4vx::Variable>();
      if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<int>>(
              s->reduction->initial))
        accum_var->datatype = ast4vx::Datatype::INT32;
      else if (auto c = std::dynamic_pointer_cast<ast4vx::Constant<float>>(
                   s->reduction->initial))
        accum_var->datatype = ast4vx::Datatype::FLOAT;
      else if (auto c =
                   std::dynamic_pointer_cast<ast4vx::Constant<unsigned char>>(
                       s->reduction->initial))
        accum_var->datatype = ast4vx::Datatype::UINT8;
      else if (auto c =
                   std::dynamic_pointer_cast<ast4vx::Constant<unsigned int>>(
                       s->reduction->initial))
        accum_var->datatype = ast4vx::Datatype::UINT32;
      else
        throw std::runtime_error(
            "CPPVisitor: GlobalOperation: reduce: could not determine the "
            "constants type");
      accum_var->name = accum_var_name;

      pre_loop +=
          visit(std::make_shared<ast4vx::VariableDeclaration>(accum_var)) +
          ";\n";
      pre_loop += visit(assign(accum_var, s->reduction->initial)) + ";\n";

      accumulator_string = accum_var_name;

      std::vector<std::string> pixel_mappings;
      pixel_mappings.emplace_back(
          generate_image_name(s->input_pixel_mappings[0]));
      auto old_pixel_mapping = pixelaccessor_mapping;
      pixelaccessor_mapping = &pixel_mappings;

      std::string reduction_body = visit(s->reduction);

      pixelaccessor_mapping = old_pixel_mapping;

      std::string outer_loop = setup_outer_loop(s, s->input_pixel_mappings);

      std::vector<std::string> variable_mappings;
      variable_mappings.emplace_back(generate_scalar_name(s->reduction_out));
      auto old_variable_mapping = variableaccessor_mapping;
      variableaccessor_mapping = &variable_mappings;
      auto statements = std::make_shared<ast4vx::Statements>();
      std::string post_loop =
          visit(statements << assign(
                    std::make_shared<ast4vx::VariableAccessor>(0), accum_var));
      variableaccessor_mapping = old_variable_mapping;

      outer_loop = use_template(outer_loop, "CODE", reduction_body);
      return pre_loop + "\n" + outer_loop + "\n" + post_loop + "\n";
    }
    default:
      throw std::runtime_error(
          "CPPVisitor: visited no case for this AbstractionNode");
  }
  return "";
}
