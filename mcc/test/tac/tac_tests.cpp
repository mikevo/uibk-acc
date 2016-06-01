#include <gtest/gtest.h>

#include "mcc/tac/tac.h"

#include <memory>
#include <string>

#include "ast.h"
#include "mcc/tac/type.h"
#include "parser.h"

namespace mcc {
namespace tac {

TEST(Tac, ADD) {
  auto tree = parser::parse(R"(1 + 2;)");

  Tac tac = Tac(tree);

  std::string expectedValue = tac.codeLines.back().get()->getValue();
  expectedValue.append(" = 1 + 2");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, SUB) {
  auto tree = parser::parse(R"(1 - 2;)");

  Tac tac = Tac(tree);

  std::string expectedValue = tac.codeLines.back().get()->getValue();
  expectedValue.append(" = 1 - 2");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, MUL) {
  auto tree = parser::parse(R"(1 * 2;)");

  Tac tac = Tac(tree);

  std::string expectedValue = tac.codeLines.back().get()->getValue();
  expectedValue.append(" = 1 * 2");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, DIV) {
  auto tree = parser::parse(R"(1 / 2;)");

  Tac tac = Tac(tree);

  std::string expectedValue = tac.codeLines.back().get()->getValue();
  expectedValue.append(" = 1 / 2");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::FLOAT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, EQ) {
  auto tree = parser::parse(R"(1 == 2;)");

  Tac tac = Tac(tree);

  std::string expectedValue = tac.codeLines.back().get()->getValue();
  expectedValue.append(" = 1 == 2");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, NE) {
  auto tree = parser::parse(R"(1 != 2;)");

  Tac tac = Tac(tree);

  std::string expectedValue = tac.codeLines.back().get()->getValue();
  expectedValue.append(" = 1 != 2");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, LE) {
  auto tree = parser::parse(R"(1 <= 2;)");

  Tac tac = Tac(tree);

  std::string expectedValue = tac.codeLines.back().get()->getValue();
  expectedValue.append(" = 1 <= 2");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, GE) {
  auto tree = parser::parse(R"(1 >= 2;)");

  Tac tac = Tac(tree);

  std::string expectedValue = tac.codeLines.back().get()->getValue();
  expectedValue.append(" = 1 >= 2");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, LT) {
  auto tree = parser::parse(R"(1 < 2;)");

  Tac tac = Tac(tree);

  std::string expectedValue = tac.codeLines.back().get()->getValue();
  expectedValue.append(" = 1 < 2");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, GT) {
  auto tree = parser::parse(R"(1 > 2;)");

  Tac tac = Tac(tree);

  std::string expectedValue = tac.codeLines.back().get()->getValue();
  expectedValue.append(" = 1 > 2");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, DeclarationInt) {
  auto tree = parser::parse(R"(int a;)");

  Tac tac = Tac(tree);

  EXPECT_EQ(0, tac.codeLines.size());
}

TEST(Tac, DeclarationInt2) {
  auto tree = parser::parse(R"(int a = 1;)");

  std::string expectedValue = "a0:0:0 = 1";

  Tac tac = Tac(tree);

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, DeclarationIntWithPlus) {
  auto tree = parser::parse(R"(int a = 1 + 2;)");

  Tac tac = Tac(tree);

  auto expectedValue = "a0:0:0 = 1 + 2";

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, DeclarationIntWithPlus2) {
  auto tree = parser::parse(R"(int a = 1 + 2 + 3;)");

  Tac tac = Tac(tree);

  auto tempVarName = tac.codeLines.front().get()->getValue();

  std::string expectedValue = tempVarName;
  expectedValue.append(" = 2 + 3\n");
  expectedValue.append("a0:0:0 = 1 + ");
  expectedValue.append(tempVarName);

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(2, tac.codeLines.size());
}

TEST(Tac, DeclarationFloat) {
  auto tree = parser::parse(R"(float b = 1.0;)");

  std::string expectedValue = "b0:0:0 = " + std::to_string(1.0);

  Tac tac = Tac(tree);

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::FLOAT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, Minus) {
  auto tree = parser::parse(R"(-2;)");

  Tac tac = Tac(tree);

  std::string expectedValue = tac.codeLines.back().get()->getValue();
  expectedValue.append(" = -2");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(1, tac.codeLines.size());
}

TEST(Tac, Not) {
  auto tree = parser::parse(R"(!(1==2);)");

  Tac tac = Tac(tree);

  auto elem = tac.codeLines.begin();

  auto tempId = elem->get()->getTargetVariable()->getId();
  auto temp = "$t" + std::to_string(tempId);
  elem++;
  auto tempId2 = elem->get()->getTargetVariable()->getId();
  auto temp2 = "$t" + std::to_string(tempId2);

  std::string expectedValue = temp;
  expectedValue.append(" = 1 == 2\n");
  expectedValue.append(temp2);
  expectedValue.append(" = !");
  expectedValue.append(temp);

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
  EXPECT_EQ(2, tac.codeLines.size());
}

TEST(Tac, Parenthesis) {
  auto tree = parser::parse(R"(int a = (1 + 2) + 3;)");

  Tac tac = Tac(tree);

  auto tempVarName = tac.codeLines.front().get()->getValue();

  std::string expectedValue = tempVarName;
  expectedValue.append(" = 1 + 2\n");
  expectedValue.append("a0:0:0 = ");
  expectedValue.append(tempVarName);
  expectedValue.append(" + 3");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(2, tac.codeLines.size());
}

TEST(Tac, Compound) {
  auto tree = parser::parse(R"({int a = 1 + 2; int b = 3;})");

  Tac tac = Tac(tree);

  auto tempVarName = tac.codeLines.front().get()->getValue();

  std::string expectedValue = "a0:1:0 = 1 + 2\n";
  expectedValue.append("b0:1:0 = 3");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(2, tac.codeLines.size());
}

TEST(Tac, IfElse) {
  auto tree = parser::parse(
      R"(
        {
          int a = 0;
          if( 1 <= 2) {
            a = 1;
          } else {
            a = 2;
          }
        })");

  Tac tac = Tac(tree);

  auto elem = tac.codeLines.begin();
  elem++;

  auto varId = elem->get()->getTargetVariable()->getId();
  auto tempId = elem->get()->getId();
  auto varName = "$t" + std::to_string(varId);
  auto label = "$L" + std::to_string(tempId + 1);
  auto label2 = "$L" + std::to_string(tempId + 4);

  std::string expectedValue = "a0:1:0 = 0\n";
  expectedValue.append(varName + " = 1 <= 2\n");
  expectedValue.append("JUMPFALSE " + varName + " " + label + "\n");
  expectedValue.append("a0:1:0 = 1\n");
  expectedValue.append("JUMP " + label2 + "\n");
  expectedValue.append("LABEL " + label + "\n");
  expectedValue.append("a0:1:0 = 2\n");
  expectedValue.append("LABEL " + label2);

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(8, tac.codeLines.size());
}

TEST(Tac, If) {
  auto tree = parser::parse(
      R"(
        {
          int a = 0;
          if(1 <= 2) {
            a = 1;
          } 
        })");

  Tac tac = Tac(tree);

  auto elem = tac.codeLines.begin();
  elem++;

  auto tempId = elem->get()->getId();
  auto varId = elem->get()->getTargetVariable()->getId();
  auto varName = "$t" + std::to_string(varId);
  auto label = "$L" + std::to_string(tempId + 1);

  std::string expectedValue = "a0:1:0 = 0\n";
  expectedValue.append(varName + " = 1 <= 2\n");
  expectedValue.append("JUMPFALSE " + varName + " " + label + "\n");
  expectedValue.append("a0:1:0 = 1\n");
  expectedValue.append("LABEL " + label);

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(5, tac.codeLines.size());
}

TEST(Tac, While) {
  auto tree = parser::parse(
      R"(
        {
          int a = 0;
          while(a < 30) {
            int b = 0;
            while(b < 5) {
              b = b + 1;
            }
            a = a + b;
          }
        })");

  Tac tac = Tac(tree);

  auto elem = tac.codeLines.begin();
  elem++;

  auto labelId = elem->get()->getId();
  auto againLabel = "$L" + std::to_string(labelId);

  ++elem;
  auto condVarId = elem->get()->getTargetVariable()->getId();
  auto condVarName = "$t" + std::to_string(condVarId);

  labelId += 2;
  auto exitLabel = "$L" + std::to_string(labelId);

  std::string expectedValue = "a0:1:0 = 0\n";
  expectedValue.append("LABEL " + againLabel + "\n");
  expectedValue.append(condVarName + " = a0:1:0 < 30\n");
  expectedValue.append("JUMPFALSE " + condVarName + " " + exitLabel + "\n");
  expectedValue.append("b0:2:0 = 0\n");

  labelId += 3;
  auto again2Label = "$L" + std::to_string(labelId);
  labelId += 2;
  auto exit2Label = "$L" + std::to_string(labelId);

  condVarId += 3;
  condVarName = "$t" + std::to_string(condVarId);
  expectedValue.append("LABEL " + again2Label + "\n");
  expectedValue.append(condVarName + " = b0:2:0 < 5\n");
  expectedValue.append("JUMPFALSE " + condVarName + " " + exit2Label + "\n");

  condVarId += 5;
  condVarName = "$t" + std::to_string(condVarId);
  expectedValue.append(condVarName + " = b0:2:0 + 1\n");
  expectedValue.append("b0:2:0 = " + condVarName + "\n");
  expectedValue.append("JUMP " + again2Label + "\n");
  expectedValue.append("LABEL " + exit2Label + "\n");

  condVarId += 6;
  condVarName = "$t" + std::to_string(condVarId);
  expectedValue.append(condVarName + " = a0:1:0 + b0:2:0\n");
  expectedValue.append("a0:1:0 = " + condVarName + "\n");
  expectedValue.append("JUMP " + againLabel + "\n");
  expectedValue.append("LABEL " + exitLabel);

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(16, tac.codeLines.size());
}

TEST(Tac, Array) {
  auto tree = parser::parse(R"({int[5] a; int b; a[2] = 1; b = a[2] + 1;})");

  Tac tac = Tac(tree);

  auto tempVarName = tac.codeLines.front().get()->getValue();

  std::string expectedValue = "a0:1:0 = 1 + 2\n";
  expectedValue.append("b0:1:0 = 3");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
  EXPECT_EQ(2, tac.codeLines.size());
}
}
}
