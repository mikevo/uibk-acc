/*
 * File:   mnemonic.h
 * Author: Philipp Stiegernigg
 *
 * Created on May 16, 2016, 12:11 PM
 */

#ifndef MNEMONIC_H
#define MNEMONIC_H
#include "mcc/gas/operand.h"
#include "mcc/gas/x86_instruction_set.h"

namespace mcc {
namespace gas {

class Mnemonic {
 public:
  typedef std::shared_ptr<Mnemonic> ptr_t;

  Mnemonic(Instruction instruction);
  Mnemonic(Instruction instruction, Operand::ptr_t opOne);
  Mnemonic(Instruction instruction, Operand::ptr_t opOne, Operand::ptr_t opTwo);
  Mnemonic(std::string labelName);

  friend std::ostream& operator<<(std::ostream& os,
                                  const mcc::gas::Mnemonic& mnemonic);
  friend std::ostream& operator<<(std::ostream& os,
                                  const mcc::gas::Mnemonic::ptr_t mnemonic);

 private:
  Instruction mInstruction;
  Operand::ptr_t mOperandOne, mOperandTwo;
  bool mIsLabel = false;
  std::string mlabelName;
};
}
}

#endif /* MNEMONIC_H */
