/* 
 * File:   triple.h
 *
 * Created on March 18, 2016, 11:02 PM
 */

#ifndef MCC_TAC_TRIPLE_H
#define MCC_TAC_TRIPLE_H

#include <string>
#include <memory>

#include "mcc/tac/operator.h"
#include "mcc/tac/operand.h"
#include "mcc/tac/operator.h"

namespace mcc {
    namespace tac {

        class Triple : public Operand {
        public:
            Triple(std::shared_ptr<Operand> arg);
            Triple(OperatorName op);
            Triple(Operator op, std::shared_ptr<Operand> arg);
            Triple(Operator op, std::shared_ptr<Operand> arg1,
                    std::shared_ptr<Operand> arg2);
            std::shared_ptr<Operand> arg1, arg2;
            Operator op;
            unsigned basicBlockId;
            std::string getName() const;
            void setName(const std::string name);
            unsigned getId() const;
            virtual bool isLeaf() const override;
            virtual std::string getValue() const override;
            virtual std::string toString() const;

        private:
            static unsigned nextId;
            unsigned id;
            std::string name;
        };
    }
}

#endif /* MCC_TAC_TRIPLE_H */

