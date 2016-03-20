/* 
 * File:   operand.h
 *
 * Created on March 19, 2016, 1:44 PM
 */

#ifndef MCC_TAC_OPERAND_H
#define MCC_TAC_OPERAND_H

#include <string>

namespace mcc {
    namespace tac {

        enum Type {
            NONE, INT, FLOAT, BOOL
        };

        class Operand {
        public:
            Operand(Type t);
            virtual Type getType() const;
            virtual void setType(Type t);
            virtual std::string getValue() const = 0;
        private:
            Type t;
        };
    }
}

#endif /* MCC_TAC_OPERAND_H */

