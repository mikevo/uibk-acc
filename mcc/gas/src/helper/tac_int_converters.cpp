/*
 * File:   tac_int_converter.h
 *
 * Created on June 2, 2016, 3:35 PM
 */

#include "mcc/tac/array_access.h"
#include "mcc/tac/helper/ast_converters.h"

#include "mcc/gas/gas.h"
#include "mcc/gas/helper/tac_converters.h"
#include "mcc/gas/helper/tac_int_converters.h"

extern bool resultAvailable;
extern Label::ptr_t currentFunction;
extern OperatorName lastOperator;

extern std::vector<std::tuple<Label::ptr_t, Array::ptr_t,
                              mcc::gas::Operand::ptr_t>> declaredArrays;

namespace mcc {
namespace gas {
namespace helper {
void convertIntArithmetic(Gas *gas, Triple::ptr_t triple) {
  Operand::ptr_t reg0;
  Operand::ptr_t reg1;
  Operand::ptr_t tmp;

  auto operatorName = triple->getOperator().getName();

  reg0 = gas->loadOperand(currentFunction, triple->getArg1());

  tmp = gas->getRegisterManager()->getTmpRegister();
  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::MOV, tmp, reg0));

  if (operatorName == OperatorName::DIV) {
    gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::CDQ));
  }

  reg1 = gas->loadOperand(currentFunction, triple->getArg2());

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

  gas->storeOperandFromRegister(currentFunction, triple->getTargetVariable(),
                                tmp);
}

void convertIntAssign(Gas *gas, Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    auto arg1 = triple->getArg1();
    Operand::ptr_t arg1Op;
    if (tac::helper::isType<Variable>(arg1)) {
      auto destVar = std::static_pointer_cast<Variable>(arg1);
      arg1Op = gas->getRegisterManager()->getLocationForVariable(
          currentFunction, destVar);
    } else if (tac::helper::isType<ArrayAccess>(arg1)) {
      auto arrAcc = std::static_pointer_cast<ArrayAccess>(arg1);
      auto arr = arrAcc->getArray();

      if (lookupDeclaredArray(currentFunction, arr) == declaredArrays.end()) {
        assert(false && "Use of array before declaration!");
      }

      arg1Op = gas->loadOperand(currentFunction, arrAcc);
    }

    if (triple->containsArg2()) {
      auto arg2 = triple->getArg2();
      if (tac::helper::isType<ArrayAccess>(arg2)) {
        auto arrAcc = std::static_pointer_cast<ArrayAccess>(arg2);

        bool loadToTemp = arg1Op->isAddress();
        Operand::ptr_t tmp;
        if (loadToTemp) {
          tmp = gas->getRegisterManager()->getTmpRegister();
        }
        auto arg2Op = gas->loadOperand(currentFunction, arrAcc, tmp);
        gas->addMnemonic(
            std::make_shared<Mnemonic>(Instruction::MOV, arg1Op, arg2Op));
      } else {
        auto arg2Op = gas->loadOperand(currentFunction, arg2);

        if (arg2Op->isAddress() && arg1Op->isAddress()) {
          auto tmp = gas->getRegisterManager()->getTmpRegister();
          gas->addMnemonic(
              std::make_shared<Mnemonic>(Instruction::MOV, tmp, arg2Op));
          arg2Op = tmp;
        }

        gas->addMnemonic(
            std::make_shared<Mnemonic>(Instruction::MOV, arg1Op, arg2Op));
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

  reg0 = gas->loadOperand(currentFunction, triple->getArg1());
  reg1 = gas->loadOperand(currentFunction, triple->getArg2());

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