#include "cpp_ast.hpp"

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
      std::string channel;
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

    case ast4vx::NodeType::PixelToPixel: {
      auto s = std::dynamic_pointer_cast<ast4vx::PixelToPixel>(n);
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
            if (s->p2p_ops[y][x].get() == nullptr) continue;
            if (s->p2p_ops[y][x]->out_pixel_mappings.size() != 1)
              throw std::runtime_error(
                  "CPPVisitor: WindowOperation: At operations requires exactly "
                  "one output mapping");
          }
          auto old_x = current_output_x;
          auto old_y = current_output_y;
          current_output_x = std::to_string(x);
          current_output_y = std::to_string(y);

          if (s->current_state == ast4vx::WindowOperation::State::At)
            code += visit(s->p2p_ops[y][x]);
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
        code += visit(assign(std::make_shared<ast4vx::PixelAccessor>(0),
                             accum_var)) +
                ';';
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

      if (maskaccessor_mapping->size() != s->masks.size())
        throw std::runtime_error(
            "MaskPixelToPixel AST did not get the same amount of Mask "
            "Accessors it expects at code generation time");

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
