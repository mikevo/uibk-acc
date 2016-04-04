#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/operator.h"

namespace mcc {
  namespace tac {

    TEST(Operator, NOP) {
      OperatorName name = OperatorName::NOP;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::LINE, op.getType());
      EXPECT_EQ(Type::AUTO, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, ADD) {
      OperatorName name = OperatorName::ADD;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::AUTO, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, ASSIGN) {
      OperatorName name = OperatorName::ASSIGN;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::AUTO, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, DIV) {
      OperatorName name = OperatorName::DIV;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::FLOAT, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, EQ) {
      OperatorName name = OperatorName::EQ;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::BOOL, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, NE) {
      OperatorName name = OperatorName::NE;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::BOOL, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, LE) {
      OperatorName name = OperatorName::LE;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::BOOL, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, GE) {
      OperatorName name = OperatorName::GE;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::BOOL, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, LT) {
      OperatorName name = OperatorName::LT;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::BOOL, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, GT) {
      OperatorName name = OperatorName::GT;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::BOOL, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, JUMP) {
      OperatorName name = OperatorName::JUMP;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::UNARY, op.getType());
      EXPECT_EQ(Type::AUTO, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, JUMPFALSE) {
      OperatorName name = OperatorName::JUMPFALSE;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::AUTO, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, LABEL) {
      OperatorName name = OperatorName::LABEL;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::UNARY, op.getType());
      EXPECT_EQ(Type::AUTO, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, MUL) {
      OperatorName name = OperatorName::MUL;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::AUTO, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, SUB) {
      OperatorName name = OperatorName::SUB;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::BINARY, op.getType());
      EXPECT_EQ(Type::AUTO, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, MINUS) {
      OperatorName name = OperatorName::MINUS;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::UNARY, op.getType());
      EXPECT_EQ(Type::AUTO, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }

    TEST(Operator, NOT) {
      OperatorName name = OperatorName::NOT;
      Operator op = Operator(name);

      EXPECT_EQ(OperatorType::UNARY, op.getType());
      EXPECT_EQ(Type::AUTO, op.getResultType());
      EXPECT_EQ(name, op.getName());
    }
  }
}
