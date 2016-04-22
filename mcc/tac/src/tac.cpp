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
          { ast::unary_operand::MINUS, OperatorName::MINUS }, {
              ast::unary_operand::NOT, OperatorName::NOT } };

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

          tac->getScope().setCheckPoint();

          do {
            auto key = std::make_pair(v->name,
                tac->getScope().getCurrentScope());
            auto mapVar = tac->getVarTable().find(key);

            if (mapVar != tac->getVarTable().end()) {
              tac->getScope().goToCheckPoint();
              return mapVar->second.back();
            }
          } while (tac->getScope().goToParent());

          // TODO: do we need this output? If not, remove it.
          std::cout << v->name << ":"
              << tac->getScope().getCurrentScope()->getDepth() << ":"
              << tac->getScope().getCurrentScope()->getIndex() << std::endl;
          assert(false && "Usage of undeclared variable");
        }

        if (typeid (*n.get()) == typeid(ast::binary_operation)) {
          auto v = std::static_pointer_cast<ast::binary_operation>(n);

          auto lhs = convertNode(tac, v->lhs);
          auto rhs = convertNode(tac, v->rhs);

          bool setTarVar = false;

          VarTableValue variable;

          if (*v.get()->op.get() == ast::binary_operand::ASSIGN) {
            // TODO: are there any other cases? don't get it why here is an if
            // if this is only an additional check, change it with an assert!
            if (typeid(*lhs.get()) == typeid(Variable)) {
              auto var = std::static_pointer_cast<Variable>(lhs);
              auto key = std::make_pair(var->getName(), var->getScope());
              variable = tac->addVarRenaming(key);

              setTarVar = true;

              lhs = variable;
            }
          }

          auto triple = std::make_shared<Triple>(
              Operator(binaryOperatorMap.at(*v.get()->op.get())), lhs, rhs);

          if (setTarVar) {
            triple->setTargetVariable(variable);
          }

          if (triple->containsTargetVar()) {
            tac->addToVarTable(tac, triple->getTargetVariable());
          }

          tac->addLine(triple);

          return triple;
        }

        if (typeid (*n.get()) == typeid(ast::unary_operation)) {
          auto v = std::static_pointer_cast<ast::unary_operation>(n);
          auto lhs = convertNode(tac, v.get()->sub);

          auto var = std::make_shared<Triple>(
              Operator(unaryOperatorMap.at(*v.get()->op.get())), lhs);

          if (var->containsTargetVar()) {
            tac->addToVarTable(tac, var->getTargetVariable());
          }

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

          tac->getScope().addNewChild();

          std::for_each(statements.begin(), statements.end(),
              [&](std::shared_ptr<ast::node> const &s) {
                convertNode(tac, s);
              });

          tac->getScope().goToParent();

          return nullptr;
        }

        if (typeid (*n.get()) == typeid(ast::decl_stmt)) {
          auto v = std::static_pointer_cast<ast::decl_stmt>(n);
          auto tempVar = v->var;

          auto variable = std::make_shared<Variable>(
              convertType(*tempVar->var_type.get()), tempVar->name);
          variable->setScope(tac->getScope().getCurrentScope());

          if (v->init_expr != nullptr) {
            auto initExpression = convertNode(tac, v->init_expr);
            tac->addToVarTable(tac, variable);

            if (initExpression->isLeaf()) {
              auto var = std::make_shared<Triple>(
                  Operator(OperatorName::ASSIGN), variable, initExpression);
              var->setTargetVariable(variable);

              tac->addLine(var);
              return var;
            } else {
              // if initExpression is a triple
              if (typeid (*initExpression.get()) == typeid(Triple)) {
                auto triple = std::static_pointer_cast<Triple>(initExpression);
                assert(
                    triple->containsTargetVar() && "should contain a variable");

                tac->removeFromVarTable(tac, triple->getTargetVariable());
                triple->setTargetVariable(variable);

                return triple;
              } else {
                assert(false && "Unknown subtype");
              }
            }
          } else {
            tac->addToVarTable(tac, variable);
          }

          return variable;
        }

        if (typeid (*n.get()) == typeid(ast::if_stmt)) {
          auto stmt = std::static_pointer_cast<ast::if_stmt>(n);

          auto condition = convertNode(tac, stmt.get()->condition);

          auto falseLabel = std::make_shared<Label>();
          auto var = std::make_shared<Triple>(Operator(OperatorName::JUMPFALSE),
              condition, falseLabel);

          tac->addLine(var);
          tac->nextBasicBlock();

          convertNode(tac, stmt.get()->then_stmt);

          std::shared_ptr<Label> trueLabel;

          if (stmt->else_stmt != nullptr) {
            trueLabel = std::make_shared<Label>();
            auto trueJump = std::make_shared<Triple>(
                Operator(OperatorName::JUMP), trueLabel);
            tac->addLine(trueJump);
          }

          tac->nextBasicBlock();
          tac->addLine(falseLabel);

          if (stmt->else_stmt != nullptr) {
            convertNode(tac, stmt.get()->else_stmt);
            tac->nextBasicBlock();
            tac->addLine(trueLabel);
          }

          return nullptr;
        }

        std::cout << typeid (*n.get()).name() << std::endl;
        assert(false && "Unknown node type");
        return nullptr;
      }
    }

    Tac::Tac(std::shared_ptr<ast::node> n) :
        currentBasicBlock(0) {
      this->convertAst(n);
    }

    void Tac::convertAst(std::shared_ptr<ast::node> n) {
      convertNode(this, n);
    }

    void Tac::addLine(std::shared_ptr<Triple> line) {
      line->setBasicBlockId(currentBasicBlock);
      this->codeLines.push_back(line);
    }

    void Tac::addLine(std::shared_ptr<Label> line) {
      line->setBasicBlockId(currentBasicBlock);
      this->codeLines.push_back(line);
    }

    Scope& Tac::getScope() {
      return this->variableStore;
    }

    void Tac::nextBasicBlock() {
      ++currentBasicBlock;
    }

    void Tac::createBasicBlockIndex() {
      basicBlockIndex.clear();

      auto currentBasicBlock = std::make_shared<BasicBlock>(0);
      auto currentBasicBlockId = codeLines.front()->getBasicBlockId();

      for (auto& triple : codeLines) {
        if (triple->getBasicBlockId() != currentBasicBlockId) {
          basicBlockIndex.push_back(currentBasicBlock);
          currentBasicBlock = std::make_shared<BasicBlock>(
              triple->getBasicBlockId());
          currentBasicBlockId = triple->getBasicBlockId();
        }

        currentBasicBlock.get()->push_back(triple);
      }

      // Add last block
      basicBlockIndex.push_back(currentBasicBlock);
    }

    std::string Tac::toString() const {
      std::string output;

      unsigned count = 0;

      for (auto &line : codeLines) {
        output.append(line.get()->toString());

        if (codeLines.size() > 1 && count < codeLines.size() - 1) {
          output.append("\n");
        }

        ++count;
      }

      return output;
    }

    unsigned Tac::basicBlockCount() {
      return basicBlockIndex.size();
    }

    const std::vector<std::shared_ptr<BasicBlock>>&Tac::getBasicBlockIndex() {
      if (basicBlockIndex.empty()) {
        createBasicBlockIndex();
      }

      return basicBlockIndex;
    }

    const std::map<VarTableKey, std::vector<VarTableValue>>&Tac::getVarTable() {
      return varTable;
    }

    void Tac::addToVarTable(Tac *tac, VarTableValue value) {
      VarTableKey key = std::make_pair(value->getName(), value->getScope());

      std::vector<VarTableValue> valueVec;
      valueVec.push_back(value);

      varTable.insert(std::make_pair(key, valueVec));
    }

    void Tac::removeFromVarTable(Tac* tac, VarTableValue value) {
      // TODO: possibly wrong
      VarTableKey key = std::make_pair(value->getName(), value->getScope());

      varTable.erase(key);
    }

    VarTableValue Tac::addVarRenaming(VarTableKey key) {

      auto valuePair = varTable.find(key);

      if (valuePair != varTable.end()) {
        auto& varVector = valuePair->second;
        auto cloneVar = std::make_shared<Variable>(*varVector.front());
        cloneVar->setScope(varVector.front()->getScope());

        cloneVar->setIndex(varVector.size());
        varVector.push_back(cloneVar);

        return cloneVar;
      }

      assert(false && "Variable to rename does not exist!");

    }
  }
}
