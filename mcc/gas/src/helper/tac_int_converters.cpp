/*
 * File:   tac_int_converter.h
 *
 * Created on June 2, 2016, 3:35 PM
 */

#include "mcc/tac/helper/ast_converters.h"

#include "mcc/gas/gas.h"
#include "mcc/gas/helper/tac_converters.h"

extern bool resultAvailable;
extern Label::ptr_t currentFunction;
extern OperatorName lastOperator;

namespace mcc {
namespace gas {
namespace helper {
void convertIntArithmetic(Gas *gas, Triple::ptr_t triple) {
  Operand::ptr_t reg0;
  Operand::ptr_t reg1;
  Operand::ptr_t tmp;

  auto operatorName = triple->getOperator().getName();

  reg0 = gas->loadOperandToRegister(currentFunction, triple->getArg1());

  tmp = gas->getRegisterManager()->getTmpRegister();
  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::MOV, tmp, reg0));

  if (operatorName == OperatorName::DIV) {
    gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::CDQ));
  }

  reg1 = gas->loadOperandToRegister(currentFunction, triple->getArg2());

  switch (operatorName) {
    case OperatorName::ADD:
      gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::ADD, tmp, reg1));
      break;
    case OperatorName::SUB:
      gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::SUB, tmp, reg1));
      break;
    case OperatorName::MUL:
      gas->addMnemonic(
          std::make_shared<Mnemonic>(Instruction::IMUL, tmp, reg1));
      break;
    case OperatorName::DIV:
      gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::IDIV, reg1));
      break;
    default:
      assert(false && "unknown operation");
  }

  gas->storeVariableFromRegister(currentFunction, triple->getTargetVariable(),
                                 tmp);
}

void convertIntAssign(Gas *gas, Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    auto op = triple->getArg1();
    if (tac::helper::isType<Variable>(op)) {
      auto destVar = std::static_pointer_cast<Variable>(op);

      if (triple->containsArg2()) {
        auto reg =
            gas->loadOperandToRegister(currentFunction, triple->getArg2());
        gas->storeVariableFromRegister(currentFunction, destVar, reg);
      }
    }
  }
}

void convertIntJumpFalse(Gas *gas, Triple::ptr_t triple) {
  if (triple->containsArg2()) {
    auto operand = triple->getArg2();
    if (tac::helper::isType<Label>(operand)) {
      auto label = std::static_pointer_cast<Label>(operand);
      auto asmLabel = std::make_shared<Operand>(label->getName());
      switch (lastOperator) {
        case OperatorName::EQ:
          gas->addMnemonic(
              std::make_shared<Mnemonic>(Instruction::JNE, asmLabel));
          break;
        case OperatorName::NE:
          gas->addMnemonic(
              std::make_shared<Mnemonic>(Instruction::JE, asmLabel));
          break;
        case OperatorName::LE:
          gas->addMnemonic(
              std::make_shared<Mnemonic>(Instruction::JG, asmLabel));
          break;
        case OperatorName::GE:
          gas->addMnemonic(
              std::make_shared<Mnemonic>(Instruction::JL, asmLabel));
          break;
        case OperatorName::LT:
          gas->addMnemonic(
              std::make_shared<Mnemonic>(Instruction::JGE, asmLabel));
          break;
        case OperatorName::GT:
          gas->addMnemonic(
              std::make_shared<Mnemonic>(Instruction::JLE, asmLabel));
          break;
        default:
          assert(false && "Unknown compare for jump false");
      }
    }
  }
}

void convertIntLogicOperator(Gas *gas, Triple::ptr_t triple) {
  Operand::ptr_t reg0;
  Operand::ptr_t reg1;
  Operand::ptr_t tmp;

  reg0 = gas->loadOperandToRegister(currentFunction, triple->getArg1());
  reg1 = gas->loadOperandToRegister(currentFunction, triple->getArg2());

  if (reg0->isAddress()) {
    tmp = gas->getRegisterManager()->getTmpRegister();
    gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::MOV, tmp, reg0));
  } else {
    tmp = reg0;
  }

  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::CMP, tmp, reg1));
}
}
}
}