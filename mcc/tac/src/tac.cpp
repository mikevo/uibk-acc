#include "mcc/tac/tac.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <ostream>
#include <typeinfo>

#include "ast.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/float_literal.h"
#include "mcc/tac/variable.h"
#include "mcc/tac/label.h"

namespace mcc {
  namespace tac {
    namespace {

      static const std::map<ast::binary_operand, OperatorName> binaryOperatorMap {
          { ast::binary_operand::ADD, OperatorName::ADD }, {
              ast::binary_operand::SUB, OperatorName::SUB }, {
              ast::binary_operand::MUL, OperatorName::MUL }, {
              ast::binary_operand::DIV, OperatorName::DIV }, {
              ast::binary_operand::EQ, OperatorName::EQ }, {
              ast::binary_operand::NE, OperatorName::NE }, {
              ast::binary_operand::LE, OperatorName::LE }, {
              ast::binary_operand::GE, OperatorName::GE }, {
              ast::binary_operand::LT, OperatorName::LT }, {
              ast::binary_operand::GT, OperatorName::GT }, {
              ast::binary_operand::ASSIGN, OperatorName::ASSIGN } };

      static const std::map<ast::unary_operand, OperatorName> unaryOperatorMap {
          { ast::unary_operand::MINUS, OperatorName::MINUS } };

      Type convertType(ast::type& type) {
        if (typeid (type) == typeid(ast::int_type&)) {
          return Type::INT;
        }

        if (typeid (type) == typeid(ast::float_type&)) {
          return Type::FLOAT;
        }

        assert(false && "Unknown data type");
        return Type::NONE;
      }

      std::shared_ptr<Operand> convertNode(Tac *tac,
          std::shared_ptr<ast::node> n) {
        if (typeid (*n.get()) == typeid(ast::int_literal)) {
          int v = std::static_pointer_cast<ast::int_literal>(n).get()->value;

          return std::make_shared<IntLiteral>(v);
        }

        if (typeid (*n.get()) == typeid(ast::float_literal)) {
          float v = std::static_pointer_cast<ast::float_literal>(n).get()->value;

          return std::make_shared<FloatLiteral>(v);
        }

        if (typeid (*n.get()) == typeid(ast::variable)) {

          auto v = std::static_pointer_cast<ast::variable>(n);

          auto var = std::make_shared<Variable>(
              convertType(*v.get()->var_type.get()), v.get()->name);

          return var;
        }

        if (typeid (*n.get()) == typeid(ast::binary_operation)) {
          auto v = std::static_pointer_cast<ast::binary_operation>(n);

          auto lhs = convertNode(tac, v.get()->lhs);
          auto rhs = convertNode(tac, v.get()->rhs);

          auto var = std::make_shared<Triple>(
              Operator(binaryOperatorMap.at(*v.get()->op.get())), lhs, rhs);

          tac->addLine(var);

          return var;
        }

        if (typeid (*n.get()) == typeid(ast::unary_operation)) {
          auto v = std::static_pointer_cast<ast::unary_operation>(n);
          auto lhs = convertNode(tac, v.get()->sub);

          auto var = std::make_shared<Triple>(
              Operator(unaryOperatorMap.at(*v.get()->op.get())), lhs);

          tac->addLine(var);

          return var;
        }

        if (typeid (*n.get()) == typeid(ast::expr_stmt)) {
          auto v = std::static_pointer_cast<ast::expr_stmt>(n);

          return convertNode(tac, v.get()->sub);
        }

        if (typeid (*n.get()) == typeid(ast::paren_expr)) {
          auto v = std::static_pointer_cast<ast::paren_expr>(n);

          return convertNode(tac, v.get()->sub);
        }

        if (typeid (*n.get()) == typeid(ast::compound_stmt)) {
          auto v = std::static_pointer_cast<ast::compound_stmt>(n);
          auto statements = v.get()->statements;

          std::for_each(statements.begin(), statements.end(),
              [&](std::shared_ptr<ast::node> const &s) {
                convertNode(tac, s);
              });

          return NULL;
        }

        if (typeid (*n.get()) == typeid(ast::decl_stmt)) {
          auto v = std::static_pointer_cast<ast::decl_stmt>(n);

          auto lhs = convertNode(tac, v.get()->var);
          auto initStmt = v.get()->init_expr;
          if (initStmt != NULL) {
            auto rhs = convertNode(tac, v.get()->init_expr);

            if (rhs.get()->isLeaf()) {
              auto var = std::make_shared<Triple>(
                  Operator(OperatorName::ASSIGN), lhs, rhs);

              tac->addLine(var);
              return var;
            } else {
              if (typeid (*rhs.get()) == typeid(Triple)) {
                auto var = std::static_pointer_cast<Triple>(rhs);
                var.get()->setName(lhs.get()->getValue());
                return var;
              } else {
                assert(false && "Unknown subtype");
                return lhs;
              }
            }

          }

          tac->addLine(lhs);

          return lhs;
        }

        if (typeid (*n.get()) == typeid(ast::if_stmt)) {
          auto stmt = std::static_pointer_cast<ast::if_stmt>(n);

          auto condition = convertNode(tac, stmt.get()->condition);

          auto label = std::make_shared<Label>();
          auto var = std::make_shared<Triple>(Operator(OperatorName::JUMPFALSE),
              condition, label);

          tac->addLine(var);

          convertNode(tac, stmt.get()->then_stmt);

          tac->addLine(label);

          if (stmt.get()->else_stmt != NULL) {
            convertNode(tac, stmt.get()->else_stmt);
          }

          return NULL;
        }

        std::cout << typeid (*n.get()).name() << std::endl;
        assert(false && "Unknown node type");
        return NULL;
      }
    }

    Tac::Tac() {
    }

    void Tac::convertAst(std::shared_ptr<ast::node> n) {
      convertNode(this, n);
    }

    void Tac::addLine(std::shared_ptr<Operand> operand) {
      addLine(std::make_shared<Triple>(operand));
    }

    void Tac::addLine(std::shared_ptr<Triple> line) {
      this->codeLines.push_back(line);
    }

    void Tac::addLine(std::shared_ptr<Label> line) {
      this->codeLines.push_back(line);
    }

    std::string Tac::toString() const {
      std::string output;

      unsigned count = 0;
      std::for_each(std::begin(codeLines), std::end(codeLines),
          [&](std::shared_ptr<Triple> const &line) {
            output.append(line.get()->toString());

            if (codeLines.size() > 1 && count < codeLines.size() - 1) {
              output.append("\n");
            }

            ++count;
            
          });

      return output;
    }
  }
}
