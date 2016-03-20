#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {

    Variable::Variable(Type t, std::string name) : Operand(t), name(name) {
    }

    std::string Variable::getName() const {
      return name;
    }

    std::string Variable::getValue() const {
      return this->getName();
    }
  }
}