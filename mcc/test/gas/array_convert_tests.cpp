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

TEST(ArrayConvert, ArrayStackOffset) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto tree = parser::parse(
      R"(
           void foo(int arg) {
             int a[3];
             int b[5];
           }
  
           void main() {
             int a[10];
             int i;
             int b[20];
           }
        )");

  Tac tac = Tac(tree);
  Gas gas = Gas(tac);

  auto regMan = gas.getRegisterManager();

  Label::ptr_t currentFunction;
  int varCounter = 0;
  signed expectedOffsets[] = {-4, -8, -4, -8};
  for (auto codeLine : tac.codeLines) {
    if (helper::isType<Label>(codeLine)) {
      auto label = std::static_pointer_cast<Label>(codeLine);
      if (label->isFunctionEntry()) {
        currentFunction = label;
      }
    }

    for (auto arrDecl : *(tac.getArrayDeclVec())) {
      auto arrDeclLine = arrDecl.second;
      if (arrDeclLine == nullptr || arrDeclLine == codeLine) {
        auto arr = arrDecl.first;
        auto arrOffset = regMan->lookupArrayStackOffset(currentFunction, arr);

        EXPECT_EQ(expectedOffsets[varCounter++], arrOffset);
      }
    }
  }
}

TEST(ArrayConvert, ArrayDeclaration) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto tree = parser::parse(
      R"(
           void main() {
             int a[10];
           }
        )");

  auto tac = Tac(tree);
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
	mov DWORD PTR [ebp - 4], esp
	mov eax, 10
	imul eax, 4
	sub esp, eax
	mov ebx, 10
	imul ebx, 4
	add esp, ebx
	pop esi
	pop edi
	pop ebx
	add esp, 4
	mov esp, ebp
	pop ebp
	ret


.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(ArrayConvert, ArrayAccess) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto tree = parser::parse(
      R"(
           void main() {
             int a[1];
             a[0];
           }
        )");

  auto tac = Tac(tree);
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
	mov DWORD PTR [ebp - 4], esp
	mov eax, 1
	imul eax, 4
	sub esp, eax
	mov ebx, 1
	imul ebx, 4
	add esp, ebx
	pop esi
	pop edi
	pop ebx
	add esp, 4
	mov esp, ebp
	pop ebp
	ret


.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(ArrayConvert, ArrayArithmetic) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto tree = parser::parse(
      R"(
        void main() {
          int a[1];
          int b[1];
          int c[1];
  
          a[0] = b[0] + c[0];
          a[0] = b[0] - c[0];
          a[0] = b[0] * c[0];
          a[0] = b[0] / c[0];
        }
        )");

  Tac tac = Tac(tree);

  Gas gas = Gas(tac);
  auto expected = R"(.intel_syntax noprefix
.global main

main:
	push ebp
	mov ebp, esp
	sub esp, 44
	push ebx
	push edi
	push esi
	mov DWORD PTR [ebp - 4], esp
	mov eax, 1
	imul eax, 4
	sub esp, eax
	mov DWORD PTR [ebp - 8], esp
	mov ebx, 1
	imul ebx, 4
	sub esp, ebx
	mov DWORD PTR [ebp - 12], esp
	mov eax, 1
	imul eax, 4
	sub esp, eax
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 8]
	neg ebx
	mov eax, DWORD PTR [ebx]
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 12]
	neg ebx
	add eax, DWORD PTR [ebx]
	mov ecx, eax
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 4]
	neg eax
	mov DWORD PTR [eax], ecx
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 8]
	neg ebx
	mov eax, DWORD PTR [ebx]
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 12]
	neg ebx
	sub eax, DWORD PTR [ebx]
	mov ecx, eax
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 4]
	neg eax
	mov DWORD PTR [eax], ecx
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 8]
	neg ebx
	mov eax, DWORD PTR [ebx]
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 12]
	neg ebx
	imul eax, DWORD PTR [ebx]
	mov ecx, eax
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 4]
	neg eax
	mov DWORD PTR [eax], ecx
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 8]
	neg ebx
	mov eax, DWORD PTR [ebx]
	cdq
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 12]
	neg ebx
	idiv DWORD PTR [ebx]
	mov ecx, eax
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 4]
	neg eax
	mov DWORD PTR [eax], ecx
	mov ebx, 1
	imul ebx, 4
	add esp, ebx
	mov ebx, 1
	imul ebx, 4
	add esp, ebx
	mov ebx, 1
	imul ebx, 4
	add esp, ebx
	pop esi
	pop edi
	pop ebx
	add esp, 44
	mov esp, ebp
	pop ebp
	ret


.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(ArrayConvert, ArrayDeclarationVariableLength) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto tree = parser::parse(
      R"(
           void main() {
             int i;
             i = 10;
             int a[i];
           }
        )");

  auto tac = Tac(tree);
  Gas gas = Gas(tac);

  auto expected = R"(.intel_syntax noprefix
.global main

main:
	push ebp
	mov ebp, esp
	sub esp, 8
	push ebx
	push edi
	push esi
	mov ecx, 10
	mov DWORD PTR [ebp - 8], esp
	mov eax, ecx
	imul eax, 4
	sub esp, eax
	mov ebx, ecx
	imul ebx, 4
	add esp, ebx
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

TEST(ArrayConvert, VLALoop) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto tree = parser::parse(
      R"(
           void main() {
             int i;
             i = 0;
             while(i < 10) {
               int a[i];
               
               i = i + 1;
             }
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

main:
	push ebp
	mov ebp, esp
	sub esp, 12
	push ebx
	push edi
	push esi
	mov edx, 0

)" + *curLabel++ + R"(:
	cmp edx, 10
	jge )" + *curLabel++ +
                  R"(
	mov DWORD PTR [ebp - 8], esp
	mov eax, edx
	imul eax, 4
	sub esp, eax
	mov ebx, edx
	add ebx, 1
	mov ecx, ebx
	mov eax, edx
	imul eax, 4
	add esp, eax
	mov edx, ecx
	jmp )" + *curLabel++ +
                  R"(

)" + *curLabel++ + R"(:
	pop esi
	pop edi
	pop ebx
	add esp, 12
	mov esp, ebp
	pop ebp
	ret


.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(ArrayConvert, ArrayAssign) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto tree = parser::parse(
      R"(
        void main() {
          int a[1];
          int b[1];
          int i;
          i = 41;
  
          a[0] = 42;
          a[0] = i;
          a[0] = b[0];
        }
        )");

  Tac tac = Tac(tree);

  Gas gas = Gas(tac);
  auto expected = R"(.intel_syntax noprefix
.global main

main:
	push ebp
	mov ebp, esp
	sub esp, 24
	push ebx
	push edi
	push esi
	mov DWORD PTR [ebp - 4], esp
	mov eax, 1
	imul eax, 4
	sub esp, eax
	mov DWORD PTR [ebp - 8], esp
	mov ebx, 1
	imul ebx, 4
	sub esp, ebx
	mov ecx, 41
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 4]
	neg eax
	mov DWORD PTR [eax], 42
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 4]
	neg ebx
	mov DWORD PTR [ebx], ecx
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 4]
	neg eax
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 8]
	neg ebx
	mov ebx, DWORD PTR [ebx]
	mov DWORD PTR [eax], ebx
	mov ebx, 1
	imul ebx, 4
	add esp, ebx
	mov ebx, 1
	imul ebx, 4
	add esp, ebx
	pop esi
	pop edi
	pop ebx
	add esp, 24
	mov esp, ebp
	pop ebp
	ret


.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(ArrayConvert, ArrayLogicOperator) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto tree = parser::parse(
      R"(
        void main() {
          int a[1];
          int b[1];
  
          if(a[0] < b[0]) {
          } else if(a[0] <= b[0]) {
          } else if(a[0] > b[0]) {
          } else if(a[0] >= b[0]) {
          } else if(a[0] == b[0]) {
          } else if(a[0] != b[0]) {
          }
        }
        )");

  Tac tac = Tac(tree);

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

main:
	push ebp
	mov ebp, esp
	sub esp, 8
	push ebx
	push edi
	push esi
	mov DWORD PTR [ebp - 4], esp
	mov eax, 1
	imul eax, 4
	sub esp, eax
	mov DWORD PTR [ebp - 8], esp
	mov ebx, 1
	imul ebx, 4
	sub esp, ebx
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 4]
	neg eax
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 8]
	neg ebx
	mov eax, DWORD PTR [eax]
	cmp eax, DWORD PTR [ebx]
	jge )" + *curLabel++ +
                  R"(
	jmp )" + *curLabel++ +
                  R"(

)" + *curLabel++ + R"(:
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 4]
	neg ebx
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 8]
	neg eax
	mov ebx, DWORD PTR [ebx]
	cmp ebx, DWORD PTR [eax]
	jg )" + *curLabel++ +
                  R"(
	jmp )" + *curLabel++ +
                  R"(

)" + *curLabel++ + R"(:
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 4]
	neg eax
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 8]
	neg ebx
	mov eax, DWORD PTR [eax]
	cmp eax, DWORD PTR [ebx]
	jle )" + *curLabel++ +
                  R"(
	jmp )" + *curLabel++ +
                  R"(

)" + *curLabel++ + R"(:
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 4]
	neg ebx
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 8]
	neg eax
	mov ebx, DWORD PTR [ebx]
	cmp ebx, DWORD PTR [eax]
	jl )" + *curLabel++ +
                  R"(
	jmp )" + *curLabel++ +
                  R"(

)" + *curLabel++ + R"(:
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 4]
	neg eax
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 8]
	neg ebx
	mov eax, DWORD PTR [eax]
	cmp eax, DWORD PTR [ebx]
	jne )" + *curLabel++ +
                  R"(
	jmp )" + *curLabel++ +
                  R"(

)" + *curLabel++ + R"(:
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 4]
	neg ebx
	mov eax, 0
	imul eax, 4
	sub eax, DWORD PTR [ebp - 8]
	neg eax
	mov ebx, DWORD PTR [ebx]
	cmp ebx, DWORD PTR [eax]
	je )" + *curLabel++ +
                  R"(

)" + *curLabel++ + R"(:

)" + *curLabel++ + R"(:

)" + *curLabel++ + R"(:

)" + *curLabel++ + R"(:

)" + *curLabel++ + R"(:

)" + *curLabel++ + R"(:
	mov ebx, 1
	imul ebx, 4
	add esp, ebx
	mov ebx, 1
	imul ebx, 4
	add esp, ebx
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

TEST(ArrayConvert, ArrayFunctionCall) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto tree = parser::parse(
      R"(
        void foo(int arg) {}
  
        void main() {
          int a[1];
  
          foo(a[0]);
        }
        )");

  Tac tac = Tac(tree);

  Gas gas = Gas(tac);

  auto expected = R"(.intel_syntax noprefix
.global main

foo:
	push ebp
	mov ebp, esp
	push ebx
	push edi
	push esi
	mov ecx, DWORD PTR [ebp + 8]
	pop esi
	pop edi
	pop ebx
	mov esp, ebp
	pop ebp
	ret

main:
	push ebp
	mov ebp, esp
	sub esp, 4
	push ebx
	push edi
	push esi
	mov DWORD PTR [ebp - 4], esp
	mov eax, 1
	imul eax, 4
	sub esp, eax
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 4]
	neg ebx
	push ecx
	push edx
	push DWORD PTR [ebx]
	call foo
	add esp, 4
	pop edx
	pop ecx
	mov ebx, 1
	imul ebx, 4
	add esp, ebx
	pop esi
	pop edi
	pop ebx
	add esp, 4
	mov esp, ebp
	pop ebp
	ret


.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}

TEST(ArrayConvert, ArrayReturn) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto tree = parser::parse(
      R"(
        int main() {
          int a[1];
  
          return a[0];
        }
        )");

  Tac tac = Tac(tree);

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
	mov DWORD PTR [ebp - 4], esp
	mov eax, 1
	imul eax, 4
	sub esp, eax
	mov ebx, 0
	imul ebx, 4
	sub ebx, DWORD PTR [ebp - 4]
	neg ebx
	mov eax, DWORD PTR [ebx]
	mov ebx, 1
	imul ebx, 4
	add esp, ebx
	pop esi
	pop edi
	pop ebx
	add esp, 4
	mov esp, ebp
	pop ebp
	ret


.att_syntax noprefix
)";

  EXPECT_EQ(expected, gas.toString());
}
}
}
