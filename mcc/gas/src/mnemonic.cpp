#include "mcc/gas/mnemonic.h"
#include <iostream>

namespace mcc {
namespace gas {
Mnemonic::Mnemonic(Instruction instruction)
    : mInstruction(instruction), mOperandOne{nullptr}, mOperandTwo{nullptr} {}

Mnemonic::Mnemonic(Instruction instruction, Operand::ptr_t opOne)
    : mInstruction(instruction), mOperandOne(opOne), mOperandTwo(nullptr) {}

Mnemonic::Mnemonic(Instruction instruction, Operand::ptr_t opOne,
                   Operand::ptr_t opTwo)
    : mInstruction(instruction), mOperandOne(opOne), mOperandTwo(opTwo) {}

std::ostream& operator<<(std::ostream& os, const mcc::gas::Mnemonic& mnemonic) {
  os << InstructionName[mnemonic.mInstruction];
  if (mnemonic.mOperandOne) {
    os << " " << (*mnemonic.mOperandOne);
  }

  if (mnemonic.mOperandTwo) {
    os << ", " << (*mnemonic.mOperandTwo);
  }

  return os;
}
}
}
