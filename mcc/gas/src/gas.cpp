#include "mcc/gas/gas.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <memory>
#include <ostream>
#include <typeinfo>

#include "mcc/gas/x86_instruction_set.h"
#include "mcc/tac/float_literal.h"
#include "mcc/tac/helper/ast_converters.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/type.h"
#include "mcc/tac/variable.h"

#include "mcc/gas/helper/tac_converters.h"

namespace mcc {
namespace gas {

Gas::Gas(Tac& tac) {
  this->constantFloatsMap = std::make_shared<constant_floats_map_type>();
  this->registerManager = std::make_shared<RegisterManager>(tac);

  helper::convertTac(this, tac);
}

std::shared_ptr<Operand> Gas::loadOperandToRegister(
    Label::ptr_t functionLabel, mcc::tac::Operand::ptr_t op) {
  if (tac::helper::isType<Variable>(op)) {
    auto variableOp = std::static_pointer_cast<Variable>(op);
    return this->registerManager->getLocationForVariable(functionLabel,
                                                         variableOp);
  } else if (tac::helper::isType<Triple>(op)) {
    auto triple = std::static_pointer_cast<Triple>(op);
    auto variableOp = triple->getTargetVariable();
    return this->registerManager->getLocationForVariable(functionLabel,
                                                         variableOp);
  } else {
    // constant values
    if (op->getType() == Type::FLOAT) {
      auto floatConstant = createFloatConstant(op->getValue());

      return std::make_shared<Operand>(floatConstant);
    } else {
      return std::make_shared<Operand>(op->getValue());
    }
  }
}

std::shared_ptr<Operand> Gas::loadSpilledVariable(Label::ptr_t functionLabel,
                                                  Variable::ptr_t var,
                                                  Operand::ptr_t reg) {
  signed varOffset =
      this->registerManager->lookupVariableStackOffset(functionLabel, var);
  // register of operand is ignored
  auto asmVar = std::make_shared<Operand>(varOffset);

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, reg, asmVar));

  return asmVar;
}

void Gas::loadVariableToRegister(Label::ptr_t functionLabel,
                                 Variable::ptr_t var, Operand::ptr_t reg) {
  signed varOffset =
      this->registerManager->lookupVariableStackOffset(functionLabel, var);
  auto asmVar = std::make_shared<Operand>(varOffset);

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, reg, asmVar));
}

void Gas::storeVariableFromRegister(Label::ptr_t functionLabel,
                                    Variable::ptr_t var, Operand::ptr_t reg) {
  auto asmVar =
      this->registerManager->getLocationForVariable(functionLabel, var);

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, asmVar, reg));
}

void Gas::storeStackVariableFromRegister(Label::ptr_t functionLabel,
                                         Variable::ptr_t var,
                                         Operand::ptr_t reg) {
  auto stackVar = getAsmVar(functionLabel, var);

  auto tmp = this->registerManager->getTmpRegister();
  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, tmp, reg));
  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, stackVar, tmp));
}

void Gas::storeVariableFromFloatRegister(Label::ptr_t functionLabel,
                                         Variable::ptr_t var) {
  auto stackVar = getAsmVar(functionLabel, var);
  auto asmVar =
      this->registerManager->getLocationForVariable(functionLabel, var);

  if (asmVar->isAddress()) {
    auto tmp = this->registerManager->getTmpRegister();
    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::FSTP, stackVar));
    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::MOV, tmp, stackVar));
    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::MOV, asmVar, tmp));

  } else {
    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::FSTP, stackVar));

    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::MOV, asmVar, stackVar));
  }
}

void Gas::storeRegisters(std::initializer_list<Register> list) {
  this->storeRegisters(list, asmInstructions.size());
}

void Gas::storeRegisters(std::initializer_list<Register> list, unsigned pos) {
  auto it = asmInstructions.begin() + pos;
  std::vector<Mnemonic::ptr_t> ops;

  for (auto reg : list) {
    auto regOp = std::make_shared<Operand>(reg);
    ops.push_back(std::make_shared<Mnemonic>(Instruction::PUSH, regOp));
  }

  asmInstructions.insert(it, ops.begin(), ops.end());
}

void Gas::restoreRegisters(std::initializer_list<Register> list) {
  this->restoreRegisters(list, asmInstructions.size());
}

void Gas::restoreRegisters(std::initializer_list<Register> list, unsigned pos) {
  auto it = asmInstructions.begin() + pos;
  std::vector<Mnemonic::ptr_t> ops;

  for (auto reg : list) {
    auto regOp = std::make_shared<Operand>(reg);
    ops.push_back(std::make_shared<Mnemonic>(Instruction::POP, regOp));
  }

  asmInstructions.insert(it, ops.begin(), ops.end());
}

void Gas::pushOperandToFloatRegister(Label::ptr_t functionLabel,
                                     mcc::tac::Operand::ptr_t op) {
  assert(op->getType() == Type::FLOAT && "Variable is not of type float!");

  Operand::ptr_t asmVar;
  if (tac::helper::isType<Variable>(op)) {
    auto var = std::static_pointer_cast<Variable>(op);
    asmVar = getAsmVar(functionLabel, var);
  } else if (tac::helper::isType<Triple>(op)) {
    auto triple = std::static_pointer_cast<Triple>(op);
    auto var = triple->getTargetVariable();
    asmVar = getAsmVar(functionLabel, var);
  } else {
    // constant values
    auto floatConstant = createFloatConstant(op->getValue());
    asmVar = std::make_shared<Operand>(floatConstant);
  }

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::FLD, asmVar));
}

std::pair<std::string, std::string> Gas::createFloatConstant(
    std::string value) {
  auto constName = ".FC" + std::to_string(constantFloatsMap->size());
  auto floatConstant = std::make_pair(constName, value);
  constantFloatsMap->insert(floatConstant);

  return floatConstant;
}

Operand::ptr_t Gas::getAsmVar(Label::ptr_t functionLabel, Variable::ptr_t var) {
  signed varOffset =
      this->registerManager->lookupVariableStackOffset(functionLabel, var);
  auto asmVar = std::make_shared<Operand>(varOffset);

  return asmVar;
}

void Gas::addMnemonic(Mnemonic::ptr_t mnemonic) {
  asmInstructions.push_back(mnemonic);
}

std::shared_ptr<RegisterManager> Gas::getRegisterManager() {
  return registerManager;
}

std::vector<Mnemonic::ptr_t>& Gas::getAsmInstructions() {
  return asmInstructions;
}

std::string Gas::toString() const {
  std::ostringstream stream;

  stream << ".intel_syntax noprefix" << std::endl;
  stream << ".global main" << std::endl;

  for (auto mnemonic : asmInstructions) {
    stream << mnemonic << std::endl;
  }

  stream << std::endl;
  for (auto floatConstant : *constantFloatsMap.get()) {
    auto varName = floatConstant.first;
    auto value = floatConstant.second;
    stream << varName << ": .float " << value << std::endl;
  }

  stream << std::endl << ".att_syntax noprefix" << std::endl;

  return stream.str();
}

std::ostream& operator<<(std::ostream& os, const mcc::gas::Gas& gas) {
  os << gas.toString();

  return os;
}
}
}
