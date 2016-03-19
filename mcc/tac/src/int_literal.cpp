#include "mcc/tac/int_literal.h"

namespace mcc {
    namespace tac {
        IntLiteral::IntLiteral(int value) : Operand(Type::INT) {
            this->value = value;
        }
        
        std::string IntLiteral::getValue() const {
            return std::to_string(value);
        }
    }
}