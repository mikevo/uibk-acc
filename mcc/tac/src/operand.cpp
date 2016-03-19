#include "mcc/tac/operand.h"

namespace mcc {
    namespace tac {

        Operand::Operand(Type t) : t(t) {}

        Type Operand::getType() const {
            return t;
        }

        void Operand::setType(Type type) {
            this->t = type;
        }
    }
}
