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
  Operand(Register reg, int offset);
  Operand(std::string label);

  friend std::ostream& operator<<(std::ostream& os,
                                  const mcc::gas::Operand& op);

 private:
  OperandType mType;
  Register mRegister;
  int mAddrOffset;
  std::string mLabelName;
};
}
}
#endif /* OPERAND_H */
