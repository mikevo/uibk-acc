/* 
 * File:   int_literal.h
 * Author: Michael Voetter <Michael.Voetter@student.uibk.ac.at>
 *
 * Created on March 19, 2016, 2:50 PM
 */

#ifndef MCC_TAC_FLOAT_LITERAL_H
#define MCC_TAC_FLOAT_LITERAL_H

#include "mcc/tac/operand.h"

namespace mcc {
    namespace tac {
        class FloatLiteral : public Operand {
        public:
            FloatLiteral(float value);
            virtual std::string getValue() const;
            float value;
        };
    }
}


#endif /* MCC_TAC_FLOAT_LITERAL_H */

