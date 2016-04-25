/*
 * sub_expression_tests.cpp
 *
 *  Created on: Apr 25, 2016
 */

#include <gtest/gtest.h>

#include "mcc/cfg/sub_expression.h"

#include <string>

#include "mcc/tac/triple.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/operator.h"

namespace mcc {
  namespace cfg {

    using namespace mcc::tac;

    TEST(SubExpression, Unary) {
      std::shared_ptr<IntLiteral> i = std::make_shared<IntLiteral>(42);
      Operator op = Operator(OperatorName::NOT);

      SubExpression se = SubExpression(op, i);

      EXPECT_EQ(op.getName(), se.getOperator().getName());
      EXPECT_EQ(i, se.getArg1());
    }

    TEST(SubExpression, Binary) {
      std::shared_ptr<IntLiteral> i = std::make_shared<IntLiteral>(42);
      std::shared_ptr<IntLiteral> j = std::make_shared<IntLiteral>(43);
      Operator op = Operator(OperatorName::ADD);

      SubExpression se = SubExpression(op, i, j);

      EXPECT_EQ(op.getName(), se.getOperator().getName());
      EXPECT_EQ(i, se.getArg1());
      EXPECT_EQ(j, se.getArg2());
    }

    TEST(SubExpression, UnaryToString) {
      std::shared_ptr<IntLiteral> i = std::make_shared<IntLiteral>(42);
      Operator op = Operator(OperatorName::NOT);

      SubExpression se = SubExpression(op, i);

      EXPECT_EQ("! 42", se.toString());
    }

    TEST(SubExpression, BinaryToString) {
      std::shared_ptr<IntLiteral> i = std::make_shared<IntLiteral>(42);
      std::shared_ptr<IntLiteral> j = std::make_shared<IntLiteral>(43);
      Operator op = Operator(OperatorName::ADD);

      SubExpression se = SubExpression(op, i, j);

      EXPECT_EQ("42 + 43", se.toString());
    }

    TEST(SubExpression, UnaryTriple) {
      std::shared_ptr<IntLiteral> i = std::make_shared<IntLiteral>(42);
      Operator op = Operator(OperatorName::NOT);

      auto const t1 = std::make_shared<Triple>(op, i);

      SubExpression se = SubExpression(t1);

      EXPECT_EQ(t1->getOperator().getName(), se.getOperator().getName());
      EXPECT_EQ(t1->getArg1(), se.getArg1());
    }

    TEST(SubExpression, BinaryTriple) {
      std::shared_ptr<IntLiteral> i = std::make_shared<IntLiteral>(42);
      std::shared_ptr<IntLiteral> j = std::make_shared<IntLiteral>(43);
      Operator op = Operator(OperatorName::ADD);

      auto const t1 = std::make_shared<Triple>(op, i, j);

      SubExpression se = SubExpression(t1);

      EXPECT_EQ(t1->getOperator().getName(), se.getOperator().getName());
      EXPECT_EQ(t1->getArg1(), se.getArg1());
      EXPECT_EQ(t1->getArg2(), se.getArg2());
    }

    TEST(SubExpression, GetVariables) {
      std::shared_ptr<IntLiteral> i = std::make_shared<IntLiteral>(42);
      std::shared_ptr<IntLiteral> j = std::make_shared<IntLiteral>(43);
      Operator addOp = Operator(OperatorName::ADD);

      SubExpression se1 = SubExpression(addOp, i, j);

      EXPECT_EQ(0, se1.getVariables().size());

      Operator notOp = Operator(OperatorName::NOT);
      std::set<std::shared_ptr<Variable>> expectedVarSet;

      auto const t1 = std::make_shared<Triple>(notOp, i);
      expectedVarSet.insert(t1->getTargetVariable());

      SubExpression se2 = SubExpression(addOp, t1, j);

      EXPECT_EQ(expectedVarSet, se2.getVariables());

      auto const t2 = std::make_shared<Triple>(notOp, j);
      expectedVarSet.insert(t2->getTargetVariable());

      SubExpression se3 = SubExpression(addOp, t1, t2);

      EXPECT_EQ(expectedVarSet, se3.getVariables());
    }
    
    TEST(SubExpression, SetNoDuplicates) {
        auto i = std::make_shared<IntLiteral>(42);
        auto j = std::make_shared<IntLiteral>(43);

        auto op1 = Operator(OperatorName::ADD);
        auto op2 = Operator(OperatorName::ADD);

        auto se1 = std::make_shared<SubExpression>(op1, i, j);
        auto se2 = std::make_shared<SubExpression>(op2, i, j);
                
        std::set<std::shared_ptr<SubExpression>, SubExpression::less> set;
                
        set.insert(se1);
        set.insert(se2);

        EXPECT_EQ(set.size(), 1);
    }
  }
}

