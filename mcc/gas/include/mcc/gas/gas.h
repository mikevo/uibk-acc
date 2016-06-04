/*
 * File:   gas.h
 *
 * Created on May 12, 2016, 11:55 PM
 */

#ifndef GAS_H
#define GAS_H

#include <memory>
#include <string>
#include <vector>

#include "mcc/gas/register_manager.h"
#include "mcc/tac/tac.h"
#include "mnemonic.h"

using namespace mcc::tac;

namespace mcc {
namespace gas {

// typedef std::map<std::string, Label::ptr_t> function_map_type;
// map: variableName -> value
typedef std::map<std::string, std::string> constant_floats_map_type;

class Gas {
 public:
  Gas(Tac& tac);
  std::vector<Mnemonic::ptr_t>& getAsmInstructions();
  std::string toString() const;

  friend std::ostream& operator<<(std::ostream& os, const mcc::gas::Gas& gas);

  std::shared_ptr<RegisterManager> getRegisterManager();

  void addMnemonic(Mnemonic::ptr_t mnemonic);

  Operand::ptr_t loadOperand(Label::ptr_t functionLabel,
                             mcc::tac::Operand::ptr_t op);
  Operand::ptr_t loadOperandToRegister(Label::ptr_t functionLabel,
                                       mcc::tac::Operand::ptr_t op,
                                       Operand::ptr_t reg);
  Operand::ptr_t storeOperandFromRegister(Label::ptr_t functionLabel,
                                          mcc::tac::Operand::ptr_t op,
                                          Operand::ptr_t reg);

  void storeVariableFromFloatRegister(Label::ptr_t functionLabel,
                                      Variable::ptr_t var);
  void pushOperandToFloatRegister(Label::ptr_t functionLabel,
                                  mcc::tac::Operand::ptr_t op);
  Operand::ptr_t getAsmVar(Label::ptr_t functionLabel, Variable::ptr_t var);
  std::pair<std::string, std::string> createFloatConstant(std::string value);

  void storeRegisters(std::initializer_list<Register> list);
  void storeRegisters(std::initializer_list<Register> list, unsigned pos);
  void restoreRegisters(std::initializer_list<Register> list);
  void restoreRegisters(std::initializer_list<Register> list, unsigned pos);

 private:
  //  std::shared_ptr<function_map_type> functionMap;
  std::shared_ptr<constant_floats_map_type> constantFloatsMap;
  std::vector<Mnemonic::ptr_t> asmInstructions;
  std::shared_ptr<RegisterManager> registerManager;
};
}
}

#endif /* GAS_H */
