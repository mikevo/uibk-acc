#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {

    Variable::Variable(Type t, std::string name) : Operand(t), name(name) {
    }
    
    bool Variable::isLeaf() const {
      return true;
    }

    std::string Variable::getName() const {
      return name;
    }

    std::string Variable::getValue() const {
      return this->getName();
    }
  }
}