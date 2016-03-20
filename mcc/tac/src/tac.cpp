#include "mcc/tac/tac.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <typeinfo>

#include "ast.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/float_literal.h"
#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {
    namespace {

      Type convertType(ast::type& type) {
        if (typeid (type) == typeid (ast::int_type&)) {
          return Type::INT;
        }

        if (typeid (type) == typeid (ast::float_type&)) {
          return Type::FLOAT;
        }

        assert(false && "Unknown data type");
        return Type::NONE;
      }
  
      std::shared_ptr<Operand> convertNode(Tac *tac, ast::node &n) {
        if (typeid (n) == typeid (ast::int_literal&)) {
          int v = dynamic_cast<const ast::int_literal&> (n).value;

          return std::make_shared<IntLiteral>(v);
        }

        if (typeid (n) == typeid (ast::float_literal&)) {
          float v = dynamic_cast<const ast::float_literal&> (n).value;

          return std::make_shared<FloatLiteral>(v);
        }

        if (typeid (n) == typeid (ast::variable&)) {
          const ast::variable &v = dynamic_cast<const ast::variable&> (n);

          std::shared_ptr<Variable> var = std::make_shared<Variable>(
                  convertType(*v.var_type.get()), v.name);

          return var;
        }

        assert(false && "Unknown node type");
        return NULL;
      }
    }

    Tac::Tac() {
    }

    void Tac::convertAst(ast::node &n) {
      std::shared_ptr<Operand> operand = convertNode(this, n);
      this->addLine(operand);
    }

    void Tac::addLine(std::shared_ptr<Operand> operand) {
      std::shared_ptr<Triple> line;
      
      if (typeid(operand.get()) == typeid(Triple)) {
        line = std::dynamic_pointer_cast<Triple>(operand);
      } else {
        line = std::make_shared<Triple>(operand);
      }
      
      codeLines.push_back(line);
    }

    std::string Tac::toString() const {
      std::string output;

      std::for_each(codeLines.begin(), codeLines.end(),
              [&](auto const &line) {
                output.append(line.get()->toString());
              }
      );

      return output;
    }
  }
}