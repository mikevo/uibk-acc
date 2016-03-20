#include "mcc/tac/triple.h"

#include <cassert>

namespace mcc {
    namespace tac {
        unsigned Triple::nextId = 0;

        Triple::Triple(Operator &op, std::shared_ptr<Operand> arg) :
        Triple(op, arg, NULL) {
            // TODO: check if it is an unary operator;
        }

        Triple::Triple(Operator &op, std::shared_ptr<Operand> arg1,
                std::shared_ptr<Operand> arg2) :
        Operand(arg1->getType()),
        arg1(arg1),
        arg2(arg2),
        op(op),
        basicBlockId(0),
        id(++nextId) {
            if (arg1 == NULL) {
                assert(false && "Missing argument");
            }

            if (arg2 != NULL) {
                assert((arg1->getType() == arg2->getType()) && "Type miss match");
            }
        }

        unsigned Triple::getId() const {
            return id;
        }

        std::string Triple::getValue() const {
            return "t" + std::to_string(getId());
        }
    }
}
