#include <gtest/gtest.h>

#include "mcc/tac/tac.h"

#include <memory>
#include <string>

#include "ast.h"
#include "parser.h"
#include "mcc/tac/type.h"

namespace mcc {
  namespace tac {

    TEST(Tac, ADD) {
      auto tree = parser::parse(R"(1 + 2;)");


      Tac tac;
      tac.convertAst(tree);

      std::string expectedValue = tac.codeLines.back().get()->getValue();
      expectedValue.append(" = 1 + 2");

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, SUB) {
      auto tree = parser::parse(R"(1 - 2;)");

      Tac tac;
      tac.convertAst(tree);

      std::string expectedValue = tac.codeLines.back().get()->getValue();
      expectedValue.append(" = 1 - 2");

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, MUL) {
      auto tree = parser::parse(R"(1 * 2;)");

      Tac tac;
      tac.convertAst(tree);

      std::string expectedValue = tac.codeLines.back().get()->getValue();
      expectedValue.append(" = 1 * 2");

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, DIV) {
      auto tree = parser::parse(R"(1 / 2;)");

      Tac tac;
      tac.convertAst(tree);

      std::string expectedValue = tac.codeLines.back().get()->getValue();
      expectedValue.append(" = 1 / 2");

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, EQ) {
      auto tree = parser::parse(R"(1 == 2;)");

      Tac tac;
      tac.convertAst(tree);

      std::string expectedValue = tac.codeLines.back().get()->getValue();
      expectedValue.append(" = 1 == 2");

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, NE) {
      auto tree = parser::parse(R"(1 != 2;)");

      Tac tac;
      tac.convertAst(tree);

      std::string expectedValue = tac.codeLines.back().get()->getValue();
      expectedValue.append(" = 1 != 2");

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, LE) {
      auto tree = parser::parse(R"(1 <= 2;)");

      Tac tac;
      tac.convertAst(tree);

      std::string expectedValue = tac.codeLines.back().get()->getValue();
      expectedValue.append(" = 1 <= 2");

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, GE) {
      auto tree = parser::parse(R"(1 >= 2;)");

      Tac tac;
      tac.convertAst(tree);

      std::string expectedValue = tac.codeLines.back().get()->getValue();
      expectedValue.append(" = 1 >= 2");

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, LT) {
      auto tree = parser::parse(R"(1 < 2;)");

      Tac tac;
      tac.convertAst(tree);

      std::string expectedValue = tac.codeLines.back().get()->getValue();
      expectedValue.append(" = 1 < 2");

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, GT) {
      auto tree = parser::parse(R"(1 > 2;)");

      Tac tac;
      tac.convertAst(tree);

      std::string expectedValue = tac.codeLines.back().get()->getValue();
      expectedValue.append(" = 1 > 2");

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, DeclatrationInt) {
      auto tree = parser::parse(R"(int a;)");

      Tac tac;
      tac.convertAst(tree);

      auto tempVarName = tac.codeLines.front().get()->getValue();

      std::string expectedValue = tempVarName;

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, DeclatrationInt2) {
      auto tree = parser::parse(R"(int a = 1;)");

      std::string expectedValue = "a = 1";

      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, DeclatrationIntWithPlus) {
      auto tree = parser::parse(R"(int a = 1 + 2;)");

      Tac tac;
      tac.convertAst(tree);

      auto expectedValue = "a = 1 + 2";

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, DeclatrationIntWithPlus2) {
      auto tree = parser::parse(R"(int a = 1 + 2 + 3;)");

      Tac tac;
      tac.convertAst(tree);

      auto tempVarName = tac.codeLines.front().get()->getValue();

      std::string expectedValue = tempVarName;
      expectedValue.append(" = 2 + 3\n");
      expectedValue.append("a = 1 + ");
      expectedValue.append(tempVarName);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
      EXPECT_EQ(2, tac.codeLines.size());
    }

    TEST(Tac, DeclatrationFloat) {
      auto tree = parser::parse(R"(float b = 1.0;)");

      std::string expectedValue = "b = " + std::to_string(1.0);

      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::FLOAT, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }

    TEST(Tac, Minus) {
      auto tree = parser::parse(R"(-2;)");

      Tac tac;
      tac.convertAst(tree);

      std::string expectedValue = tac.codeLines.back().get()->getValue();
      expectedValue.append(" = -2");

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
      EXPECT_EQ(1, tac.codeLines.size());
    }
  }
}

