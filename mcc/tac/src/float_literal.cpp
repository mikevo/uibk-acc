#include "mcc/tac/float_literal.h"

namespace mcc {
    namespace tac {
        FloatLiteral::FloatLiteral(float value) : Operand(Type::FLOAT) {
            this->value = value;
        }
        
        std::string FloatLiteral::getValue() const {
            return std::to_string(value);
        }
    }
}