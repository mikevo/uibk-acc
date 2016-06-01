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

// resolve circular dependency
namespace mcc {
namespace gas {
class RegisterManager;
}
}

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

  std::shared_ptr<Operand> loadSpilledVariable(Variable::ptr_t var,
                                               Operand::ptr_t reg);
  unsigned lookupVariableStackOffset(Variable::ptr_t var);

  std::shared_ptr<RegisterManager> getRegisterManager();

 private:
  //  std::shared_ptr<function_map_type> functionMap;
  OperatorName lastOperator;
  std::shared_ptr<constant_floats_map_type> constantFloatsMap;
  Label::ptr_t currentFunction;

  std::vector<Mnemonic::ptr_t> asmInstructions;

  std::shared_ptr<RegisterManager> registerManager;

  mcc::tac::Tac& tac;

  void convertTac(Tac& tac);

  std::shared_ptr<Operand> loadOperandToRegister(mcc::tac::Operand::ptr_t op);
  void loadVariableToRegister(Variable::ptr_t var, Operand::ptr_t);
  void storeVariableFromRegister(Variable::ptr_t var, Operand::ptr_t reg);
  void storeStackVariableFromRegister(Variable::ptr_t var, Operand::ptr_t reg);
  void storeVariableFromFloatRegister(Variable::ptr_t var);
  void pushOperandToFloatRegister(mcc::tac::Operand::ptr_t op);
  Operand::ptr_t getAsmVar(Variable::ptr_t var);
  std::pair<std::string, std::string> createFloatConstant(std::string value);

  void storeRegisters(std::initializer_list<Register> list);
  void storeRegisters(std::initializer_list<Register> list, unsigned pos);
  void restoreRegisters(std::initializer_list<Register> list);
  void restoreRegisters(std::initializer_list<Register> list, unsigned pos);
  void prepareCall(Label::ptr_t label);
  void cleanUpCall(Label::ptr_t label);
  void createFunctionProlog(Label::ptr_t label);
  void createFunctionEpilog(Label::ptr_t label);

  void convertLabel(Triple::ptr_t triple);
  void convertReturn(Triple::ptr_t triple);
  void convertCall(Triple::ptr_t triple);
  void convertPush(Triple::ptr_t triple);
  void convertAssign(Triple::ptr_t triple);

  void convertArithmetic(Triple::ptr_t triple);
  void convertIntArithmetic(Triple::ptr_t triple);
  void convertFloatArithmetic(Triple::ptr_t triple);

  void convertLogicOperator(Triple::ptr_t triple);
  void convertIntLogicOperator(Triple::ptr_t triple);
  void convertFloatLogicOperator(Triple::ptr_t triple);

  void convertJump(Triple::ptr_t triple);

  void convertJumpFalse(Triple::ptr_t triple);
  void convertIntJumpFalse(Triple::ptr_t triple);
  void convertFloatJumpFalse(Triple::ptr_t triple);

  void convertMinus(Triple::ptr_t triple);
  void convertNot(Triple::ptr_t triple);
  void convertUnary(Triple::ptr_t triple, Instruction i);
  void convertFloatMinus(Triple::ptr_t triple);
};
}
}

#endif /* GAS_H */
