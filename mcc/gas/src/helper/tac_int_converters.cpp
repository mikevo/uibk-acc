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

extern std::vector<std::pair<Array::ptr_t, unsigned>> definedArrays;

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
    auto arg1 = triple->getArg1();
    Operand::ptr_t arg1Op;
    if (tac::helper::isType<Variable>(arg1)) {
      auto destVar = std::static_pointer_cast<Variable>(arg1);
      arg1Op = gas->getRegisterManager()->getLocationForVariable(
          currentFunction, destVar);
    } else if (tac::helper::isType<ArrayAccess>(arg1)) {
      auto arrAcc = std::static_pointer_cast<ArrayAccess>(arg1);

      // if array not already defined
      if (lookupDefinedArray(arrAcc->getArray()) == definedArrays.end()) {
        defineArray(gas, arrAcc);
      }

      arg1Op = loadArrayAccess(gas, arrAcc);
    }

    if (triple->containsArg2()) {
      auto arg2 = triple->getArg2();
      if (tac::helper::isType<ArrayAccess>(arg2)) {
        auto arrAcc = std::static_pointer_cast<ArrayAccess>(arg2);

        bool loadToTemp = arg1Op->isAddress();
        auto arg2Op = loadArrayAccess(gas, arrAcc, loadToTemp);
        gas->addMnemonic(
            std::make_shared<Mnemonic>(Instruction::MOV, arg1Op, arg2Op));
      } else {
        auto arg2Op = gas->loadOperandToRegister(currentFunction, arg2);

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

void defineArray(Gas *gas, ArrayAccess::ptr_t arrAcc) {
  // TODO refactor for variable length array
  auto arr = arrAcc->getArray();
  auto esp = std::make_shared<Operand>(Register::ESP);
  auto tmp = gas->getRegisterManager()->getTmpRegister();

  computeAndStoreArrayStartAddress(gas, arrAcc);

  // make space on stack for new array
  auto arrLength = arr->length();
  auto arrLengthOp = std::make_shared<Operand>(std::to_string(arrLength));
  // for VLA
  //          auto arrLengthOp = gas->loadOperandToRegister(currentFunction,
  //          arr->length());

  auto arrTypeSize = gas->getRegisterManager()->getSize(arr->getType());
  auto arrTypeSizeOp = std::make_shared<Operand>(std::to_string(arrTypeSize));
  gas->addMnemonic(
      std::make_shared<Mnemonic>(Instruction::MOV, tmp, arrLengthOp));
  gas->addMnemonic(
      std::make_shared<Mnemonic>(Instruction::IMUL, tmp, arrTypeSizeOp));

  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::SUB, esp, tmp));

  // store as defined
  definedArrays.push_back(std::make_pair(arr, arrLength));
}

void computeAndStoreArrayStartAddress(Gas *gas, ArrayAccess::ptr_t arrAcc) {
  // store start address on defined stack position
  auto arr = arrAcc->getArray();
  auto newArrOp = gas->loadOperandToRegister(currentFunction, arr);
  if (!definedArrays.empty()) {
    auto tmp = gas->getRegisterManager()->getTmpRegister();
    // last defined array
    auto lastArrLengthPair = definedArrays.back();
    auto lastArr = lastArrLengthPair.first;
    auto lastArrLength = lastArrLengthPair.second;

    auto lastArrOp = gas->loadOperandToRegister(currentFunction, lastArr);
    auto lastArrLengthOp =
        std::make_shared<Operand>(std::to_string(lastArrLength));

    // load last arr start point to new arr start point
    gas->addMnemonic(
        std::make_shared<Mnemonic>(Instruction::MOV, tmp, lastArrOp));
    gas->addMnemonic(
        std::make_shared<Mnemonic>(Instruction::MOV, newArrOp, tmp));

    auto lastArrTypeSize =
        gas->getRegisterManager()->getSize(lastArr->getType());
    auto lastArrTypeSizeOp =
        std::make_shared<Operand>(std::to_string(lastArrTypeSize));

    // calc stack usage of last array
    gas->addMnemonic(
        std::make_shared<Mnemonic>(Instruction::MOV, tmp, lastArrLengthOp));
    gas->addMnemonic(
        std::make_shared<Mnemonic>(Instruction::IMUL, tmp, lastArrTypeSizeOp));

    // store new arr start pointer
    gas->addMnemonic(
        std::make_shared<Mnemonic>(Instruction::SUB, newArrOp, tmp));
  } else {
    auto ebp = std::make_shared<Operand>(Register::EBP);
    gas->addMnemonic(
        std::make_shared<Mnemonic>(Instruction::MOV, newArrOp, ebp));

    auto functionStackSpace =
        gas->getRegisterManager()->lookupFunctionStackSpace(currentFunction);
    // TODO replace +4 with some lookup - which is the initial offset of local
    // stack offsets
    auto functionStackSpaceOp =
        std::make_shared<Operand>(std::to_string(functionStackSpace + 4));
    gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::SUB, newArrOp,
                                                functionStackSpaceOp));
  }
}

Operand::ptr_t loadArrayAccess(Gas *gas, ArrayAccess::ptr_t arrAcc,
                               bool loadIntoTempReg) {
  auto tmp = gas->getRegisterManager()->getTmpRegister();

  auto arrOffsetOp =
      gas->loadOperandToRegister(currentFunction, arrAcc->getPos());
  auto arrTypeSize = gas->getRegisterManager()->getSize(arrAcc->getType());
  auto arrTypeSizeOp = std::make_shared<Operand>(std::to_string(arrTypeSize));

  gas->addMnemonic(
      std::make_shared<Mnemonic>(Instruction::MOV, tmp, arrOffsetOp));
  gas->addMnemonic(
      std::make_shared<Mnemonic>(Instruction::IMUL, tmp, arrTypeSizeOp));

  auto arrStartOp =
      gas->loadOperandToRegister(currentFunction, arrAcc->getArray());
  gas->addMnemonic(
      std::make_shared<Mnemonic>(Instruction::SUB, tmp, arrStartOp));
  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::NEG, tmp));

  auto arrAccOp = std::make_shared<Operand>(tmp, 0);

  if (loadIntoTempReg) {
    gas->addMnemonic(
        std::make_shared<Mnemonic>(Instruction::MOV, tmp, arrAccOp));
    return tmp;
  }

  return arrAccOp;
}
}
}
}