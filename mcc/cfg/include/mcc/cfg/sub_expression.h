/*
 * sub_expression.h
 *
 *  Created on: Apr 25, 2016
 */

#ifndef INCLUDES_MCC_CFG_SUB_EXPRESSION_H_
#define INCLUDES_MCC_CFG_SUB_EXPRESSION_H_

#include <memory>
#include <set>

#include "mcc/tac/operand.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/variable.h"

namespace mcc {
namespace cfg {

class SubExpression {
 public:
  typedef std::shared_ptr<SubExpression> ptr_t;

  struct less {
    bool operator()(const ptr_t &lhs, const ptr_t &rhs) const {
      return *lhs.get() < *rhs.get();
    }
  };

  typedef std::set<ptr_t, less> set_t;

  typedef mcc::tac::Operator Operator;

  SubExpression(Operator op, mcc::tac::Operand::ptr_t arg);
  SubExpression(Operator op, mcc::tac::Operand::ptr_t arg1,
                mcc::tac::Operand::ptr_t arg2);
  SubExpression(mcc::tac::Triple::ptr_t const triple);

  bool operator<(SubExpression const other) const;
  bool operator>(SubExpression const other) const;
  bool operator<=(SubExpression const other) const;
  bool operator>=(SubExpression const other) const;
  bool operator==(SubExpression const other) const;
  bool operator!=(SubExpression const other) const;

  std::string toString() const;

  bool containsArg1() const;
  bool containsArg2() const;

  mcc::tac::Operand::ptr_t getArg1() const;
  mcc::tac::Operand::ptr_t getArg2() const;

  Operator getOperator() const;

  mcc::tac::Variable::set_t getVariables() const;

 private:
  Operator op;
  mcc::tac::Operand::ptr_t arg1, arg2;
};
}
}

#endif /* INCLUDES_MCC_CFG_SUB_EXPRESSION_H_ */
