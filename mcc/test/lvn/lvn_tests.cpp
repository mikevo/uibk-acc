#include "mcc/lvn/lvn.h"
#include "ast.h"
#include "mcc/tac/tac.h"
#include "parser.h"
#include <gtest/gtest.h>

using namespace mcc::tac;

namespace mcc {
namespace lvn {
TEST(LVN, ConstantExpression) {
  auto tree = parser::parse(
      R"(
         {
            int x=1 + 2;
            float y = 3.0 - 1.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0 + 2;
         })");

  Tac tac = Tac(tree);

  auto currentLine = tac.codeLines.begin();
  auto x = (currentLine++)->get()->getTargetVariable()->toString();
  std::string expectedValue = x + " = 3\n";
  auto y = (currentLine++)->get()->getTargetVariable()->toString();
  expectedValue.append(y + " = 2.000000\n");

  while ((currentLine + 1) != tac.codeLines.end()) {
    expectedValue.append((currentLine++)->get()->toString() + "\n");
  }

  auto a = currentLine->get()->getTargetVariable()->toString();
  expectedValue.append(a + " = 2");

  LVN::transform(tac);

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(34, tac.codeLines.size());
}

TEST(LVN, RedundantExpression) {
  auto tree = parser::parse(
      R"(
         {
            float x = 8.0 * 12.0;
            float y = x + 12.0;
            float z = x + 12.0;

            if(x > 0.0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  Tac tac = Tac(tree);

  auto currentLine = tac.codeLines.begin();
  auto x = (currentLine++)->get()->getTargetVariable()->toString();
  std::string expectedFirst = x + " = 96.000000\n";

  auto y = (currentLine)->get()->getTargetVariable()->toString();
  expectedFirst.append((currentLine++)->get()->toString() + "\n");
  auto z = (currentLine++)->get()->getTargetVariable()->toString();

  std::string expectedSecond;
  while ((currentLine + 1) != tac.codeLines.end()) {
    expectedSecond.append((currentLine++)->get()->toString() + "\n");
  }
  expectedSecond.append((currentLine)->get()->toString());

  LVN::transform(tac);

  auto temp = tac.codeLines.at(2)->getTargetVariable()->toString();
  expectedFirst.append(temp + " = " + y + "\n");
  expectedFirst.append(z + " = " + temp + "\n");

  std::string expectedValue = expectedFirst + expectedSecond;

  EXPECT_EQ(expectedValue, tac.toString());
  EXPECT_EQ(36, tac.codeLines.size());
}
}
}
