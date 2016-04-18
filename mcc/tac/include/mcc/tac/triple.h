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
#include "mcc/tac/variable.h"

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
            unsigned getBasicBlockId() const;
            void setBasicBlockId(unsigned blockId);
            virtual bool isLeaf() const override;
            virtual std::string getValue() const override;
            virtual std::string toString() const;
            void setTargetVariable(std::shared_ptr<Variable> var);
            std::shared_ptr<Variable> getTargetVariable();
            bool isTargetVarValid(void) const;

            bool containsArg1() const;
            bool containsArg2() const;

        private:
            std::shared_ptr<Variable> targetVar;
            static unsigned nextId;
            unsigned id;
            std::string name;
            bool targetVarIsValid;
        };
    }
}

#endif /* MCC_TAC_TRIPLE_H */

