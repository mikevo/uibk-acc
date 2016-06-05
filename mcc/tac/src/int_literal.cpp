#include "mcc/tac/int_literal.h"

namespace mcc {
namespace tac {

IntLiteral::IntLiteral(int value, Scope::ptr_t scope)
    : Operand(Type::INT, scope), value(value) {}

bool IntLiteral::isLeaf() const { return true; }

std::string IntLiteral::getValue() const { return std::to_string(value); }
}
}