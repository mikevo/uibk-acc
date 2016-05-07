/*
 * File:   operation.h
 * Author: Michael Voetter <Michael.Voetter@student.uibk.ac.at>
 *
 * Created on March 19, 2016, 3:29 PM
 */

#ifndef MCC_TAC_OPERATION_H
#define MCC_TAC_OPERATION_H

#include <memory>
#include <string>

#include "mcc/tac/type.h"

namespace mcc {
namespace tac {

enum class OperatorType { NONE, UNARY, BINARY };

enum class OperatorName {
  // INFO: add mapping in operatorMap (operator.cpp)
  NOP,
  ADD,
  SUB,
  MUL,
  ASSIGN,
  LABEL,
  JUMP,
  JUMPFALSE,
  DIV,
  EQ,
  NE,
  LE,
  GE,
  LT,
  GT,
  MINUS,
  NOT,
  PUSH,
  POP,
  CALL,
  RET,
};

class Operator {
 public:
  typedef std::shared_ptr<Operator> ptr_t;

  Operator(OperatorName name);
  OperatorType getType() const;
  OperatorName getName() const;
  Type getResultType() const;
  std::string toString() const;

 private:
  OperatorName name;
  OperatorType type;
  Type resultType;
};
}
}

#endif /* MCC_TAC_OPERATION_H */
