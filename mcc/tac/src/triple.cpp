#include "mcc/tac/triple.h"

#include <cassert>
#include <iostream>
#include <typeinfo>

#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {
    unsigned Triple::nextId = 1;

    Triple::Triple(std::shared_ptr<Operand> arg) :
    Triple(Operator(OperatorName::NOP), arg) {
      // TODO: check if it is a terminal
    }

    Triple::Triple(OperatorName op) : Triple(Operator(op), NULL) {

    }

    Triple::Triple(Operator op, std::shared_ptr<Operand> arg) :
    Triple(op, arg, NULL) {
      // TODO: check if it is an unary operator;
    }

    Triple::Triple(Operator op, std::shared_ptr<Operand> arg1,
            std::shared_ptr<Operand> arg2) :
    arg1(arg1),
    arg2(arg2),
    op(op),
    basicBlockId(0),
    id(0) {
      if (arg1 != NULL) {
        this->setType(arg1->getType());
      }

      this->updateResultType(op);

      if (arg2 != NULL) {
        assert((arg1->getType() == arg2->getType()) && "Type miss match");
      }

      if (arg1 != NULL) {
        if (typeid (*arg1.get()) == typeid (Variable)) {
          this->name = arg1.get()->getValue();
        } else {
          id = ++nextId;
          this->name = "$t" + std::to_string(id);
        }
      } else {
        id = ++nextId;
        this->name = "$t" + std::to_string(id);
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

    std::string Triple::toString() const {
      std::string output;

      switch (op.getName()) {
        case OperatorName::JUMPFALSE:
          output.append(op.toString());
          output.append(arg1.get()->getValue());
          output.append(" ");
          output.append(arg2.get()->getValue());

          return output;

        case OperatorName::LABEL:
          output.append(op.toString());
          output.append(getName());

          return output;
        default:;
      }

      if ((name != arg1.get()->getValue())
              || ((op.getName() != OperatorName::ASSIGN) && (arg2 != NULL))) {
        output.append(name);
        auto assignOp = Operator(OperatorName::ASSIGN);
        output.append(assignOp.toString());
      }

      if (op.getType() != OperatorType::BINARY) {
        output.append(op.toString());
        output.append(arg1.get()->getValue());
      } else if (arg2 == NULL) {
        output.append(arg1.get()->getValue());
      }

      if (arg2 != NULL) {
        output.append(arg1.get()->getValue());
        output.append(op.toString());
        output.append(arg2.get()->getValue());
      }

      return output;
    }
  }
}
