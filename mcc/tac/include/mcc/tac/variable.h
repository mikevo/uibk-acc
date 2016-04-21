/* 
 * File:   int_literal.h
 * Author: Michael Voetter <Michael.Voetter@student.uibk.ac.at>
 *
 * Created on March 19, 2016, 2:50 PM
 */

#ifndef MCC_TAC_VARIABLE_LITERAL_H
#define MCC_TAC_VARIABLE_LITERAL_H

#include "mcc/tac/operand.h"

#include <string>

namespace mcc {
    namespace tac {
        class Variable : public Operand {
        public:
            Variable(Type t);
            Variable(Type t, std::string name);
            unsigned getId() const;
            virtual std::string getName() const;
            virtual std::string getFullName() const;
            virtual std::string getValue() const override;
            virtual bool isLeaf() const override;
            std::pair<unsigned, unsigned> getScope() const;
            void setScope(std::pair<unsigned, unsigned> scope);
            void setIndex(unsigned);
            unsigned getIndex();
            bool isTemporary() const;
            
            std::string name;
            
        private:
            std::pair<unsigned, unsigned> scope;
            unsigned index;
            static unsigned nextId;
            unsigned id;
        };
    }
}


#endif /* MCC_TAC_VARIABLE_LITERAL_H */

