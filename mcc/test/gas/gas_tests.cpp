#include <gtest/gtest.h>

#include "mcc/gas/gas.h"

#include "mcc/tac/helper/ast_converters.h"
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

  function_stack_space_map_type expected;

  auto l1 = std::make_shared<Label>("foo");
  auto l2 = std::make_shared<Label>("bar");
  auto l3 = std::make_shared<Label>("main");

  expected[l1] = 4;
  expected[l2] = 8;
  expected[l3] = 0;

  EXPECT_EQ(expected.size(), gas.getFunctionStackSpaceMap()->size());

  for (auto const e : expected) {
    auto result = gas.getFunctionStackSpaceMap()->find(e.first);

    EXPECT_NE(gas.getFunctionStackSpaceMap()->end(), result);
    EXPECT_EQ(e.second, result->second);
  }
}

TEST(Gas, VariableStackOffset) {
  auto tree = parser::parse(
      R"(
         void bar(int arg1);
  
         void foo(int arg1, int arg2) {
            if(arg1 < 0) return;
            bar(arg1 - 1);
         }
         
         void bar(int arg1) {
            if(arg1 < 0) return;
            foo(arg1 - 1, arg1);
            bar(arg1 - 1);
         }   
            
          int main() {
             foo(10, 10);
             return 0;
          }
        )");

  Tac tac = Tac(tree);
  Gas gas = Gas(tac);

  auto stackOffsetMap = gas.getVariableStackOffsetMap();
  Label::ptr_t currentFunction;
  int varCounter = 0;
  signed expectedOffsets[] = {8, 12, -4, -4, -8, 8, -4, -4, -8, -8, -12, -4};
  for (auto codeLine : tac.codeLines) {
    if (helper::isType<Label>(codeLine)) {
      auto label = std::static_pointer_cast<Label>(codeLine);
      if (label->isFunctionEntry()) {
        currentFunction = label;
      }
    }

    if (codeLine->containsTargetVar()) {
      auto targetVar = codeLine->getTargetVariable();
      auto targetVarOffset =
          stackOffsetMap->find(std::make_pair(currentFunction, targetVar));
      // target variable has to be in stackOffsetMap
      EXPECT_NE(targetVarOffset, stackOffsetMap->end());

      EXPECT_EQ(expectedOffsets[varCounter++], targetVarOffset->second);
      std::cout << targetVar->toString() << " -- " << targetVarOffset->second
                << std::endl;
    }
  }
}
}
}
