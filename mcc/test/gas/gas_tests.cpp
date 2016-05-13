#include <gtest/gtest.h>

#include "mcc/gas/gas.h"

#include "parser.h"
#include "test_utils.h"

#include <iostream>

using namespace mcc::gas;

namespace mcc {
namespace gas {
TEST(Gas, FunctionStackSpace) {
  auto tree = parser::parse(
      R"(
         void bar(int arg1);
  
         void foo(int arg1) {
            if(arg1 < 0) return;
            bar(arg1 - 1);
         }
         
         void bar(int arg1) {
            if(arg1 < 0) return;
            foo(arg1 - 1);
            bar(arg1 - 1);
         }   
            
          int main() {
             foo(10);
             return 0;
          }
        )");

  Tac tac = Tac(tree);
  Gas gas = Gas(tac);

  std::shared_ptr<function_stack_space_map_type> expected =
      std::make_shared<function_stack_space_map_type>();
  (*expected)["foo"] = 5;
  (*expected)["bar"] = 9;
  (*expected)["main"] = 0;

  EXPECT_EQ(*expected, *(gas.getFunctionStackSpaceMap()));
}

TEST(Gas, VariableStackOffset) {
  auto tree = parser::parse(
      R"(
         void bar(int arg1);
  
         void foo(int arg1) {
            if(arg1 < 0) return;
            bar(arg1 - 1);
         }
         
         void bar(int arg1) {
            if(arg1 < 0) return;
            foo(arg1 - 1);
            bar(arg1 - 1);
         }   
            
          int main() {
             foo(10);
             return 0;
          }
        )");

  Tac tac = Tac(tree);
  Gas gas = Gas(tac);

  auto stackOffsetMap = gas.getVariableStackOffsetMap();

  int varCounter = 0;
  unsigned expectedOffsets[] = {0, 4, 5, 9, 0, 4, 5, 9, 9, 13, 0};
  for (auto codeLine : tac.codeLines) {
    if (codeLine->containsTargetVar()) {
      auto targetVar = codeLine->getTargetVariable();
      auto targetVarOffset = stackOffsetMap->find(targetVar);
      // target variable has to be in stackOffsetMap
      EXPECT_NE(targetVarOffset, stackOffsetMap->end());

      EXPECT_EQ(expectedOffsets[varCounter++], targetVarOffset->second);
    }
  }
}
}
}
