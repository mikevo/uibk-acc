#include "mcc/tac/operand.h"
#include "mcc/tac/type.h"

namespace mcc {
namespace tac {

Operand::Operand() : Operand(Type::NONE) {}

Operand::Operand(Type t) : t(t) {}

void Operand::updateResultType(Operator op) {
  if (op.getResultType() != Type::AUTO) {
    t = op.getResultType();
  }
}

Type Operand::getType() const { return t; }

void Operand::setType(Type type) { this->t = type; }

unsigned Operand::getSize() const {
  switch (t) {
    case Type::BOOL:
      return 0;  // Gets not stored in assembly
      break;
    case Type::INT:
      return 4;
      break;
    case Type::FLOAT:
      return 4;
      break;
    default:
      return 0;
  }
}
}
}
