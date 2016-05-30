#include "mcc/gas/operand.h"
#include <cassert>

namespace mcc {
namespace gas {

Operand::Operand(Register reg)
    : mType(OperandType::REGISTER),
      mRegister(reg),
      mAddrOffset(0),
      containsTempReg(false) {}
Operand::Operand(bool tempRegister, Register reg)
    : mType(OperandType::REGISTER),
      mRegister(reg),
      mAddrOffset(0),
      containsTempReg(tempRegister) {}
Operand::Operand(int offset)
    : mType(OperandType::ADDRESS),
      mRegister(Register::EBP),
      mAddrOffset(offset),
      containsTempReg(false) {}
Operand::Operand(std::string label)
    : mType(OperandType::LABEL), mLabelName(label), containsTempReg(false) {}
// float constant
Operand::Operand(std::pair<std::string, std::string> floatConstant)
    : mType(OperandType::FLOAT_CONSTANT),
      mLabelName(floatConstant.first),
      containsTempReg(false) {}

std::ostream& operator<<(std::ostream& os, const mcc::gas::Operand& op) {
  switch (op.mType) {
    case OperandType::LABEL:
      return os << op.mLabelName;

    case OperandType::REGISTER:
      return os << RegisterName[op.mRegister];

    case OperandType::ADDRESS:
      os << "DWORD PTR [" << RegisterName[op.mRegister];
      if (op.mAddrOffset > 0) {
        return os << " + " << op.mAddrOffset << "]";
      } else if (op.mAddrOffset < 0) {
        return os << " - " << -op.mAddrOffset << "]";
      } else {
        return os << "]";
      }

    case OperandType::FLOAT_CONSTANT:
      os << "DWORD PTR " << op.mLabelName;
      return os;

    default:
      assert(false && "Unknown assembly operand type");
      return os;
  }
}

bool Operand::isTempRegister() const { return this->containsTempReg; }
bool Operand::isRegister() const {
  return this->mType == OperandType::REGISTER;
}
bool Operand::isAddress() const { return this->mType == OperandType::ADDRESS; }
}
}
