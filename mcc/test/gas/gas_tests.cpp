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
