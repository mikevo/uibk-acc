/* 
 * File:   triple.h
 *
 * Created on March 18, 2016, 11:02 PM
 */

#ifndef MCC_TACTRIPLE_H
#define MCC_TACTRIPLE_H

#include <string>

#include "mcc/tac/tac.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/operand.h"

namespace mcc {
    namespace tac {

        class Triple : public Operand {
        public:
            Triple(Operator op, Operand *arg);
            Triple(Operator op, Operand *arg1, Operand *arg2);
            Operand *arg1, *arg2;
            Operator op;
            unsigned basicBlockId;
            unsigned getId() const;
            virtual std::string getValue() const;
            
        private:
            static unsigned nextId;
            const unsigned id;
        };
    }
}


#endif /* MCC_TACTRIPLE_H */

