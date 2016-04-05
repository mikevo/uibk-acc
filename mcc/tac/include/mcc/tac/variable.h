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
            Variable(Type t, std::string name);
            virtual std::string getName() const;
            virtual std::string getValue() const override;
            virtual bool isLeaf() const override;
            int getScope() const;
            void setScope(int scope);
            
            std::string name;
            
        private:
            int scope;
            
        };
    }
}


#endif /* MCC_TAC_VARIABLE_LITERAL_H */

