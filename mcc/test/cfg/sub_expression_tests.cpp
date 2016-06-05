/*
 * sub_expression_tests.cpp
 *
 *  Created on: Apr 25, 2016
 */

#include <gtest/gtest.h>

#include "mcc/cfg/sub_expression.h"

#include <string>

#include "mcc/tac/int_literal.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/triple.h"

using namespace mcc::tac;

namespace mcc {
namespace cfg {

TEST(SubExpression, Unary) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  Operator op = Operator(OperatorName::NOT);

  SubExpression se = SubExpression(op, i);

  EXPECT_EQ(op.getName(), se.getOperator().getName());
  EXPECT_EQ(i, se.getArg1());
}

TEST(SubExpression, Binary) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  IntLiteral::ptr_t j = std::make_shared<IntLiteral>(43, scope);
  Operator op = Operator(OperatorName::ADD);

  SubExpression se = SubExpression(op, i, j);

  EXPECT_EQ(op.getName(), se.getOperator().getName());
  EXPECT_EQ(i, se.getArg1());
  EXPECT_EQ(j, se.getArg2());
}

TEST(SubExpression, UnaryToString) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  Operator op = Operator(OperatorName::NOT);

  SubExpression se = SubExpression(op, i);

  EXPECT_EQ("! 42", se.toString());
}

TEST(SubExpression, BinaryToString) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  IntLiteral::ptr_t j = std::make_shared<IntLiteral>(43, scope);
  Operator op = Operator(OperatorName::ADD);

  SubExpression se = SubExpression(op, i, j);

  EXPECT_EQ("42 + 43", se.toString());
}

TEST(SubExpression, UnaryTriple) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  Operator op = Operator(OperatorName::NOT);

  auto const t1 = std::make_shared<Triple>(op, i, scope);

  SubExpression se = SubExpression(t1);

  EXPECT_EQ(t1->getOperator().getName(), se.getOperator().getName());
  EXPECT_EQ(t1->getArg1(), se.getArg1());
}

TEST(SubExpression, BinaryTriple) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  IntLiteral::ptr_t j = std::make_shared<IntLiteral>(43, scope);
  Operator op = Operator(OperatorName::ADD);

  auto const t1 = std::make_shared<Triple>(op, i, j, scope);

  SubExpression se = SubExpression(t1);

  EXPECT_EQ(t1->getOperator().getName(), se.getOperator().getName());
  EXPECT_EQ(t1->getArg1(), se.getArg1());
  EXPECT_EQ(t1->getArg2(), se.getArg2());
}

TEST(SubExpression, GetVariables) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  IntLiteral::ptr_t j = std::make_shared<IntLiteral>(43, scope);
  Operator addOp = Operator(OperatorName::ADD);

  SubExpression se1 = SubExpression(addOp, i, j);

  EXPECT_EQ(0, se1.getVariables().size());

  Operator notOp = Operator(OperatorName::NOT);
  Variable::set_t expectedVarSet;

  auto const t1 = std::make_shared<Triple>(notOp, i, scope);
  expectedVarSet.insert(t1->getTargetVariable());

  SubExpression se2 = SubExpression(addOp, t1, j);

  EXPECT_EQ(expectedVarSet, se2.getVariables());

  auto const t2 = std::make_shared<Triple>(notOp, j, scope);
  expectedVarSet.insert(t2->getTargetVariable());

  SubExpression se3 = SubExpression(addOp, t1, t2);

  EXPECT_EQ(expectedVarSet, se3.getVariables());
}

TEST(SubExpression, SetNoDuplicates) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto i = std::make_shared<IntLiteral>(42, scope);
  auto j = std::make_shared<IntLiteral>(43, scope);

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
