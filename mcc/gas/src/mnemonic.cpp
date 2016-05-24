#include "mcc/gas/mnemonic.h"
#include <iostream>

namespace mcc {
namespace gas {
Mnemonic::Mnemonic(Instruction instruction)
    : mInstruction(instruction), mOperandOne(nullptr), mOperandTwo(nullptr) {}

Mnemonic::Mnemonic(Instruction instruction, Operand::ptr_t opOne)
    : mInstruction(instruction), mOperandOne(opOne), mOperandTwo(nullptr) {}

Mnemonic::Mnemonic(Instruction instruction, Operand::ptr_t opOne,
                   Operand::ptr_t opTwo)
    : mInstruction(instruction), mOperandOne(opOne), mOperandTwo(opTwo) {}

Mnemonic::Mnemonic(std::string labelName)
    : mInstruction(Instruction::NOP),
      mOperandOne(nullptr),
      mOperandTwo(nullptr),
      mIsLabel(true),
      mlabelName(labelName) {}

std::ostream& operator<<(std::ostream& os, const mcc::gas::Mnemonic& mnemonic) {
  if (mnemonic.mIsLabel) {
    return os << "\n" << mnemonic.mlabelName << ":";
  }

  os << "\t" << InstructionName[mnemonic.mInstruction];
  if (mnemonic.mOperandOne) {
    os << " " << (*mnemonic.mOperandOne);
  }

  if (mnemonic.mOperandTwo) {
    os << ", " << (*mnemonic.mOperandTwo);
  }

  return os;
}

std::ostream& operator<<(std::ostream& os,
                         const mcc::gas::Mnemonic::ptr_t mnemonic) {
  os << (*mnemonic);

  return os;
}

Instruction Mnemonic::getInstruction() { return mInstruction; }
}
}
