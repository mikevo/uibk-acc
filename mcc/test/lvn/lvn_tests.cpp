#include "mcc/lvn/lvn.h"
#include "ast.h"
#include "mcc/tac/tac.h"
#include "parser.h"
#include <gtest/gtest.h>

using namespace mcc::tac;

namespace mcc {
namespace lvn {
TEST(LVN, DISABLED_ConstantExpression) {
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

TEST(LVN, DISABLED_RedundantExpression) {
  auto tree = parser::parse(R"(
        {   
            int x = 5;
            int y = x + 12; 
            int z = x + 12;
            int a = x + 12;

        })");

  Tac tac = Tac(tree);
  LVN::transform(tac);
  
  std::cout << tac.toString();

  std::string expectedValue = "x0:1:0 = 5\n";
  expectedValue.append("y0:1:0 = x0:1:0 + 12\n");
  expectedValue.append("z0:1:0 = y0:1:0");

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(3, tac.codeLines.size());
}
}
}
