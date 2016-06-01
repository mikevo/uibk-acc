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

namespace mcc {
namespace gas {
namespace {
bool resultAvailable;
}

Gas::Gas(Tac& tac) : tac(tac) {
  this->registerManager = std::make_shared<RegisterManager>(tac);

  this->constantFloatsMap = std::make_shared<constant_floats_map_type>();

  this->convertTac(tac);
}

void Gas::convertTac(Tac& tac) {
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
        convertPush(triple);
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

  if (currentFunction && labelTriple->isFunctionEntry()) {
    // TODO maybe there is a nicer way to check if prev instruction was RET
    if (asmInstructions.back()->getInstruction() != Instruction::RET) {
      // old function ends here
      this->createFunctionEpilog(currentFunction);
    }
  }

  auto label = std::make_shared<Mnemonic>(labelTriple->getName());
  asmInstructions.push_back(label);

  // Add function entry
  if (labelTriple->isFunctionEntry()) {
    currentFunction = labelTriple;
    this->createFunctionProlog(labelTriple);
  }
}

void Gas::convertCall(Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    auto operand = triple->getArg1();
    if (helper::isType<Label>(operand)) {
      auto label = std::static_pointer_cast<Label>(operand);
      auto asmLabel = std::make_shared<Operand>(label->getName());

      prepareCall(label);

      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::CALL, asmLabel));

      // Assign result to variable
      if (triple->containsTargetVar()) {
        auto destVar = triple->getTargetVariable();
        if (this->registerManager->getSize(destVar) > 0) {
          resultAvailable = true;
          auto result = std::make_shared<Operand>(Register::EAX);
          this->storeVariableFromRegister(destVar, result);
        }
      }

      cleanUpCall(label);
    }
  }
}

void Gas::convertReturn(Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    // TODO godbolt produces different gas code for float, but it seems to work
    // with the int implementation
    auto reg = this->loadOperandToRegister(triple->getArg1());
    auto eax = std::make_shared<Operand>(Register::EAX);
    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::MOV, eax, reg));
  }

  this->createFunctionEpilog(currentFunction);

  asmInstructions.push_back(std::make_shared<Mnemonic>(Instruction::RET));
}

void Gas::convertAssign(Triple::ptr_t triple) {
  if (triple->containsArg1()) {
    auto op = triple->getArg1();
    if (helper::isType<Variable>(op)) {
      auto destVar = std::static_pointer_cast<Variable>(op);

      if (triple->containsArg2()) {
        auto reg = this->loadOperandToRegister(triple->getArg2());
        if (reg->isFloatConstant()) {
          this->storeStackVariableFromRegister(destVar, reg);
        } else {
          this->storeVariableFromRegister(destVar, reg);
        }
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
  Operand::ptr_t tmp;

  auto operatorName = triple->getOperator().getName();

  reg0 = this->loadOperandToRegister(triple->getArg1());

  tmp = this->registerManager->getTmpRegister();
  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, tmp, reg0));

  if (operatorName == OperatorName::DIV) {
    asmInstructions.push_back(std::make_shared<Mnemonic>(Instruction::CDQ));
  }

  reg1 = this->loadOperandToRegister(triple->getArg2());

  switch (operatorName) {
    case OperatorName::ADD:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::ADD, tmp, reg1));
      break;
    case OperatorName::SUB:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::SUB, tmp, reg1));
      break;
    case OperatorName::MUL:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::IMUL, tmp, reg1));
      break;
    case OperatorName::DIV:
      asmInstructions.push_back(
          std::make_shared<Mnemonic>(Instruction::IDIV, reg1));
      break;
    default:
      assert(false && "unknown operation");
  }

  this->storeVariableFromRegister(triple->getTargetVariable(), tmp);
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

  if (triple->containsTargetVar()) {
    storeVariableFromFloatRegister(triple->getTargetVariable());
  }
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
  Operand::ptr_t tmp;

  reg0 = this->loadOperandToRegister(triple->getArg1());
  reg1 = this->loadOperandToRegister(triple->getArg2());

  if (reg0->isAddress()) {
    tmp = this->registerManager->getTmpRegister();
    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::MOV, tmp, reg0));
  } else {
    tmp = reg0;
  }

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::CMP, tmp, reg1));
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

void Gas::convertPush(Triple::ptr_t triple) {
  convertUnary(triple, Instruction::PUSH);
}

void Gas::convertUnary(Triple::ptr_t triple, Instruction i) {
  if (i == Instruction::NEG && triple->getType() == Type::FLOAT) {
    convertFloatMinus(triple);
  } else {
    // convert all other unary operands as follows
    auto eax = this->loadOperandToRegister(triple->getArg1());
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
    storeVariableFromFloatRegister(triple->getTargetVariable());
  }
}

std::shared_ptr<Operand> Gas::loadOperandToRegister(
    mcc::tac::Operand::ptr_t op) {
  if (helper::isType<Variable>(op)) {
    auto variableOp = std::static_pointer_cast<Variable>(op);
    return this->registerManager->getLocationForVariable(currentFunction,
                                                         variableOp);
  } else if (helper::isType<Triple>(op)) {
    auto triple = std::static_pointer_cast<Triple>(op);
    auto variableOp = triple->getTargetVariable();
    return this->registerManager->getLocationForVariable(currentFunction,
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

std::shared_ptr<Operand> Gas::loadSpilledVariable(Variable::ptr_t var,
                                                  Operand::ptr_t reg) {
  unsigned varOffset =
      this->registerManager->lookupVariableStackOffset(currentFunction, var);
  // register of operand is ignored
  auto asmVar = std::make_shared<Operand>(varOffset);

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, reg, asmVar));

  return asmVar;
}

void Gas::loadVariableToRegister(Variable::ptr_t var, Operand::ptr_t reg) {
  unsigned varOffset =
      this->registerManager->lookupVariableStackOffset(currentFunction, var);
  auto asmVar = std::make_shared<Operand>(varOffset);

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, reg, asmVar));
}

void Gas::storeVariableFromRegister(Variable::ptr_t var, Operand::ptr_t reg) {
  auto asmVar =
      this->registerManager->getLocationForVariable(currentFunction, var);

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, asmVar, reg));
}

void Gas::storeStackVariableFromRegister(Variable::ptr_t var,
                                         Operand::ptr_t reg) {
  auto stackVar = getAsmVar(var);

  auto tmp = this->registerManager->getTmpRegister();
  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, tmp, reg));
  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, stackVar, tmp));
}

void Gas::storeVariableFromFloatRegister(Variable::ptr_t var) {
  auto stackVar = getAsmVar(var);
  auto asmVar =
      this->registerManager->getLocationForVariable(currentFunction, var);

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
  unsigned varOffset =
      this->registerManager->lookupVariableStackOffset(currentFunction, var);
  auto asmVar = std::make_shared<Operand>(varOffset);

  return asmVar;
}

std::vector<Mnemonic::ptr_t>& Gas::getAsmInstructions() {
  return asmInstructions;
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

void Gas::prepareCall(Label::ptr_t label) {
  // TODO find better solution
  unsigned argSize = this->registerManager->lookupFunctionArgSize(label);
  unsigned pos = asmInstructions.size() - argSize / 4;
  storeRegisters({Register::ECX, Register::EDX}, pos);
  resultAvailable = false;
}

void Gas::cleanUpCall(Label::ptr_t label) {
  // Cleanup stack
  unsigned argSize = this->registerManager->lookupFunctionArgSize(label);

  std::vector<Mnemonic::ptr_t>::iterator it = asmInstructions.end();

  int lineOffset = 0;
  if (resultAvailable) --lineOffset;
  resultAvailable = false;

  if (argSize > 0) {
    auto esp = std::make_shared<Operand>(Register::ESP);
    auto stackspaceOp = std::make_shared<Operand>(std::to_string(argSize));

    asmInstructions.insert(
        it + lineOffset,
        std::make_shared<Mnemonic>(Instruction::ADD, esp, stackspaceOp));
  }

  // Restore registers
  restoreRegisters({Register::EDX, Register::ECX},
                   asmInstructions.size() + lineOffset);
}

void Gas::createFunctionProlog(Label::ptr_t label) {
  auto ebp = std::make_shared<Operand>(Register::EBP);
  auto esp = std::make_shared<Operand>(Register::ESP);

  asmInstructions.push_back(std::make_shared<Mnemonic>(Instruction::PUSH, ebp));

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, ebp, esp));

  unsigned stackSpace = this->registerManager->lookupFunctionStackSpace(label);

  // Do we need space for temporaries on the stack?
  if (stackSpace > 0) {
    auto stackspaceOp = std::make_shared<Operand>(std::to_string(stackSpace));
    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::SUB, esp, stackspaceOp));
  }

  // Store callee saved registers
  storeRegisters({Register::EBX, Register::EDI, Register::ESI});

  for (auto var :
       this->registerManager->lookupFunctionVariables(currentFunction)) {
    auto reg =
        this->registerManager->getLocationForVariable(currentFunction, var);

    this->loadSpilledVariable(var, reg);
  }
}

void Gas::createFunctionEpilog(Label::ptr_t label) {
  unsigned stackSpace = this->registerManager->lookupFunctionStackSpace(label);
  auto esp = std::make_shared<Operand>(Register::ESP);
  auto ebp = std::make_shared<Operand>(Register::EBP);

  restoreRegisters({Register::ESI, Register::EDI, Register::EBX});

  // Cleanup stack
  if (stackSpace > 0) {
    auto stackspaceOp = std::make_shared<Operand>(std::to_string(stackSpace));

    asmInstructions.push_back(
        std::make_shared<Mnemonic>(Instruction::ADD, esp, stackspaceOp));
  }

  asmInstructions.push_back(
      std::make_shared<Mnemonic>(Instruction::MOV, esp, ebp));
  asmInstructions.push_back(std::make_shared<Mnemonic>(Instruction::POP, ebp));
}

std::shared_ptr<RegisterManager> Gas::getRegisterManager() {
  return registerManager;
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
