/*
 * File:   tac_float_converter.h
 *
 * Created on June 2, 2016, 3:35 PM
 */

#include "mcc/tac/helper/ast_converters.h"

#include "mcc/gas/gas.h"
#include "mcc/gas/helper/tac_converters.h"

extern bool resultAvailable;
extern Label::ptr_t currentFunction;
extern OperatorName lastOperator;

extern std::vector<std::pair<Array::ptr_t, unsigned>> definedArrays;

namespace mcc {
namespace gas {
namespace helper {
void convertFloatArithmetic(Gas *gas, Triple::ptr_t triple) {
  gas->pushOperandToFloatRegister(currentFunction, triple->getArg1());
  gas->pushOperandToFloatRegister(currentFunction, triple->getArg2());

  // TODO calling this constructor creates LABEL operands, maybe this leads to
  // problems later
  Operand::ptr_t op1 = std::make_shared<Operand>("st(1)");
  Operand::ptr_t op2 = std::make_shared<Operand>("st");
  auto operatorName = triple->getOperator().getName();
  switch (operatorName) {
    case OperatorName::ADD:
      gas->addMnemonic(
          std::make_shared<Mnemonic>(Instruction::FADDP, op1, op2));
      break;
    case OperatorName::SUB:
      gas->addMnemonic(
          std::make_shared<Mnemonic>(Instruction::FSUBP, op1, op2));
      break;
    case OperatorName::MUL:
      gas->addMnemonic(
          std::make_shared<Mnemonic>(Instruction::FMULP, op1, op2));
      break;
    case OperatorName::DIV:
      gas->addMnemonic(
          std::make_shared<Mnemonic>(Instruction::FDIVP, op1, op2));
      break;
    default:
      assert(false && "unknown operation");
  }

  if (triple->containsTargetVar()) {
    gas->storeVariableFromFloatRegister(currentFunction,
                                        triple->getTargetVariable());
  }
}

void convertFloatAssign(Gas *gas, Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    auto op = triple->getArg1();
    if (tac::helper::isType<Variable>(op)) {
      auto destVar = std::static_pointer_cast<Variable>(op);

      if (triple->containsArg2()) {
        auto reg =
            gas->loadOperandToRegister(currentFunction, triple->getArg2());
        gas->storeStackVariableFromRegister(currentFunction, destVar, reg);
      }
    }
  }
}

void convertFloatJumpFalse(Gas *gas, Triple::ptr_t triple) {
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
        case OperatorName::GE:
        case OperatorName::LE:
          gas->addMnemonic(
              std::make_shared<Mnemonic>(Instruction::JB, asmLabel));
          break;
        case OperatorName::GT:
        case OperatorName::LT:
          gas->addMnemonic(
              std::make_shared<Mnemonic>(Instruction::JBE, asmLabel));
          break;
        default:
          assert(false && "Unknown compare for jump false");
      }
    }
  }
}

void convertFloatLogicOperator(Gas *gas, Triple::ptr_t triple) {
  // implementation little bit adapted from godbolt
  auto operatorName = triple->getOperator().getName();

  // TODO calling this constructor creates LABEL operands, maybe this leads to
  // problems later
  Operand::ptr_t st = std::make_shared<Operand>("st");
  Operand::ptr_t st0 = std::make_shared<Operand>("st(0)");
  Operand::ptr_t st1 = std::make_shared<Operand>("st(1)");

  switch (operatorName) {
    case OperatorName::EQ:
    case OperatorName::NE:
    case OperatorName::GT:
    case OperatorName::GE:
      gas->pushOperandToFloatRegister(currentFunction, triple->getArg1());
      gas->pushOperandToFloatRegister(currentFunction, triple->getArg2());
      break;
    case OperatorName::LT:
    case OperatorName::LE:
      gas->pushOperandToFloatRegister(currentFunction, triple->getArg2());
      gas->pushOperandToFloatRegister(currentFunction, triple->getArg1());
      break;
    default:
      assert(false && "Unknown operator name in float logic conversion!");
  }

  switch (operatorName) {
    case OperatorName::LT:
    case OperatorName::LE:
    case OperatorName::GT:
    case OperatorName::GE:
      gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::FXCH, st1));
      break;
    default:
      // ignore
      break;
  }

  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::FUCOMIP, st, st1));
  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::FSTP, st0));

  // jump handled later
}

void convertFloatMinus(Gas *gas, Triple::ptr_t triple) {
  gas->pushOperandToFloatRegister(currentFunction, triple->getArg1());
  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::FCHS));

  if (triple->containsTargetVar()) {
    gas->storeVariableFromFloatRegister(currentFunction,
                                        triple->getTargetVariable());
  }
}
}
}
}