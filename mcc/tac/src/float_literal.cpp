#include "mcc/tac/float_literal.h"

namespace mcc {
namespace tac {

FloatLiteral::FloatLiteral(float value, Scope::ptr_t scope)
    : Operand(Type::FLOAT, scope), value(value) {}

bool FloatLiteral::isLeaf() const { return true; }

std::string FloatLiteral::getValue() const { return std::to_string(value); }
}
}