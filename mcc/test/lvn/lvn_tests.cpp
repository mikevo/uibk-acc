#include "mcc/lvn/lvn.h"
#include "ast.h"
#include "mcc/tac/tac.h"
#include "parser.h"
#include <gtest/gtest.h>

using namespace mcc::tac;

namespace mcc {
namespace lvn {
TEST(LVN, ConstantExpression) {
  auto tree = parser::parse(R"(
        {
            float x = 3.5 - 1.5; 
            int y = 5 + 3;
            int z = 5 + 3;
            int a = 8 * 5;

        })");

  Tac tac = Tac(tree);
  LVN::transform(tac);

  std::string expectedValue = "x0:1:0 = 2.000000\n";
  expectedValue.append("y0:1:0 = 8\n");
  expectedValue.append("z0:1:0 = 8\n");
  expectedValue.append("a0:1:0 = 40");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(4, tac.codeLines.size());
}

TEST(LVN, RedundantExpression) {
  auto tree = parser::parse(R"(
        {   
            int x = 8 * 2;
            int y = x + 12;
            y = 10;
            int z = x + 12;

        })");

  Tac tac = Tac(tree);
  LVN::transform(tac);
  
  auto tempID = tac.codeLines[2]->getTargetVariable()->getId();
  auto tempVar = "$t" + std::to_string(tempID) + ":0:0";

  std::string expectedValue = "x0:1:0 = 16\n";
  expectedValue.append("y0:1:0 = x0:1:0 + 12\n");
  expectedValue.append(tempVar + " = y0:1:0\n");
  expectedValue.append("y0:1:0 = 10\n");
  expectedValue.append("z0:1:0 = " +tempVar);

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(5, tac.codeLines.size());
}
}
}
