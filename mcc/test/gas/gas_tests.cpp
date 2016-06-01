#include <gtest/gtest.h>

#include "mcc/gas/gas.h"

#include "mcc/tac/float_literal.h"
#include "mcc/tac/helper/ast_converters.h"
#include "mcc/tac/int_literal.h"
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

  for (auto const e : expected) {
    auto result = gas.getRegisterManager()->lookupFunctionStackSpace(e.first);

    EXPECT_EQ(e.second, result);
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

  auto regMan = gas.getRegisterManager();

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
          regMan->lookupVariableStackOffset(currentFunction, targetVar);

      EXPECT_EQ(expectedOffsets[varCounter++], targetVarOffset);
    }
  }
}

/* Gas Conversion */
TEST(Gas, GasConversion) {
  auto tree = parser::parse(
      R"(
            void print_int(int out);
            int read_int();

            int fibonacci(int n) {
                if ( n == 0 )
                    return 0;
                else if ( n == 1 )
                    return 1;
                else
                    return (fibonacci(n-1) + fibonacci(n-2));
            } 

            int main(void) {
                int i = read_int();
                int fib = fibonacci(i); 
                print_int(fib);

                return 0;
            }
        )");

  auto tac = Tac(tree);
  Gas gas = Gas(tac);

  // get labels from tac
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

  auto expected = R"(.intel_syntax noprefix
.global main

fibonacci:
	push ebp
	mov ebp, esp
	sub esp, 20
	push ebx
	push edi
	push esi
	mov ecx, DWORD PTR [ebp + 8]
	cmp ecx, 0
	jne )" + *curLabel++ +
                  R"(
	mov eax, 0
	pop esi
	pop edi
	pop ebx
	add esp, 20
	mov esp, ebp
	pop ebp
	ret
	jmp )" + *curLabel++ +
                  R"(

)" + *curLabel++ +
                  R"(:
	cmp ecx, 1
	jne )" + *curLabel++ +
                  R"(
	mov eax, 1
	pop esi
	pop edi
	pop ebx
	add esp, 20
	mov esp, ebp
	pop ebp
	ret
	jmp )" + *curLabel++ +
                  R"(

)" + *curLabel++ +
                  R"(:
	mov eax, ecx
	sub eax, 1
	mov ebx, eax
	push ecx
	push edx
	push ebx
	call fibonacci
	add esp, 4
	pop edx
	pop ecx
	mov edx, eax
	mov eax, ecx
	sub eax, 2
	mov ebx, eax
	push ecx
	push edx
	push ebx
	call fibonacci
	add esp, 4
	pop edx
	pop ecx
	mov ebx, eax
	mov eax, edx
	add eax, ebx
	mov ecx, eax
	mov eax, ecx
	pop esi
	pop edi
	pop ebx
	add esp, 20
	mov esp, ebp
	pop ebp
	ret

)" + *curLabel++ +
                  R"(:

)" + *curLabel++ +
                  R"(:
	pop esi
	pop edi
	pop ebx
	add esp, 20
	mov esp, ebp
	pop ebp

main:
	push ebp
	mov ebp, esp
	sub esp, 8
	push ebx
	push edi
	push esi
	push ecx
	push edx
	call read_int
	pop edx
	pop ecx
	mov ebx, eax
	push ecx
	push edx
	push ebx
	call fibonacci
	add esp, 4
	pop edx
	pop ecx
	mov ebx, eax
	push ecx
	push edx
	push ebx
	call print_int
	add esp, 4
	pop edx
	pop ecx
	mov eax, 0
	pop esi
	pop edi
	pop ebx
	add esp, 8
	mov esp, ebp
	pop ebp
	ret


.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(Gas, GasAddIntegerConversion) {
  auto tree = parser::parse(
      R"(
        void main() {
        
        }
        )");

  Tac tac = Tac(tree);
  IntLiteral::ptr_t operandOne = std::make_shared<IntLiteral>(10);
  IntLiteral::ptr_t operandTwo = std::make_shared<IntLiteral>(15);
  Operator op = Operator(OperatorName::ADD);

  Triple::ptr_t addTriple =
      std::make_shared<Triple>(op, operandOne, operandTwo);
  tac.addLine(addTriple);

  Gas gas = Gas(tac);
  auto expected = R"(.intel_syntax noprefix
.global main

main:
	push ebp
	mov ebp, esp
	sub esp, 4
	push ebx
	push edi
	push esi
	mov eax, 10
	add eax, 15
	mov ebx, eax


.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(Gas, GasSubIntegerConversion) {
  auto tree = parser::parse(
      R"(
        void main() {
        
        }
        )");

  Tac tac = Tac(tree);
  IntLiteral::ptr_t operandOne = std::make_shared<IntLiteral>(10);
  IntLiteral::ptr_t operandTwo = std::make_shared<IntLiteral>(15);
  Operator op = Operator(OperatorName::SUB);

  Triple::ptr_t subTriple =
      std::make_shared<Triple>(op, operandOne, operandTwo);
  tac.addLine(subTriple);

  Gas gas = Gas(tac);
  auto expected = R"(.intel_syntax noprefix
.global main

main:
	push ebp
	mov ebp, esp
	sub esp, 4
	push ebx
	push edi
	push esi
	mov eax, 10
	sub eax, 15
	mov ebx, eax


.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(Gas, GasMulIntegerConversion) {
  auto tree = parser::parse(
      R"(
        void main() {
        
        }
        )");

  Tac tac = Tac(tree);
  IntLiteral::ptr_t operandOne = std::make_shared<IntLiteral>(10);
  IntLiteral::ptr_t operandTwo = std::make_shared<IntLiteral>(15);
  Operator op = Operator(OperatorName::MUL);

  Triple::ptr_t triple = std::make_shared<Triple>(op, operandOne, operandTwo);
  tac.addLine(triple);

  Gas gas = Gas(tac);

  auto expected = R"(.intel_syntax noprefix
.global main

main:
	push ebp
	mov ebp, esp
	sub esp, 4
	push ebx
	push edi
	push esi
	mov eax, 10
	imul eax, 15
	mov ebx, eax


.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(Gas, GasAddFloatConversion) {
  auto tree = parser::parse(
      R"(
        void main() {
        
        }
        )");

  Tac tac = Tac(tree);
  FloatLiteral::ptr_t operandOne = std::make_shared<FloatLiteral>(10.0);
  FloatLiteral::ptr_t operandTwo = std::make_shared<FloatLiteral>(15.0);
  Operator op = Operator(OperatorName::ADD);

  Triple::ptr_t triple = std::make_shared<Triple>(op, operandOne, operandTwo);
  tac.addLine(triple);

  Gas gas = Gas(tac);

  auto expected = R"(.intel_syntax noprefix
.global main

main:
	push ebp
	mov ebp, esp
	sub esp, 4
	push ebx
	push edi
	push esi
	fld DWORD PTR .FC0
	fld DWORD PTR .FC1
	faddp st(1), st
	fstp DWORD PTR [ebp - 4]
	mov ebx, DWORD PTR [ebp - 4]

.FC0: .float 10.000000
.FC1: .float 15.000000

.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(Gas, GasSubFloatConversion) {
  auto tree = parser::parse(
      R"(
        void main() {
        
        }
        )");

  Tac tac = Tac(tree);
  FloatLiteral::ptr_t operandOne = std::make_shared<FloatLiteral>(10.0);
  FloatLiteral::ptr_t operandTwo = std::make_shared<FloatLiteral>(15.0);
  Operator op = Operator(OperatorName::SUB);

  Triple::ptr_t triple = std::make_shared<Triple>(op, operandOne, operandTwo);
  tac.addLine(triple);

  Gas gas = Gas(tac);

  auto expected = R"(.intel_syntax noprefix
.global main

main:
	push ebp
	mov ebp, esp
	sub esp, 4
	push ebx
	push edi
	push esi
	fld DWORD PTR .FC0
	fld DWORD PTR .FC1
	fsubp st(1), st
	fstp DWORD PTR [ebp - 4]
	mov ebx, DWORD PTR [ebp - 4]

.FC0: .float 10.000000
.FC1: .float 15.000000

.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(Gas, GasMulFloatConversion) {
  auto tree = parser::parse(
      R"(
        void main() {
        
        }
        )");

  Tac tac = Tac(tree);
  FloatLiteral::ptr_t operandOne = std::make_shared<FloatLiteral>(10.0);
  FloatLiteral::ptr_t operandTwo = std::make_shared<FloatLiteral>(15.0);
  Operator op = Operator(OperatorName::MUL);

  Triple::ptr_t triple = std::make_shared<Triple>(op, operandOne, operandTwo);
  tac.addLine(triple);

  Gas gas = Gas(tac);

  auto expected = R"(.intel_syntax noprefix
.global main

main:
	push ebp
	mov ebp, esp
	sub esp, 4
	push ebx
	push edi
	push esi
	fld DWORD PTR .FC0
	fld DWORD PTR .FC1
	fmulp st(1), st
	fstp DWORD PTR [ebp - 4]
	mov ebx, DWORD PTR [ebp - 4]

.FC0: .float 10.000000
.FC1: .float 15.000000

.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(Gas, GasDivFloatConversion) {
  auto tree = parser::parse(
      R"(
        void main() {
        
        }
        )");

  Tac tac = Tac(tree);
  FloatLiteral::ptr_t operandOne = std::make_shared<FloatLiteral>(10.0);
  FloatLiteral::ptr_t operandTwo = std::make_shared<FloatLiteral>(15.0);
  Operator op = Operator(OperatorName::DIV);

  Triple::ptr_t triple = std::make_shared<Triple>(op, operandOne, operandTwo);
  tac.addLine(triple);

  Gas gas = Gas(tac);

  auto expected = R"(.intel_syntax noprefix
.global main

main:
	push ebp
	mov ebp, esp
	sub esp, 4
	push ebx
	push edi
	push esi
	fld DWORD PTR .FC0
	fld DWORD PTR .FC1
	fdivp st(1), st
	fstp DWORD PTR [ebp - 4]
	mov ebx, DWORD PTR [ebp - 4]

.FC0: .float 10.000000
.FC1: .float 15.000000

.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}
}
}
