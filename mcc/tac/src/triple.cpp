#include "mcc/tac/triple.h"

#include <cassert>
#include <memory>
#include <iostream>
#include <typeinfo>

#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {
    unsigned Triple::nextId = 0;

    Triple::Triple(std::shared_ptr<Operand> arg) :
        Triple(Operator(OperatorName::NOP), arg) {
      // check if it is a terminal
      assert(arg.get()->isLeaf() && "Operand is non-terminal!");
    }

    // constructor needed for Label
    Triple::Triple(OperatorName op) :
        Triple(Operator(op), nullptr) {
      assert(this->op.getType() == OperatorType::NONE && "Operator not unary!");
    }

    Triple::Triple(Operator op, std::shared_ptr<Operand> arg) :
        Triple(op, arg, nullptr) {
      // check if operator is UNARY or NONE (NOP)
      assert(
          (op.getType() == OperatorType::NONE
              || op.getType() == OperatorType::UNARY) && "Operator not unary!");
    }

    Triple::Triple(Operator op, std::shared_ptr<Operand> arg1,
        std::shared_ptr<Operand> arg2) :
        Operand(), op(op), basicBlockId(0), arg1(arg1), arg2(arg2), targetVar(
            nullptr), id(0) {
      if (this->containsArg1()) {
        this->setType(arg1->getType());
      }

      this->updateResultType(op);

      switch (op.getType()) {
        case OperatorType::BINARY:
          assert(this->containsArg2() && "second argument needed");
        case OperatorType::UNARY:
          assert(this->containsArg1() && "first argument needed");
          break;
        case OperatorType::NONE:
          assert(
              (!this->containsArg1() && !this->containsArg2())
                  && "No argument allowed");
          break;
      }

      if (this->containsArg2()) {
        assert(
            (op.getName() == OperatorName::JUMPFALSE
                || arg1->getType() == arg2->getType()) && "Type mismatch");
      }

      switch (op.getName()) {
        case OperatorName::NOP:
        case OperatorName::ASSIGN:
        case OperatorName::JUMPFALSE:
        case OperatorName::JUMP:
          break;
        default:
          id = ++nextId;
          this->name = "$t" + std::to_string(id);

          setTargetVariable(
              std::make_shared<Variable>(this->getType(), this->name));
      }
    }

    bool Triple::isLeaf() const {
      return false;
    }

    unsigned Triple::getId() const {
      return id;
    }

    std::string Triple::getName() const {
      return name;
    }

    void Triple::setName(const std::string name) {
      this->name = name;
    }

    std::string Triple::getValue() const {
      return getName();
    }

    unsigned Triple::getBasicBlockId() const {
      return basicBlockId;
    }

    void Triple::setBasicBlockId(unsigned blockId) {
      basicBlockId = blockId;
    }

    std::string Triple::toString() const {
      std::string output;

      switch (op.getName()) {
        case OperatorName::JUMPFALSE:
        case OperatorName::JUMP:
          output.append(op.toString());
          output.append(arg1.get()->getValue());
          if (arg2 != nullptr) {
            output.append(" ");
            output.append(arg2.get()->getValue());
          }

          return output;

        case OperatorName::LABEL:
          output.append(op.toString());
          output.append(getName());

          return output;

        case OperatorName::NOP:
          output.append(arg1.get()->getValue());

          return output;

        default:
          if (op.getName() != OperatorName::ASSIGN) {
            output.append(name);
            output.append(" = ");
          }

          switch (op.getType()) {
            case OperatorType::UNARY:

              output.append(op.toString());
              output.append(arg1.get()->getValue());

              break;
            default:
              output.append(arg1.get()->getValue());
              output.append(op.toString());
              if (arg2 != nullptr) {
                output.append(arg2.get()->getValue());
              }
          }
          return output;
      }
    }

    void Triple::setTargetVariable(std::shared_ptr<Variable> var) {
      if (var != nullptr) {
        targetVar = var;
      } else {
        assert(false && "don't kill the object");
      }
    }

    std::shared_ptr<Variable> Triple::getTargetVariable() {
      return targetVar;
    }

    bool Triple::containsArg1() const {
      return (arg1 != nullptr);
    }

    bool Triple::containsArg2() const {
      return (arg2 != nullptr);
    }

    bool Triple::containsTargetVar(void) const {
      return (targetVar != nullptr);
    }

    std::shared_ptr<Operand> Triple::getArg1() const {
      return this->arg1;
    }
    std::shared_ptr<Operand> Triple::getArg2() const {
      return this->arg2;
    }
  }
}
