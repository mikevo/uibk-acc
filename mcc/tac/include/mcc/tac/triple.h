/* 
 * File:   triple.h
 *
 * Created on March 18, 2016, 11:02 PM
 */

#ifndef MCC_TACTRIPLE_H
#define MCC_TACTRIPLE_H

#include <string>
#include <memory>

#include "mcc/tac/operator.h"
#include "mcc/tac/operand.h"

namespace mcc {
    namespace tac {

        class Triple : public Operand {
        public:
            Triple(std::shared_ptr<Operand> arg);
            Triple(Operator op, std::shared_ptr<Operand> arg);
            Triple(Operator op, std::shared_ptr<Operand> arg1,
                    std::shared_ptr<Operand> arg2);
            std::shared_ptr<Operand> arg1, arg2;
            Operator op;
            unsigned basicBlockId;
            std::string getName() const;
            void setName(const std::string name);
            unsigned getId() const;
            virtual bool isLeaf() const;
            virtual std::string getValue() const;
            virtual std::string toString() const;

        private:
            static unsigned nextId;
            unsigned id;
            std::string name;
        };
    }
}


#endif /* MCC_TACTRIPLE_H */

