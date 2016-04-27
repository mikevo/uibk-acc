/*
 * sub_expression.cpp
 *
 *  Created on: Apr 25, 2016
 */

#include "mcc/cfg/sub_expression.h"

#include <cassert>

#include "mcc/tac/helper/ast_converters.h"

namespace mcc {
namespace cfg {

SubExpression::SubExpression(Operator op, OperandPtr arg)
    : SubExpression(op, arg, nullptr) {}

SubExpression::SubExpression(Operator op, OperandPtr arg1, OperandPtr arg2)
    : op(op), arg1(arg1), arg2(arg2) {}

SubExpression::SubExpression(TriplePtr const triple)
    : SubExpression(triple->getOperator(), triple->getArg1()) {
  if (triple->containsArg2()) {
    this->arg2 = triple->getArg2();
  }

  switch (this->op.getType()) {
    case mcc::tac::OperatorType::NONE:
      break;
    case mcc::tac::OperatorType::BINARY:
      assert(this->containsArg2() && "SubExpression needs arg2 for BinOp");
    case mcc::tac::OperatorType::UNARY:
      assert(this->containsArg1() && "SubExpression needs arg1");
      break;
  }
}

bool SubExpression::operator<(SubExpression const other) const {
  return (this->toString() < other.toString());
}

bool SubExpression::operator>(SubExpression const other) const {
  return (this->toString() > other.toString());
}

bool SubExpression::operator<=(SubExpression const other) const {
  return (this->toString() <= other.toString());
}

bool SubExpression::operator>=(SubExpression const other) const {
  return (this->toString() >= other.toString());
}

bool SubExpression::operator==(SubExpression const other) const {
  return (this->toString() == other.toString());
}

bool SubExpression::operator!=(SubExpression const other) const {
  return (this->toString() != other.toString());
}

std::string SubExpression::toString() const {
  if (this->containsArg2()) {
    std::string out = this->arg1->getValue();
    out.append(this->op.toString());
    out.append(this->arg2->getValue());

    return out;
  } else {
    std::string out = this->op.toString();
    out.append(" " + this->arg1->getValue());

    return out;
  }
}

bool SubExpression::containsArg1() const { return (this->arg1 != nullptr); }

bool SubExpression::containsArg2() const { return (this->arg2 != nullptr); }

SubExpression::OperandPtr SubExpression::getArg1() const { return this->arg1; }

SubExpression::OperandPtr SubExpression::getArg2() const { return this->arg2; }

SubExpression::Operator SubExpression::getOperator() const { return this->op; }

mcc::tac::Variable::set_t SubExpression::getVariables() const {
  using namespace mcc::tac;

  Variable::set_t vars;

  if (helper::isType<Triple>(this->arg1)) {
    auto triple = std::static_pointer_cast<Triple>(this->arg1);

    if (triple->containsTargetVar()) {
      vars.insert(triple->getTargetVariable());
    }
  }

  if (this->containsArg2() && helper::isType<Triple>(this->arg2)) {
    auto triple = std::static_pointer_cast<Triple>(this->arg2);

    if (triple->containsTargetVar()) {
      vars.insert(triple->getTargetVariable());
    }
  }

  return vars;
}
}
}
