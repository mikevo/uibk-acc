/* 
 * File:   triple.h
 *
 * Created on March 18, 2016, 11:02 PM
 */

#ifndef MCC_TACTRIPLE_H
#define MCC_TACTRIPLE_H

#include <string>
#include <memory>

#include "mcc/tac/tac.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/operand.h"

namespace mcc {
    namespace tac {

        class Triple : public Operand {
        public:
            Triple(Operator &op, std::shared_ptr<Operand> arg);
            Triple(Operator &op, std::shared_ptr<Operand> arg1,
                    std::shared_ptr<Operand> arg2);
            std::shared_ptr<Operand> arg1, arg2;
            Operator &op;
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

