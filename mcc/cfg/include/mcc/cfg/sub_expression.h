/*
 * sub_expression.h
 *
 *  Created on: Apr 25, 2016
 */

#ifndef INCLUDES_MCC_CFG_SUB_EXPRESSION_H_
#define INCLUDES_MCC_CFG_SUB_EXPRESSION_H_

#include <memory>
#include <set>

#include "mcc/tac/operator.h"
#include "mcc/tac/operand.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/variable.h"

namespace mcc {
  namespace cfg {

    class SubExpression {
      public:
        typedef mcc::tac::Operator Operator;
        typedef std::shared_ptr<mcc::tac::Operand> OperandPtr;
        typedef std::shared_ptr<mcc::tac::Triple> TriplePtr;
        typedef std::shared_ptr<mcc::tac::Variable> VariablePtr;

        SubExpression(Operator op, OperandPtr arg);
        SubExpression(Operator op, OperandPtr arg1, OperandPtr arg2);
        SubExpression(TriplePtr const triple);

        std::string toString() const;

        bool containsArg1() const;
        bool containsArg2() const;

        OperandPtr getArg1() const;
        OperandPtr getArg2() const;

        Operator getOperator() const;

        std::set<VariablePtr> getVariales() const;

      private:
        Operator op;
        OperandPtr arg1, arg2;
    };
  }
}

#endif /* INCLUDES_MCC_CFG_SUB_EXPRESSION_H_ */
