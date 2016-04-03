#include "mcc/tac/operand.h"
#include "mcc/tac/type.h"

namespace mcc {
  namespace tac {
    
    Operand::Operand() : Operand(Type::NONE) {
    }

    Operand::Operand(Type t) : t(t) {
    }

    void Operand::updateResultType(Operator op) {
      if (op.getResultType() != Type::AUTO) {
        t = op.getResultType();
      }
    }

    Type Operand::getType() const {
      return t;
    }

    void Operand::setType(Type type) {
      this->t = type;
    }
  }
}
