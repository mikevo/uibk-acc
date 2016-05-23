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

Gas::Gas(Tac& tac) {
  this->functionStackSpaceMap =
      std::make_shared<function_stack_space_map_type>();
  this->variableStackOffsetMap =
      std::make_shared<variable_stack_offset_map_type>();
  this->functionArgSizeMap = std::make_shared<function_arg_size_type>();
  this->constantFloatsMap = std::make_shared<constant_floats_map_type>();

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
  unsigned curLocalOffset = -4;
  signed curParamOffset = 8;

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
          curLocalOffset = -4;
          curParamOffset = 8;
        }

        currentFunctionLabel = label;
      }
    } else if (codeLine->containsTargetVar()) {
      auto targetVar = codeLine->getTargetVariable();
      if (codeLine->getOperator().getName() == OperatorName::POP) {
        (*variableStackOffsetMap)[std::make_pair(currentFunctionLabel,
                                                 targetVar)] = curParamOffset;
        curParamOffset += getSize(targetVar);
      } else {
        // if variable not parameter of function
        (*variableStackOffsetMap)[std::make_pair(currentFunctionLabel,
                                                 targetVar)] = curLocalOffset;
        curLocalOffset -= getSize(targetVar);

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
    // TODO this line alone should do the same functionality, or am I wrong?
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
  auto found =
      variableStackOffsetMap->find(std::make_pair(currentFunction, var));

  if (found != variableStackOffsetMap->end()) {
    return found->second;
  } else {
    return 0;
  }
}

void Gas::convertTac(Tac& tac) {
  this->analyzeTac(tac);

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
        convertArithmetic(triple);
        break;

      case OperatorName::ASSIGN:
        convertAssign(triple);
        break;

      case OperatorName::LABEL:
        convertLabel(triple);
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
        // TODO godbolt produces different gas code for float, but it seems to
        // work with the int implementation
        convertLogicOperator(triple);
        break;

      case OperatorName::MINUS:
        convertUnary(triple, Instruction::NEG);
        break;

      case OperatorName::NOT:
        // TODO should be checked in previous stages
        assert(triple->getType() == Type::BOOL &&
               "NOT operation only for booleans!");
        convertUnary(triple, Instruction::NOT);
        break;

      case OperatorName::PUSH:
        convertUnary(triple, Instruction::PUSH);
        break;

      case OperatorName::POP:
        /*Do nothing*/
        break;

      case OperatorName::CALL:
        convertCall(triple);
        break;

      case OperatorName::RET:
        convertReturn(triple);
        break;
    }
  }
}

void Gas::convertLabel(Triple::ptr_t triple) {
  auto labelTriple = std::static_pointer_cast<Label>(triple);

  auto label = std::make_shared<Mnemonic>(labelTriple->getName());
  asmInstructions.push_back(label);

  // Add function entry
  if (labelTriple->isFunctionEntry()) {
    currentFunction = labelTriple;

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
    auto destVar = triple->getTargetVariable();
    if (getSize(destVar) > 0) {
      auto result = std::make_shared<Operand>(Register::EAX);
      this->storeVariableFromRegister(destVar, result);
    }
  }
}

void Gas::convertReturn(Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    // TODO godbolt produces different gas code for float, but it seems to work
    // with the int implementation
    this->loadOperandToRegister(triple->getArg1(), Register::EAX);
  }

  unsigned stackSize = lookupFunctionStackSize(currentFunction);

  // Cleanup stack
  if (stackSize > 0) {
    auto stackspaceOp = std::make_shared<Operand>(std::to_string(stackSize));
    auto esp = std::make_shared<Operand>(Register::ESP);
    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::ADD, esp, stackspaceOp));
  }

  auto ebp = std::make_shared<Operand>(Register::EBP);
  asmInstructions.push_back(std::make_shared<Mnemonic>(Instruction::POP, ebp));

  asmInstructions.push_back(std::make_shared<Mnemonic>(Instruction::RET));
}

void Gas::convertAssign(Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    auto op = triple->getArg1();
    if (helper::isType<Variable>(op)) {
      auto destVar = std::static_pointer_cast<Variable>(op);

      if (triple->containsArg2()) {
        auto reg =
            this->loadOperandToRegister(triple->getArg2(), Register::EAX);
        this->storeVariableFromRegister(destVar, reg);
      }
    }
  }
}

void Gas::convertArithmetic(Triple::ptr_t triple) {
  switch (triple->getType()) {
    case Type::INT:
      convertIntArithmetic(triple);
      break;
    case Type::FLOAT:
      convertFloatArithmetic(triple);
      break;
    default:
      assert(false && "Unhandled operator type in arithmetic conversion");
  }
}

void Gas::convertIntArithmetic(Triple::ptr_t triple) {
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

void Gas::convertFloatArithmetic(Triple::ptr_t triple) {
  pushOperandToFloatRegister(triple->getArg1());
  pushOperandToFloatRegister(triple->getArg2());

  // TODO calling this constructor creates LABEL operands, maybe this leads to
  // problems later
  Operand::ptr_t op1 = std::make_shared<Operand>("st(1)");
  Operand::ptr_t op2 = std::make_shared<Operand>("st");
  auto operatorName = triple->getOperator().getName();
  switch (operatorName) {
    case OperatorName::ADD:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::FADDP, op1, op2));
      break;
    case OperatorName::SUB:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::FSUBP, op1, op2));
      break;
    case OperatorName::MUL:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::FMULP, op1, op2));
      break;
    case OperatorName::DIV:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::FDIVP, op1, op2));
      break;
    default:
      assert(false && "unknown operation");
  }

  auto targetVar = getAsmVar(triple->getTargetVariable());
  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::FSTP, targetVar));
}

void Gas::convertLogicOperator(Triple::ptr_t triple) {
  auto operatorName = triple->getOperator().getName();

  // use type of an argument because
  switch (triple->getArg1()->getType()) {
    case Type::INT:
      convertIntLogicOperator(triple);
      break;
    case Type::FLOAT:
      convertFloatLogicOperator(triple);
      break;
    default:
      std::cout << triple->getType() << std::endl;
      assert(false && "Unhandled operator type in logic conversion");
  }

  lastOperator = operatorName;
}

void Gas::convertIntLogicOperator(Triple::ptr_t triple) {
  Operand::ptr_t reg0;
  Operand::ptr_t reg1;

  reg0 = this->loadOperandToRegister(triple->getArg1(), Register::EAX);
  reg1 = this->loadOperandToRegister(triple->getArg2(), Register::EDX);

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::CMP, reg0, reg1));
}

void Gas::convertFloatLogicOperator(Triple::ptr_t triple) {
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
      pushOperandToFloatRegister(triple->getArg1());
      pushOperandToFloatRegister(triple->getArg2());
      break;
    case OperatorName::LT:
    case OperatorName::LE:
      pushOperandToFloatRegister(triple->getArg2());
      pushOperandToFloatRegister(triple->getArg1());
      break;
    default:
      assert(false && "Unknown operator name in float logic conversion!");
  }

  switch (operatorName) {
    case OperatorName::LT:
    case OperatorName::LE:
    case OperatorName::GT:
    case OperatorName::GE:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::FXCH, st1));
      break;
    default:
      // ignore
      break;
  }

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::FUCOMIP, st, st1));
  asmInstructions.push_back(std::make_shared<Mnemonic>(Instruction::FSTP, st0));

  // jump handled later
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
  auto condition = std::static_pointer_cast<Triple>(triple->getArg1());
  switch (condition->getArg1()->getType()) {
    case Type::INT:
      convertIntJumpFalse(triple);
      break;
    case Type::FLOAT:
      convertFloatJumpFalse(triple);
      break;
    default:
      std::cout << triple->getType() << std::endl;
      assert(false && "Unhandled operator type in logic conversion");
  }
}

void Gas::convertIntJumpFalse(Triple::ptr_t triple) {
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

void Gas::convertFloatJumpFalse(Triple::ptr_t triple) {
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
        case OperatorName::GE:
        case OperatorName::LE:
          asmInstructions.push_back(
              std::make_shared<Mnemonic>(Instruction::JB, asmLabel));
          break;
        case OperatorName::GT:
        case OperatorName::LT:
          asmInstructions.push_back(
              std::make_shared<Mnemonic>(Instruction::JBE, asmLabel));
          break;
        default:
          assert(false && "Unknown compare for jump false");
      }
    }
  }
}

void Gas::convertUnary(Triple::ptr_t triple, Instruction i) {
  if (i == Instruction::NEG && triple->getType() == Type::FLOAT) {
    convertFloatMinus(triple);
  } else {
    // convert all other unary operands as follows
    auto eax = this->loadOperandToRegister(triple->getArg1(), Register::EAX);
    asmInstructions.push_back(std::make_shared<Mnemonic>(i, eax));

    if (triple->containsTargetVar()) {
      auto var = triple->getTargetVariable();
      this->storeVariableFromRegister(var, eax);
    }
  }
}

void Gas::convertFloatMinus(Triple::ptr_t triple) {
  this->pushOperandToFloatRegister(triple->getArg1());
  asmInstructions.push_back(std::make_shared<Mnemonic>(Instruction::FCHS));

  if (triple->containsTargetVar()) {
    auto targetVar = getAsmVar(triple->getTargetVariable());
    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::FSTP, targetVar));
  }
}

std::shared_ptr<Operand> Gas::loadOperandToRegister(mcc::tac::Operand::ptr_t op,
                                                    Register r) {
  // TODO do not always create a new register
  auto reg = std::make_shared<Operand>(r);

  if (helper::isType<Variable>(op)) {
    auto variableOp = std::static_pointer_cast<Variable>(op);
    this->loadVariableToRegister(variableOp, reg);
  } else if (helper::isType<Triple>(op)) {
    auto triple = std::static_pointer_cast<Triple>(op);
    this->loadVariableToRegister(triple->getTargetVariable(), reg);
  } else {
    // constant values
    if (op->getType() == Type::FLOAT) {
      auto floatConstant = createFloatConstant(op->getValue());

      auto operand = std::make_shared<Operand>(floatConstant);
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::MOV, reg, operand));
    } else {
      auto operand = std::make_shared<Operand>(op->getValue());
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::MOV, reg, operand));
    }
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

void Gas::pushOperandToFloatRegister(mcc::tac::Operand::ptr_t op) {
  assert(op->getType() == Type::FLOAT && "Variable is not of type float!");

  Operand::ptr_t asmVar;
  if (helper::isType<Variable>(op)) {
    auto var = std::static_pointer_cast<Variable>(op);
    asmVar = getAsmVar(var);
  } else if (helper::isType<Triple>(op)) {
    auto triple = std::static_pointer_cast<Triple>(op);
    auto var = triple->getTargetVariable();
    asmVar = getAsmVar(var);
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

Operand::ptr_t Gas::getAsmVar(Variable::ptr_t var) {
  unsigned varOffset = lookupVariableStackOffset(var);
  auto asmVar = std::make_shared<Operand>(Register::EBP, varOffset);

  return asmVar;
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
