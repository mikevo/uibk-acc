#include "mcc/tac/triple.h"

#include <cassert>
#include <memory>
#include <iostream>
#include <typeinfo>

#include "mcc/tac/variable.h"

namespace mcc {
namespace tac {
unsigned Triple::nextId = 0;

Triple::Triple(std::shared_ptr<Operand> arg)
    : Triple(Operator(OperatorName::NOP), arg) {
  // check if it is a terminal
  assert(arg->isLeaf() && "Operand is non-terminal!");
}

// constructor needed for Label
Triple::Triple(OperatorName op) : Triple(Operator(op), nullptr) {
  assert(this->op.getType() == OperatorType::NONE && "Operator not NONE!");
}

Triple::Triple(Operator op, std::shared_ptr<Operand> arg)
    : Triple(op, arg, nullptr) {
  // check if operator is UNARY or NONE (NOP)
  assert((this->op.getType() == OperatorType::NONE ||
          this->op.getType() == OperatorType::UNARY) &&
         "Operator not UNARY or NONE!");
}

Triple::Triple(Operator op, std::shared_ptr<Operand> arg1,
               std::shared_ptr<Operand> arg2)
    : Operand(),
      op(op),
      basicBlockId(0),
      arg1(arg1),
      arg2(arg2),
      targetVar(nullptr),
      id(0) {
  this->id = ++nextId;

  // there is a restriction that both arguments types need to match
  if (this->containsArg1()) {
    this->setType(arg1->getType());
  }

  this->updateResultType(op);

  switch (op.getType()) {
    case OperatorType::BINARY:
      assert(this->containsArg2() && "second argument needed");
    case OperatorType::UNARY:
      assert(this->containsArg1() && "first argument needed");
      break;
    case OperatorType::NONE:
      assert((!this->containsArg1() && !this->containsArg2()) &&
             "No argument allowed");
      break;
  }

  if (this->containsArg2()) {
    assert((this->op.getName() == OperatorName::JUMPFALSE ||
            this->arg1->getType() == this->arg2->getType()) &&
           "Type mismatch");
  }

  switch (this->op.getName()) {
    case OperatorName::NOP:
    case OperatorName::ASSIGN:
    case OperatorName::JUMPFALSE:
    case OperatorName::JUMP:
    case OperatorName::LABEL:
      break;
    default:
      // Creates a temporary variable that is used for the triple result
      this->setTargetVariable(std::make_shared<Variable>(this->getType()));
  }
}

bool Triple::isLeaf() const { return false; }

unsigned Triple::getId() const { return this->id; }

std::string Triple::getName() const {
  if (this->containsTargetVar()) {
    // TODO: get rid of this construct
    if (this->getTargetVariable()->isTemporary()) {
      return this->getTargetVariable()->getName();
    } else {
      return this->getTargetVariable()->getValue();
    }
  } else {
    assert((this->op.getName() != OperatorName::ASSIGN) &&
           "assign needs to have a target variable");
    return this->name;
  }
}

void Triple::setName(const std::string name) { this->name = name; }

std::string Triple::getValue() const { return getName(); }

unsigned Triple::getBasicBlockId() const { return this->basicBlockId; }

void Triple::setBasicBlockId(unsigned blockId) { this->basicBlockId = blockId; }

std::string Triple::toString() const {
  std::string output;

  switch (this->op.getName()) {
    case OperatorName::JUMPFALSE:
    case OperatorName::JUMP:
      output.append(this->op.toString());
      output.append(this->arg1->getValue());
      if (this->containsArg2()) {
        output.append(" ");
        output.append(this->arg2->getValue());
      }

      return output;

    case OperatorName::LABEL:
      output.append(this->op.toString());
      output.append(this->getName());

      return output;

    case OperatorName::NOP:
      output.append(this->arg1->getValue());

      return output;

    default:
      if (this->op.getName() != OperatorName::ASSIGN) {
        output.append(this->getName());
        output.append(" = ");
      }

      switch (this->op.getType()) {
        case OperatorType::UNARY:

          output.append(this->op.toString());
          output.append(this->arg1->getValue());

          break;
        default:
          output.append(this->arg1->getValue());
          output.append(this->op.toString());
          if (this->containsArg2()) {
            output.append(this->arg2->getValue());
          }
      }
      return output;
  }
}

void Triple::setTargetVariable(std::shared_ptr<Variable> var) {
  assert((var != nullptr) && "don't kill the object");
  this->targetVar = var;
}

std::shared_ptr<Variable> Triple::getTargetVariable() const {
  assert(this->containsTargetVar() &&
         "use containsTargetVar() before you call this function");
  return this->targetVar;
}

bool Triple::containsArg1() const { return (this->arg1 != nullptr); }

bool Triple::containsArg2() const { return (this->arg2 != nullptr); }

bool Triple::containsTargetVar(void) const {
  return (this->targetVar != nullptr);
}

std::shared_ptr<Operand> Triple::getArg1() const {
  assert(this->containsArg1() &&
         "use containsArg1() before you call this function");
  return this->arg1;
}
std::shared_ptr<Operand> Triple::getArg2() const {
  assert(this->containsArg2() &&
         "use containsArg2() before you call this function");
  return this->arg2;
}

Operator Triple::getOperator() const { return this->op; }

void Triple::setArg1(std::shared_ptr<Operand> arg) {
  assert((arg != nullptr) && "nullptr not allowed as operand!");
  this->arg1 = arg;
}

void Triple::setArg2(std::shared_ptr<Operand> arg) {
  assert((arg != nullptr) && "nullptr not allowed as operand!");
  this->arg2 = arg;
}

void Triple::setOperator(Operator op) { this->op = op; }
}
}
