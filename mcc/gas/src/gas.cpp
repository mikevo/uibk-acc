#include "mcc/gas/gas.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <memory>
#include <ostream>
#include <typeinfo>

#include "mcc/gas/x86_instruction_set.h"
#include "mcc/tac/float_literal.h"
#include "mcc/tac/float_literal.h"
#include "mcc/tac/helper/ast_converters.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/type.h"
#include "mcc/tac/variable.h"

namespace mcc {
namespace gas {
namespace {

unsigned getSize(mcc::tac::Type t) {
  switch (t) {
    case Type::BOOL:
      return 0;  // Is not stored in assembly
      break;
    case Type::INT:
      return sizeof(int);
      break;
    case Type::FLOAT:
      return sizeof(float);
      break;
    default:
      return 0;
  }
}

unsigned getSize(std::shared_ptr<mcc::tac::Operand> operand) {
  return getSize(operand->getType());
}

unsigned getSize(std::vector<mcc::tac::Type> argList) {
  unsigned size = 0;

  for (auto arg : argList) {
    size += getSize(arg);
  }

  return size;
}
}

Gas::Gas(Tac tac) {
  this->functionStackSpaceMap =
      std::make_shared<function_stack_space_map_type>();
  this->variableStackOffsetMap =
      std::make_shared<variable_stack_offset_map_type>();
  this->functionArgSizeMap = std::make_shared<function_arg_size_type>();

  this->convertTac(tac);
}

void Gas::analyzeTac(Tac& tac) {
  // get argSize of all declared functions
  for (auto e : *tac.getFunctionPrototypeMap().get()) {
    (*this->functionArgSizeMap)[e.first] = getSize(e.second);
  }

  Label::ptr_t currentFunctionLabel = nullptr;
  unsigned stackSpace = 0;

  // Begin offset space for ebp and return address
  unsigned curLocalOffset = 8;
  signed curParamOffset = -4;

  for (auto codeLine : tac.codeLines) {
    auto opName = codeLine->getOperator().getName();
    if (opName == OperatorName::LABEL) {
      auto label = std::static_pointer_cast<Label>(codeLine);
      if (label->isFunctionEntry()) {
        // if new function is entered
        if (currentFunctionLabel != nullptr) {
          this->setFunctionStackSpace(currentFunctionLabel, stackSpace);

          stackSpace = 0;

          // Begin with space for ebp and return address
          curLocalOffset = 8;
          curParamOffset = -4;
        }

        currentFunctionLabel = label;
      }
    } else if (codeLine->containsTargetVar()) {
      auto targetVar = codeLine->getTargetVariable();
      if (codeLine->getOperator().getName() == OperatorName::POP) {
        (*variableStackOffsetMap)[targetVar] = curParamOffset;
        curParamOffset -= getSize(targetVar);
      } else {
        // if variable not parameter of function
        (*variableStackOffsetMap)[targetVar] = curLocalOffset;
        curLocalOffset += getSize(targetVar);

        stackSpace += getSize(targetVar);
      }
    }
  }

  // add last function
  if (currentFunctionLabel) {
    this->setFunctionStackSpace(currentFunctionLabel, stackSpace);
  }
}

void Gas::setFunctionStackSpace(Label::ptr_t functionLabel,
                                unsigned stackSpace) {
  assert(functionLabel->isFunctionEntry() && "Not a function label!");

  auto result = functionStackSpaceMap->find(functionLabel);
  if (result != functionStackSpaceMap->end()) {
    //    functionStackSpaceMap->at(functionLabel) = stackSpace;
    (*functionStackSpaceMap)[functionLabel] = stackSpace;
  } else {
    functionStackSpaceMap->insert(std::make_pair(functionLabel, stackSpace));
  }
}

std::shared_ptr<function_stack_space_map_type> Gas::getFunctionStackSpaceMap() {
  return this->functionStackSpaceMap;
}

std::shared_ptr<variable_stack_offset_map_type>
Gas::getVariableStackOffsetMap() {
  return this->variableStackOffsetMap;
}

unsigned Gas::lookupFunctionArgSize(Label::ptr_t functionLabel) {
  auto found = functionArgSizeMap->find(functionLabel->getName());

  if (found != functionArgSizeMap->end()) {
    return found->second;
  } else {
    return 0;
  }
}

unsigned Gas::lookupFunctionStackSize(Label::ptr_t functionLabel) {
  auto found = functionStackSpaceMap->find(functionLabel);

  if (found != functionStackSpaceMap->end()) {
    return found->second;
  } else {
    return 0;
  }
}

unsigned Gas::lookupVariableStackOffset(Variable::ptr_t var) {
  auto found = variableStackOffsetMap->find(var);

  if (found != variableStackOffsetMap->end()) {
    return found->second;
  } else {
    return 0;
  }
}

void Gas::convertTac(Tac& tac) {
  this->analyzeTac(tac);
  Label::ptr_t currentFunction = std::make_shared<Label>();

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
        convertAddSubMulDiv(triple);
        break;

      case OperatorName::ASSIGN:
        convertAssign(triple);
        break;

      case OperatorName::LABEL:
        convertLabel(triple, currentFunction);
        break;

      case OperatorName::JUMP:
        convertJump(triple);
        break;

      case OperatorName::JUMPFALSE:
        convertJumpFalse(triple);
        break;

      case OperatorName::EQ:
      case OperatorName::NE:
      case OperatorName::LE:
      case OperatorName::GE:
      case OperatorName::LT:
      case OperatorName::GT:
        convertLogicOperator(triple);
        break;

      case OperatorName::MINUS:
        convertMinus(triple);
        break;

      case OperatorName::NOT:
        // TODO implement
        break;

      case OperatorName::PUSH:
        convertPush(triple);
        break;

      case OperatorName::POP:
        /*Do nothing*/
        break;

      case OperatorName::CALL:
        convertCall(triple);
        break;

      case OperatorName::RET:
        convertReturn(triple, currentFunction);
        break;
    }
  }
}

void Gas::convertLabel(Triple::ptr_t triple, Label::ptr_t currentFunction) {
  auto labelTriple = std::static_pointer_cast<Label>(triple);

  auto label = std::make_shared<Mnemonic>(labelTriple->getName());
  asmInstructions.push_back(label);

  // Add function entry
  if (labelTriple->isFunctionEntry()) {
    *currentFunction.get() = *labelTriple.get();

    auto ebp = std::make_shared<Operand>(Register::EBP);
    auto esp = std::make_shared<Operand>(Register::ESP);

    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::PUSH, ebp));

    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::MOV, ebp, esp));

    unsigned stackSize = lookupFunctionStackSize(labelTriple);

    // Do we need space for temporaries?
    if (stackSize > 0) {
      auto stackspaceOp = std::make_shared<Operand>(std::to_string(stackSize));
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::SUB, esp, stackspaceOp));

      // Make space on stack for variables
    }
  }
}

void Gas::convertCall(Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    auto operand = triple->getArg1();
    if (helper::isType<Label>(operand)) {
      auto label = std::static_pointer_cast<Label>(operand);
      auto asmLabel = std::make_shared<Operand>(label->getName());

      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::CALL, asmLabel));

      // Cleanup stack
      unsigned argSize = lookupFunctionArgSize(label);

      if (argSize > 0) {
        auto esp = std::make_shared<Operand>(Register::ESP);
        auto stackspaceOp = std::make_shared<Operand>(std::to_string(argSize));

        asmInstructions.push_back(
            std::make_shared<Mnemonic>(Instruction::ADD, esp, stackspaceOp));
      }
    }
  }

  // Assign result to variable
  if (triple->containsTargetVar()) {
    auto result = std::make_shared<Operand>(Register::EAX);
    auto destVar = triple->getTargetVariable();
    this->storeVariableFromRegister(destVar, result);
  }
}

void Gas::convertReturn(Triple::ptr_t triple, Label::ptr_t currentFunction) {
  auto eax = std::make_shared<Operand>(Register::EAX);
  auto ebp = std::make_shared<Operand>(Register::EBP);
  auto esp = std::make_shared<Operand>(Register::ESP);

  if (triple->containsArg1()) {
    auto op = triple->getArg1();

    if (helper::isType<IntLiteral>(op)) {
      auto intOp = std::static_pointer_cast<IntLiteral>(op);
      auto asmInt = std::make_shared<Operand>(intOp->getValue());

      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::MOV, eax, asmInt));

    } else if (helper::isType<FloatLiteral>(op)) {
      auto floatOp = std::static_pointer_cast<FloatLiteral>(op);
      auto asmFloat = std::make_shared<Operand>(floatOp->getValue());

    } else if (helper::isType<Variable>(op)) {
      auto variableOp = std::static_pointer_cast<Variable>(op);

      unsigned varOffset = lookupVariableStackOffset(variableOp);

      auto asmVar = std::make_shared<Operand>(Register::EBP, varOffset);

      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::MOV, eax, asmVar));
    }
  }

  unsigned stackSize = lookupFunctionStackSize(currentFunction);

  // Cleanup stack
  if (stackSize > 0) {
    auto stackspaceOp = std::make_shared<Operand>(std::to_string(stackSize));
    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::ADD, esp, stackspaceOp));
  }

  asmInstructions.push_back(std::make_shared<Mnemonic>(Instruction::POP, ebp));

  asmInstructions.push_back(std::make_shared<Mnemonic>(Instruction::RET));
}

void Gas::convertPush(Triple::ptr_t triple) {
  convertUnary(triple, Instruction::PUSH);
}

void Gas::convertAssign(Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    auto op = triple->getArg1();
    if (helper::isType<Variable>(op)) {
      auto destVar = std::static_pointer_cast<Variable>(op);

      if (triple->containsArg2()) {
        auto srcOp = triple->getArg2();

        auto reg = this->loadOperandToRegister(srcOp, Register::EAX);
        this->storeVariableFromRegister(destVar, reg);
      }
    }
  }
}

void Gas::convertAddSubMulDiv(Triple::ptr_t triple) {
  Operand::ptr_t reg0;
  Operand::ptr_t reg1;

  auto operatorName = triple->getOperator().getName();

  reg0 = this->loadOperandToRegister(triple->getArg1(), Register::EAX);

  if (operatorName == OperatorName::DIV) {
    asmInstructions.push_back(std::make_shared<Mnemonic>(Instruction::CDQ));
  }

  reg1 = this->loadOperandToRegister(triple->getArg2(), Register::ECX);

  switch (operatorName) {
    case OperatorName::ADD:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::ADD, reg0, reg1));
      break;
    case OperatorName::SUB:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::SUB, reg0, reg1));
      break;
    case OperatorName::MUL:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::IMUL, reg0, reg1));
      break;
    case OperatorName::DIV:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::IDIV, reg1));
      break;
    default:
      assert(false && "unknown operation");
  }

  this->storeVariableFromRegister(triple->getTargetVariable(), reg0);
}

void Gas::convertLogicOperator(Triple::ptr_t triple) {
  auto reg0 = std::make_shared<Operand>(Register::EAX);
  auto reg1 = std::make_shared<Operand>(Register::EDX);
  auto operatorName = triple->getOperator().getName();

  reg0 = this->loadOperandToRegister(triple->getArg1(), Register::EAX);
  reg1 = this->loadOperandToRegister(triple->getArg2(), Register::EDX);

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::CMP, reg0, reg1));

  lastOperator = operatorName;
}

void Gas::convertJump(Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    auto operand = triple->getArg1();
    if (helper::isType<Label>(operand)) {
      auto label = std::static_pointer_cast<Label>(operand);
      auto asmLabel = std::make_shared<Operand>(label->getName());

      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::JMP, asmLabel));
    }
  }
}

void Gas::convertJumpFalse(Triple::ptr_t triple) {
  if (triple->containsArg2()) {
    auto operand = triple->getArg2();
    if (helper::isType<Label>(operand)) {
      auto label = std::static_pointer_cast<Label>(operand);
      auto asmLabel = std::make_shared<Operand>(label->getName());
      switch (lastOperator) {
        case OperatorName::EQ:
          asmInstructions.push_back(
              std::make_shared<Mnemonic>(Instruction::JNE, asmLabel));
          break;
        case OperatorName::NE:
          asmInstructions.push_back(
              std::make_shared<Mnemonic>(Instruction::JE, asmLabel));
          break;
        case OperatorName::LE:
          asmInstructions.push_back(
              std::make_shared<Mnemonic>(Instruction::JG, asmLabel));
          break;
        case OperatorName::GE:
          asmInstructions.push_back(
              std::make_shared<Mnemonic>(Instruction::JL, asmLabel));
          break;
        case OperatorName::LT:
          asmInstructions.push_back(
              std::make_shared<Mnemonic>(Instruction::JGE, asmLabel));
          break;
        case OperatorName::GT:
          asmInstructions.push_back(
              std::make_shared<Mnemonic>(Instruction::JLE, asmLabel));
          break;
        default:
          assert(false && "Unknown compare for jump false");
      }
    }
  }
}

void Gas::convertMinus(Triple::ptr_t triple) {
  convertUnary(triple, Instruction::NEG);
}

void Gas::convertNot(Triple::ptr_t triple) {
  convertUnary(triple, Instruction::NOT);
}

void Gas::convertUnary(Triple::ptr_t triple, Instruction i) {
  auto eax = this->loadOperandToRegister(triple->getArg1(), Register::EAX);
  asmInstructions.push_back(std::make_shared<Mnemonic>(i, eax));

  if (triple->containsTargetVar()) {
    auto var = triple->getTargetVariable();
    this->storeVariableFromRegister(var, eax);
  }
}

std::shared_ptr<Operand> Gas::loadOperandToRegister(mcc::tac::Operand::ptr_t op,
                                                    Register r) {
  // TODO do not allways create a new register
  auto reg = std::make_shared<Operand>(r);

  if (helper::isType<Variable>(op)) {
    auto variableOp = std::static_pointer_cast<Variable>(op);
    this->loadVariableToRegister(variableOp, reg);
  } else if (helper::isType<Triple>(op)) {
    auto triple = std::static_pointer_cast<Triple>(op);
    this->loadVariableToRegister(triple->getTargetVariable(), reg);
  } else {
    auto operand = std::make_shared<Operand>(op->getValue());

    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::MOV, reg, operand));
  }

  return reg;
}

void Gas::loadVariableToRegister(Variable::ptr_t var, Operand::ptr_t reg) {
  unsigned varOffset = lookupVariableStackOffset(var);
  auto asmVar = std::make_shared<Operand>(Register::EBP, varOffset);

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, reg, asmVar));
}
void Gas::storeVariableFromRegister(Variable::ptr_t var, Operand::ptr_t reg) {
  unsigned varOffset = lookupVariableStackOffset(var);
  auto asmVar = std::make_shared<Operand>(Register::EBP, varOffset);

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, asmVar, reg));
}

std::string Gas::toString() const {
  std::ostringstream stream;

  stream << ".intel_syntax noprefix" << std::endl;
  stream << ".global main" << std::endl;

  for (auto mnemonic : asmInstructions) {
    stream << mnemonic << "\n";
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
