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
            } else {
              return 3.0;
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

  auto expected = R"(.intel_syntax noprefix
.global main

bar:
	push ebp
	mov ebp, esp
	sub esp, 12
	push ebx
	push edi
	push esi
	mov ecx, DWORD PTR [ebp - 8]
	mov ebx, DWORD PTR [ebp + 12]
	mov ebx, DWORD PTR [ebp + 12]
	push ecx
	push edx
	push 2
	call print_int
	add esp, 4
	pop edx
	pop ecx
	fld DWORD PTR [ebp - 8]
	fld DWORD PTR .FC0
	fxch st(1)
	fucomip st, st(1)
	fstp st(0)
	jbe $L609
	fld DWORD PTR [ebp - 8]
	fld DWORD PTR .FC1
	fsubp st(1), st
	fstp DWORD PTR [ebp - 4]
	mov ecx, edx
	jmp $L613

$L609:
	mov eax, DWORD PTR .FC2
	add esp, 12
	mov esp, ebp
	pop ebp
	ret

$L613:
	mov eax, DWORD PTR [ebp + 12]
	push ecx
	push edx
	push eax
	push ecx
	call foo
	add esp, 8
	pop edx
	pop ecx
	mov ecx, eax
	mov eax, ecx
	add esp, 12
	mov esp, ebp
	pop ebp
	ret

foo:
	push ebp
	mov ebp, esp
	sub esp, 4
	push ebx
	push edi
	push esi
	mov ecx, DWORD PTR [ebp + 8]
	mov ebx, DWORD PTR [ebp + 12]
	mov ebx, DWORD PTR [ebp + 12]
	push ecx
	push edx
	push 1
	call print_int
	add esp, 4
	pop edx
	pop ecx
	cmp ecx, 5
	jg $L625
	mov eax, DWORD PTR .FC3
	add esp, 4
	mov esp, ebp
	pop ebp
	ret
	jmp $L628

$L625:
	push ecx
	push ecx
	push edx
	mov eax, DWORD PTR [ebp + 12]
	push eax
	call bar
	add esp, 8
	pop edx
	pop ecx
	mov ecx, eax
	mov eax, ecx
	add esp, 4
	mov esp, ebp
	pop ebp
	ret

$L628:

main:
	push ebp
	mov ebp, esp
	sub esp, 16
	push ebx
	push edi
	push esi
	mov edx, 0

$L636:
	cmp edx, 10
	jge $L638
	push ecx
	push edx
	push DWORD PTR .FC4
	push edx
	call foo
	add esp, 8
	pop edx
	pop ecx
	mov ecx, eax
	push ecx
	push edx
	push ecx
	call print_float
	add esp, 4
	pop edx
	pop ecx
	mov ebx, edx
	add edx, 1
	mov ecx, edx
	mov edx, ebx
	mov edx, ecx
	jmp $L636

$L638:
	mov eax, 0
	add esp, 16
	mov esp, ebp
	pop ebp
	ret

.FC0: .float 5.000000
.FC1: .float 1.000000
.FC2: .float 3.000000
.FC3: .float 42.000000
.FC4: .float 13.200000

.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}
}
}
