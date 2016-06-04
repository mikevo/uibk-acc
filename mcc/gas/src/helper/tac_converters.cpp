/*
 * File:   tac_converter.cpp
 *
 * Created on June 1, 2016, 10:19 PM
 */

#include "mcc/tac/tac.h"
#include "mcc/cfg/cfg.h"
#include "mcc/gas/gas.h"
#include "mcc/gas/helper/tac_converters.h"
#include "mcc/gas/helper/tac_float_converters.h"
#include "mcc/gas/helper/tac_int_converters.h"
#include "mcc/tac/array.h"
#include "mcc/tac/array_access.h"
#include "mcc/tac/helper/ast_converters.h"

using namespace mcc::tac;
using namespace mcc::gas;
using namespace mcc::cfg;

bool resultAvailable;
Label::ptr_t currentFunction;
OperatorName lastOperator;

std::vector<std::tuple<Label::ptr_t, Array::ptr_t, mcc::gas::Operand::ptr_t>>
    definedArrays;

namespace mcc {
namespace gas {
namespace helper {
void convertTac(Gas *gas, Tac &tac) {
  auto cfg = Cfg(tac);
  auto arrayLiveRanges = cfg.getArrayLiveRangeMap();

  for (auto triple : tac.codeLines) {
    auto op = triple->getOperator();

    switch (op.getName()) {
      case OperatorName::NOP:
        /*Do nothing*/
        break;

      case OperatorName::ADD:
      case OperatorName::SUB:
      case OperatorName::MUL:
      case OperatorName::DIV:
        convertArithmetic(gas, triple);
        break;

      case OperatorName::ASSIGN:
        convertAssign(gas, triple);
        break;

      case OperatorName::LABEL:
        convertLabel(gas, triple);
        break;

      case OperatorName::JUMP:
        convertJump(gas, triple);
        break;

      case OperatorName::JUMPFALSE:
        convertJumpFalse(gas, triple);
        break;

      case OperatorName::EQ:
      case OperatorName::NE:
      case OperatorName::LE:
      case OperatorName::GE:
      case OperatorName::LT:
      case OperatorName::GT:
        convertLogicOperator(gas, triple);
        break;

      case OperatorName::MINUS:
        convertMinus(gas, triple);
        break;

      case OperatorName::NOT:
        // TODO should be checked in previous stages
        assert(triple->getType() == Type::BOOL &&
               "NOT operation only for booleans!");
        convertNot(gas, triple);
        break;

      case OperatorName::PUSH:
        convertPush(gas, triple);
        break;

      case OperatorName::POP:
        /*Do nothing*/
        break;

      case OperatorName::CALL:
        convertCall(gas, triple);
        break;

      case OperatorName::RET:
        convertReturn(gas, triple);
        break;
    }

    // clean up arrays
    auto liveSetAfter = cfg.liveSetAfter(triple, false);
    for (auto definedArrIt = definedArrays.rbegin();
         definedArrIt != definedArrays.rend(); ++definedArrIt) {
      auto definedArrFunc = std::get<0>(*definedArrIt);
      auto definedArr = std::get<1>(*definedArrIt);
      auto definedArrLength = std::get<2>(*definedArrIt);

      auto result = arrayLiveRanges.find(definedArr);
      if (result != arrayLiveRanges.end()) {
        // if array found in liverange map
        if (triple > result->second.back()) {
          // if current tac line is after the last use of array

          // clean
          cleanUpArray(gas, currentFunction, definedArr, definedArrLength);
          // delete current array from definedArraysVector
          definedArrays.erase(std::next(definedArrIt).base());
        } else {
          // break if one array is still alive
          break;
        }
      } else {
        assert(false && "Array not in live range map!");
      }
    }
  }

  assert(definedArrays.empty() && "Arrays are not fully cleaned up!");
}

void convertArithmetic(Gas *gas, Triple::ptr_t triple) {
  switch (triple->getType()) {
    case Type::INT:
      convertIntArithmetic(gas, triple);
      break;
    case Type::FLOAT:
      convertFloatArithmetic(gas, triple);
      break;
    default:
      assert(false && "Unhandled operator type in arithmetic conversion");
  }
}

void convertAssign(Gas *gas, Triple::ptr_t triple) {
  if (triple->containsArg2()) {
    auto arg2 = triple->getArg2();
    switch (arg2->getType()) {
      case Type::INT:
        convertIntAssign(gas, triple);
        break;
      case Type::FLOAT:
        convertFloatAssign(gas, triple);
        break;
      default:
        assert("Unhandled type of arg2 in assign op!");
    }
  } else {
    assert(false && "Assignment does not contain arg2!");
  }
}

void convertLabel(Gas *gas, Triple::ptr_t triple) {
  auto labelTriple = std::static_pointer_cast<Label>(triple);

  if (currentFunction && labelTriple->isFunctionEntry()) {
    // TODO maybe there is a nicer way to check if prev instruction was RET
    auto asmInstructions = gas->getAsmInstructions();
    if (asmInstructions.size() > 0 &&
        asmInstructions.back()->getInstruction() != Instruction::RET) {
      // old function ends here
      createFunctionEpilog(gas, currentFunction);
    }
  }

  auto label = std::make_shared<Mnemonic>(labelTriple->getName());
  gas->addMnemonic(label);

  // Add function entry
  if (labelTriple->isFunctionEntry()) {
    currentFunction = labelTriple;
    createFunctionProlog(gas, labelTriple);
  }
}

void convertJump(Gas *gas, Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    auto operand = triple->getArg1();
    if (tac::helper::isType<Label>(operand)) {
      auto label = std::static_pointer_cast<Label>(operand);
      auto asmLabel = std::make_shared<Operand>(label->getName());

      gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::JMP, asmLabel));
    }
  }
}

void convertJumpFalse(Gas *gas, Triple::ptr_t triple) {
  auto condition = std::static_pointer_cast<Triple>(triple->getArg1());
  switch (condition->getArg1()->getType()) {
    case Type::INT:
      convertIntJumpFalse(gas, triple);
      break;
    case Type::FLOAT:
      convertFloatJumpFalse(gas, triple);
      break;
    default:
      std::cout << triple->getType() << std::endl;
      assert(false && "Unhandled operator type in logic conversion");
  }
}

void convertLogicOperator(Gas *gas, Triple::ptr_t triple) {
  auto operatorName = triple->getOperator().getName();

  // use type of an argument because
  switch (triple->getArg1()->getType()) {
    case Type::INT:
      convertIntLogicOperator(gas, triple);
      break;
    case Type::FLOAT:
      convertFloatLogicOperator(gas, triple);
      break;
    default:
      std::cout << triple->getType() << std::endl;
      assert(false && "Unhandled operator type in logic conversion");
  }

  lastOperator = operatorName;
}

void convertMinus(Gas *gas, Triple::ptr_t triple) {
  // TODO distinguish float and int
  switch (triple->getType()) {
    case Type::INT:
      convertUnary(gas, triple, Instruction::NEG);
      break;
    case Type::FLOAT:
      convertFloatMinus(gas, triple);
      break;
    default:
      assert(false && "Unhandled type in convertMinus!");
  }
}

void convertNot(Gas *gas, Triple::ptr_t triple) {
  convertUnary(gas, triple, Instruction::NOT);
}

void convertPush(Gas *gas, Triple::ptr_t triple) {
  convertUnary(gas, triple, Instruction::PUSH);
}

void convertUnary(Gas *gas, Triple::ptr_t triple, Instruction i) {
  bool containsTargetVar = triple->containsTargetVar();
  Operand::ptr_t tmp;
  if (containsTargetVar) {
    tmp = gas->getRegisterManager()->getTmpRegister();
  }

  auto operand = gas->loadOperand(currentFunction, triple->getArg1(), tmp);
  gas->addMnemonic(std::make_shared<Mnemonic>(i, operand));

  if (containsTargetVar) {
    auto var = triple->getTargetVariable();
    gas->storeOperandFromRegister(currentFunction, var, operand);
  }
}

void convertCall(Gas *gas, Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    auto operand = triple->getArg1();
    if (tac::helper::isType<Label>(operand)) {
      auto label = std::static_pointer_cast<Label>(operand);
      auto asmLabel = std::make_shared<Operand>(label->getName());

      prepareCall(gas, label);

      gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::CALL, asmLabel));

      // Assign result to variable
      if (triple->containsTargetVar()) {
        auto destVar = triple->getTargetVariable();
        // TODO check if correct
        if (destVar->getType() != Type::NONE) {
          resultAvailable = true;
          auto result = std::make_shared<Operand>(Register::EAX);
          gas->storeOperandFromRegister(currentFunction, destVar, result);
        }
      }

      cleanUpCall(gas, label);
    }
  }
}

void convertReturn(Gas *gas, Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    // TODO godbolt produces different gas code for float, but it seems to work
    // with the int implementation
    auto reg = gas->loadOperand(currentFunction, triple->getArg1());
    auto eax = std::make_shared<Operand>(Register::EAX);
    gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::MOV, eax, reg));
  }

  createFunctionEpilog(gas, currentFunction);

  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::RET));
}

void prepareCall(Gas *gas, Label::ptr_t functionLabel) {
  // TODO find better solution
  unsigned argSize =
      gas->getRegisterManager()->lookupFunctionArgSize(functionLabel);
  unsigned pos = gas->getAsmInstructions().size() - argSize / 4;
  gas->storeRegisters({Register::ECX, Register::EDX}, pos);
  resultAvailable = false;
}

void cleanUpCall(Gas *gas, Label::ptr_t functionLabel) {
  // Cleanup stack
  unsigned argSize =
      gas->getRegisterManager()->lookupFunctionArgSize(functionLabel);

  std::vector<Mnemonic::ptr_t>::iterator it = gas->getAsmInstructions().end();

  int lineOffset = 0;
  if (resultAvailable) --lineOffset;
  resultAvailable = false;

  if (argSize > 0) {
    auto esp = std::make_shared<Operand>(Register::ESP);
    auto stackspaceOp = std::make_shared<Operand>(std::to_string(argSize));

    gas->getAsmInstructions().insert(
        it + lineOffset,
        std::make_shared<Mnemonic>(Instruction::ADD, esp, stackspaceOp));
  }

  // Restore registers
  gas->restoreRegisters({Register::EDX, Register::ECX},
                        gas->getAsmInstructions().size() + lineOffset);
}

void createFunctionProlog(Gas *gas, Label::ptr_t functionLabel) {
  auto ebp = std::make_shared<Operand>(Register::EBP);
  auto esp = std::make_shared<Operand>(Register::ESP);

  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::PUSH, ebp));

  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::MOV, ebp, esp));

  unsigned stackSpace =
      gas->getRegisterManager()->lookupFunctionStackSpace(functionLabel);

  // Do we need space for temporaries on the stack?
  if (stackSpace > 0) {
    auto stackspaceOp = std::make_shared<Operand>(std::to_string(stackSpace));
    gas->addMnemonic(
        std::make_shared<Mnemonic>(Instruction::SUB, esp, stackspaceOp));
  }

  // Store callee saved registers
  gas->storeRegisters({Register::EBX, Register::EDI, Register::ESI});

  // load function arguments
  for (auto var :
       gas->getRegisterManager()->lookupFunctionVariables(functionLabel)) {
    auto targetReg =
        gas->getRegisterManager()->getLocationForVariable(functionLabel, var);

    auto varOffset = gas->getRegisterManager()->lookupVariableStackOffset(
        functionLabel, var);
    auto argAddrOp = std::make_shared<Operand>(varOffset);

    gas->addMnemonic(
        std::make_shared<Mnemonic>(Instruction::MOV, targetReg, argAddrOp));
  }
}

void createFunctionEpilog(Gas *gas, Label::ptr_t functionLabel) {
  unsigned stackSpace =
      gas->getRegisterManager()->lookupFunctionStackSpace(functionLabel);
  auto esp = std::make_shared<Operand>(Register::ESP);
  auto ebp = std::make_shared<Operand>(Register::EBP);

  gas->restoreRegisters({Register::ESI, Register::EDI, Register::EBX});

  // Cleanup stack
  if (stackSpace > 0) {
    auto stackspaceOp = std::make_shared<Operand>(std::to_string(stackSpace));

    gas->addMnemonic(
        std::make_shared<Mnemonic>(Instruction::ADD, esp, stackspaceOp));
  }

  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::MOV, esp, ebp));
  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::POP, ebp));
}

std::vector<
    std::tuple<Label::ptr_t, Array::ptr_t, mcc::gas::Operand::ptr_t>>::iterator
lookupDefinedArray(Label::ptr_t functionLabel, Array::ptr_t array) {
  auto searchPair = std::make_pair(functionLabel, array);
  for (auto pairIt = definedArrays.begin(); pairIt != definedArrays.end();
       ++pairIt) {
    auto curPair = std::make_pair(std::get<0>(*pairIt), std::get<1>(*pairIt));
    if (!mcc::gas::labelArrayPairLess()(searchPair, curPair) &&
        !mcc::gas::labelArrayPairLess()(curPair, searchPair)) {
      return pairIt;
    }
  }

  return definedArrays.end();
}

void defineArray(Gas *gas, Label::ptr_t functionLabel,
                 ArrayAccess::ptr_t arrAcc) {
  // TODO refactor for variable length array
  auto arr = arrAcc->getArray();
  auto esp = std::make_shared<Operand>(Register::ESP);
  auto tmp = gas->getRegisterManager()->getTmpRegister();

  computeAndStoreArrayStartAddress(gas, functionLabel, arr);

  // make space on stack for new array
  auto arrLength = arr->length();
  auto arrLengthOp = gas->loadOperand(currentFunction, arr->length());

  auto arrTypeSize = gas->getRegisterManager()->getSize(arr->getType());
  auto arrTypeSizeOp = std::make_shared<Operand>(std::to_string(arrTypeSize));
  gas->addMnemonic(
      std::make_shared<Mnemonic>(Instruction::MOV, tmp, arrLengthOp));
  gas->addMnemonic(
      std::make_shared<Mnemonic>(Instruction::IMUL, tmp, arrTypeSizeOp));

  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::SUB, esp, tmp));

  // store as defined
  definedArrays.push_back(std::make_tuple(functionLabel, arr, arrLengthOp));
}

void computeAndStoreArrayStartAddress(Gas *gas, Label::ptr_t functionLabel,
                                      Array::ptr_t arr) {
  // store start address on defined stack position
  auto newArrOp = gas->loadOperand(functionLabel, arr);
  if (!definedArrays.empty()) {
    auto tmp = gas->getRegisterManager()->getTmpRegister();
    // last defined array
    auto lastArrTuple = definedArrays.back();
    auto lastArr = std::get<1>(lastArrTuple);
    auto lastArrLengthOp = std::get<2>(lastArrTuple);

    auto lastArrOp = gas->loadOperand(functionLabel, lastArr);

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
        gas->getRegisterManager()->lookupFunctionStackSpace(functionLabel);
    // TODO replace +4 with some lookup - which is the initial offset of local
    // stack offsets
    auto functionStackSpaceOp =
        std::make_shared<Operand>(std::to_string(functionStackSpace + 4));
    gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::SUB, newArrOp,
                                                functionStackSpaceOp));
  }
}

void cleanUpArray(Gas *gas, Label::ptr_t functionLabel, Array::ptr_t arr,
                  Operand::ptr_t length) {
  auto esp = std::make_shared<Operand>(Register::ESP);
  auto tmp = gas->getRegisterManager()->getTmpRegister();

  auto arrTypeSize = gas->getRegisterManager()->getSize(arr->getType());
  auto arrTypeSizeOp = std::make_shared<Operand>(std::to_string(arrTypeSize));

  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::MOV, tmp, length));
  gas->addMnemonic(
      std::make_shared<Mnemonic>(Instruction::IMUL, tmp, arrTypeSizeOp));

  gas->addMnemonic(std::make_shared<Mnemonic>(Instruction::ADD, esp, tmp));
}
}
}
}