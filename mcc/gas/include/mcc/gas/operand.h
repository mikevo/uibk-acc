/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   operand.h
 * Author: Philipp Stiegernigg
 *
 * Created on May 15, 2016, 11:31 PM
 */

#ifndef OPERAND_H
#define OPERAND_H
#include "mcc/gas/x86_instruction_set.h"
#include <iostream>
#include <memory>

namespace mcc {
namespace gas {

class Operand {
 public:
  typedef std::shared_ptr<Operand> ptr_t;

  Operand(Register reg);
  Operand(bool tempRegister, Register reg);
  Operand(Operand::ptr_t reg, signed offset);
  Operand(signed offset);
  Operand(std::string label);
  Operand(std::pair<std::string, std::string> floatConstant);

  friend std::ostream& operator<<(std::ostream& os,
                                  const mcc::gas::Operand& op);

  bool isTempRegister() const;
  bool isRegister() const;
  bool isAddress() const;
  bool isFloatConstant() const;

 private:
  OperandType mType;
  Register mRegister;
  signed mAddrOffset;
  std::string mLabelName;
  bool containsTempReg;
  bool mIsFloatConstant;
};
}
}
#endif /* OPERAND_H */
