#include <gtest/gtest.h>

#include "mcc/gas/gas.h"

#include "mcc/tac/helper/ast_converters.h"
#include "parser.h"
#include "test_utils.h"

#include <iostream>
#include <regex>

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
    }
  }
}

// TODO stupid test -> reimplement it
TEST(Gas, GasGeneration) {
  auto tree = parser::parse(
      R"(
        void print_int(int);
        void print_float(float);

        float foo(int arg1, float arg2);
        float bar(float arg1, int arg2);

        float bar(float arg1, int arg2) {
            print_int(2);

            if(arg1 > 5.0) {
                arg1 = arg1 - 1.0;
            }

            return foo(arg1, arg2);
        }

        float foo(int arg1, float arg2) {
            print_int(1);

            if(arg1 <= 5) {
                return 42.0;
            } else {
                return bar(arg2, arg1);
            }
        }

        int main() {
          int i = 0;
          while(i < 10) {
            print_float(foo(i, 13.2));
            i = i + 1;
          }

          return 0;
        }
        )");

  Tac tac = Tac(tree);
  Gas gas = Gas(tac);

  auto labels = std::vector<std::string>();
  for (auto triple : tac.codeLines) {
    switch (triple->getOperator().getName()) {
      case OperatorName::LABEL: {
        auto label = std::static_pointer_cast<Label>(triple);
        if (!label->isFunctionEntry()) {
          labels.push_back(label->getValue());
        }
      } break;
      case OperatorName::JUMP: {
        auto label = std::static_pointer_cast<Label>(triple->getArg1());
        if (!label->isFunctionEntry()) {
          labels.push_back(label->getValue());
        }
      } break;
      case OperatorName::JUMPFALSE: {
        auto label = std::static_pointer_cast<Label>(triple->getArg2());
        if (!label->isFunctionEntry()) {
          labels.push_back(label->getValue());
        }
      } break;
      default:
        // ignore
        break;
    }
  }

  auto curLabel = labels.begin();

  auto expected = std::string(".intel_syntax noprefix\n");
  expected.append(".global main\n\n");

  expected.append("bar:\n");
  expected.append("\tpush ebp\n");
  expected.append("\tmov ebp, esp\n");
  expected.append("\tsub esp, 12\n");
  expected.append("\tmov eax, 2\n");
  expected.append("\tpush eax\n");
  expected.append("\tcall print_int\n");
  expected.append("\tadd esp, 4\n");
  expected.append("\tmov eax, DWORD PTR [ebp - 8]\n");
  expected.append("\tmov edx, DWORD PTR .FC0\n");
  expected.append("\tcmp eax, edx\n");
  expected.append("\tjle " + *curLabel++ + "\n");
  expected.append("\tfld DWORD PTR [ebp - 8]\n");
  expected.append("\tfld DWORD PTR .FC1\n");
  expected.append("\tfsubp st(1), st\n");
  expected.append("\tfstp DWORD PTR [ebp - 4]\n");
  expected.append("\tmov eax, DWORD PTR [ebp - 4]\n");
  expected.append("\tmov DWORD PTR [ebp - 8], eax\n\n");

  expected.append(*curLabel++ + ":\n");
  expected.append("\tmov eax, DWORD PTR [ebp + 12]\n");
  expected.append("\tpush eax\n");
  expected.append("\tmov eax, DWORD PTR [ebp - 8]\n");
  expected.append("\tpush eax\n");
  expected.append("\tcall foo\n");
  expected.append("\tadd esp, 8\n");
  expected.append("\tmov DWORD PTR [ebp - 12], eax\n");
  expected.append("\tmov eax, DWORD PTR [ebp - 12]\n");
  expected.append("\tadd esp, 12\n");
  expected.append("\tpop ebp\n");
  expected.append("\tret\n\n");

  expected.append("foo:\n");
  expected.append("\tpush ebp\n");
  expected.append("\tmov ebp, esp\n");
  expected.append("\tsub esp, 4\n");
  expected.append("\tmov eax, 1\n");
  expected.append("\tpush eax\n");
  expected.append("\tcall print_int\n");
  expected.append("\tadd esp, 4\n");
  expected.append("\tmov eax, DWORD PTR [ebp + 8]\n");
  expected.append("\tmov edx, 5\n");
  expected.append("\tcmp eax, edx\n");
  expected.append("\tjg " + *curLabel++ + "\n");
  expected.append("\tmov eax, DWORD PTR .FC2\n");
  expected.append("\tadd esp, 4\n");
  expected.append("\tpop ebp\n");
  expected.append("\tret\n");
  expected.append("\tjmp " + *curLabel++ + "\n\n");

  expected.append(*curLabel++ + ":\n");
  expected.append("\tmov eax, DWORD PTR [ebp + 8]\n");
  expected.append("\tpush eax\n");
  expected.append("\tmov eax, DWORD PTR [ebp + 12]\n");
  expected.append("\tpush eax\n");
  expected.append("\tcall bar\n");
  expected.append("\tadd esp, 8\n");
  expected.append("\tmov DWORD PTR [ebp - 4], eax\n");
  expected.append("\tmov eax, DWORD PTR [ebp - 4]\n");
  expected.append("\tadd esp, 4\n");
  expected.append("\tpop ebp\n");
  expected.append("\tret\n\n");

  expected.append(*curLabel++ + ":\n\n");

  expected.append("main:\n");
  expected.append("\tpush ebp\n");
  expected.append("\tmov ebp, esp\n");
  expected.append("\tsub esp, 16\n");
  expected.append("\tmov eax, 0\n");
  expected.append("\tmov DWORD PTR [ebp - 16], eax\n\n");

  expected.append(*curLabel++ + ":\n");
  expected.append("\tmov eax, DWORD PTR [ebp - 16]\n");
  expected.append("\tmov edx, 10\n");
  expected.append("\tcmp eax, edx\n");
  expected.append("\tjge " + *curLabel++ + "\n");
  expected.append("\tmov eax, DWORD PTR .FC3\n");
  expected.append("\tpush eax\n");
  expected.append("\tmov eax, DWORD PTR [ebp - 16]\n");
  expected.append("\tpush eax\n");
  expected.append("\tcall foo\n");
  expected.append("\tadd esp, 8\n");
  expected.append("\tmov DWORD PTR [ebp - 8], eax\n");
  expected.append("\tmov eax, DWORD PTR [ebp - 8]\n");
  expected.append("\tpush eax\n");
  expected.append("\tcall print_float\n");
  expected.append("\tadd esp, 4\n");
  expected.append("\tmov eax, DWORD PTR [ebp - 16]\n");
  expected.append("\tmov ecx, 1\n");
  expected.append("\tadd eax, ecx\n");
  expected.append("\tmov DWORD PTR [ebp - 12], eax\n");
  expected.append("\tmov eax, DWORD PTR [ebp - 12]\n");
  expected.append("\tmov DWORD PTR [ebp - 16], eax\n");
  expected.append("\tjmp " + *curLabel++ + "\n\n");

  expected.append(*curLabel++ + ":\n");
  expected.append("\tmov eax, 0\n");
  expected.append("\tadd esp, 16\n");
  expected.append("\tpop ebp\n");
  expected.append("\tret\n\n");

  expected.append(".FC0: .float 5.000000\n");
  expected.append(".FC1: .float 1.000000\n");
  expected.append(".FC2: .float 42.000000\n");
  expected.append(".FC3: .float 13.200000\n\n");

  expected.append(".att_syntax noprefix\n");

  EXPECT_EQ(expected, gas.toString());
}
}
}
