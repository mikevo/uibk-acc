#include "mcc/tac/operand.h"
#include "mcc/tac/type.h"

namespace mcc {
namespace tac {

Operand::Operand(Scope::ptr_t scope) : Operand(Type::NONE, scope) {}

Operand::Operand(Type t, Scope::ptr_t scope) : t(t), scope(scope) {}

void Operand::updateResultType(Operator op) {
  if (op.getResultType() != Type::AUTO) {
    t = op.getResultType();
  }
}

Type Operand::getType() const { return t; }

void Operand::setType(Type type) { this->t = type; }

Scope::ptr_t const Operand::getScope() const { return this->scope; }
}
}
