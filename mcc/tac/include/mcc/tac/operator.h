/* 
 * File:   operation.h
 * Author: Michael Voetter <Michael.Voetter@student.uibk.ac.at>
 *
 * Created on March 19, 2016, 3:29 PM
 */

#ifndef MCC_TAC_OPERATION_H
#define MCC_TAC_OPERATION_H

#include <string>

namespace mcc {
    namespace tac {

        enum OperatorType {
            UNARY, BINARY
        };

        enum OperatorName {
            // INFO: add mapping in operatorMap (operator.cpp)
            ADD, SUB, MUL, ASSIGN, LABLE, JUMP
        };

        class Operator {
        public:
            Operator(OperatorName name);
            OperatorType getType() const;
            OperatorName getName() const;
            std::string toString() const;

        private:
            const OperatorName name;
            const OperatorType type;
        };
    }
}

#endif /* MCC_TAC_OPERATION_H */

