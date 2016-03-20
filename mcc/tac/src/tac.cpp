#include "mcc/tac/tac.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <typeinfo>

#include "ast.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/float_literal.h"

namespace mcc {
  namespace tac {
    namespace {

      std::shared_ptr<Triple> convertNode(Tac *tac, ast::node &n) {
        if (typeid (n) == typeid (ast::int_literal&)) {
          Operator op = Operator(OperatorName::ASSIGN);

          int v = dynamic_cast<const ast::int_literal&> (n).value;

          std::shared_ptr<IntLiteral> value =
                  std::make_shared<IntLiteral>(v);
          std::shared_ptr<Triple> line = std::make_shared<Triple>(op, value);

          tac->addLine(line);

          return line;
        }
        
        if (typeid (n) == typeid (ast::float_literal&)) {
          Operator op = Operator(OperatorName::ASSIGN);

          float v = dynamic_cast<const ast::float_literal&> (n).value;

          std::shared_ptr<FloatLiteral> value =
                  std::make_shared<FloatLiteral>(v);
          std::shared_ptr<Triple> line = std::make_shared<Triple>(op, value);

          tac->addLine(line);

          return line;
        }

        assert(false && "Unknown type");
        return NULL;
      }
    }

    Tac::Tac() {
    }

    void Tac::convertAst(ast::node &n) {
      convertNode(this, n);
    }

    void Tac::addLine(std::shared_ptr<Triple> line) {
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