#include "mcc/tac/variable.h"

#include <cassert>

namespace mcc {
namespace tac {
unsigned Variable::nextId = 1;

Variable::Variable(Type t, Scope::ptr_t scope) : Variable(t, "", scope) {
  // necessary to do it after the called constructor has finished
  this->name = "$t" + std::to_string(this->id);
}

Variable::Variable(Type t, std::string name, Scope::ptr_t scope, unsigned index)
    : Operand(t, scope), name(name), index(index), isArg(false) {
  this->id = ++Variable::nextId;
}

// needed exactly this way because Variable is used as key elem for maps if
// this needs to be changed then all maps need a key_comp function that does
// exactly this
bool Variable::operator<(Variable const other) const {
  return (this->getValue() < other.getValue());
}

unsigned Variable::getId() const { return this->id; }

bool Variable::isLeaf() const { return true; }

std::string Variable::getName() const { return name; }

std::string Variable::toString() const { return this->getValue(); }

std::string Variable::getNameWithIndex() const {
  if (this->isTemporary()) {
    return this->getName();
  } else {
    return this->getName() + std::to_string(index);
  }
}

std::string Variable::getValue() const {
  std::string value(this->getNameWithIndex());
  value.append(":");
  value.append(std::to_string(Operand::getScope()->getDepth()));
  value.append(":");
  value.append(std::to_string(Operand::getScope()->getIndex()));

  return value;
}

Scope::ptr_t const Variable::getScope() const {
  Scope::ptr_t scope = Operand::getScope();
  if (this->isTemporary()) {
    assert((scope != std::make_shared<Scope>(0, 0)) &&
           "temp variable has wrong scope");
  }
  return scope;
}

// void Variable::setIndex(unsigned index) { this->index = index; }

bool Variable::isTemporary() const {
  return (getName().find("$t") != std::string::npos);
}

void Variable::setTypeArgument() { this->isArg = true; }

bool Variable::isArgument() const { return this->isArg; }

bool Variable::isArray() const { return false; }
}
}
