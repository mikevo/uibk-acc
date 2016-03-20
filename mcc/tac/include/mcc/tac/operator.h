/* 
 * File:   operation.h
 * Author: Michael Voetter <Michael.Voetter@student.uibk.ac.at>
 *
 * Created on March 19, 2016, 3:29 PM
 */

#ifndef MCC_TAC_OPERATION_H
#define MCC_TAC_OPERATION_H

#include <string>

#include "mcc/tac/type.h"

namespace mcc {
  namespace tac {

    enum OperatorType {
      LINE, UNARY, BINARY
    };

    enum OperatorName {
      // INFO: add mapping in operatorMap (operator.cpp)
      NOP, ADD, SUB, MUL, ASSIGN, LABLE, JUMP, DIV, EQ, NE, LE, GE, LT, GT, MINUS
    };

    class Operator {
    public:
      Operator(OperatorName name);
      OperatorType getType() const;
      OperatorName getName() const;
      Type getResultType() const;
      std::string toString() const;

    private:
      const OperatorName name;
      const OperatorType type;
      const Type resultType;
    };
  }
}

#endif /* MCC_TAC_OPERATION_H */

