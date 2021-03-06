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

SubExpression::SubExpression(Operator op, mcc::tac::Operand::ptr_t arg)
    : SubExpression(op, arg, nullptr) {}

SubExpression::SubExpression(Operator op, mcc::tac::Operand::ptr_t arg1,
                             mcc::tac::Operand::ptr_t arg2)
    : op(op), arg1(arg1), arg2(arg2) {}

SubExpression::SubExpression(mcc::tac::Triple::ptr_t const triple)
    : SubExpression(triple->getOperator(), nullptr) {
  if (triple->containsArg1()) {
    this->arg1 = triple->getArg1();
  }

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
    case mcc::tac::OperatorType::RETURN:
      break;
    case mcc::tac::OperatorType::CALL:
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

mcc::tac::Operand::ptr_t SubExpression::getArg1() const { return this->arg1; }

mcc::tac::Operand::ptr_t SubExpression::getArg2() const { return this->arg2; }

SubExpression::Operator SubExpression::getOperator() const { return this->op; }

mcc::tac::Variable::set_t SubExpression::getVariables() const {
  using namespace mcc::tac;

  Variable::set_t vars;

  if (this->op.getName() != mcc::tac::OperatorName::ASSIGN) {
    if (this->containsArg1()) {
      if (helper::isType<Triple>(this->arg1)) {
        auto triple = std::static_pointer_cast<Triple>(this->arg1);

        if (triple->containsTargetVar()) {
          vars.insert(triple->getTargetVariable());
        }
      }

      if (helper::isType<Variable>(this->arg1)) {
        auto var = std::static_pointer_cast<Variable>(this->arg1);
        vars.insert(var);
      }
    }
  }

  if (this->containsArg2()) {
    if (helper::isType<Triple>(this->arg2)) {
      auto triple = std::static_pointer_cast<Triple>(this->arg2);

      if (triple->containsTargetVar()) {
        vars.insert(triple->getTargetVariable());
      }
    }

    if (helper::isType<Variable>(this->arg2)) {
      auto var = std::static_pointer_cast<Variable>(this->arg2);
      vars.insert(var);
    }
  }

  return vars;
}
}
}
