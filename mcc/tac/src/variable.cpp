#include "mcc/tac/variable.h"

#include <cassert>

namespace mcc {
namespace tac {
unsigned Variable::nextId = 1;

Variable::Variable(Type t) : Variable(t, "", std::make_shared<Scope>(0, 0)) {
  // necessary to do it after the called constructor has finished
  this->name = "$t" + std::to_string(this->id);
}

Variable::Variable(Type t, std::string name, Scope::ptr_t scope, unsigned index)
    : Operand(t), name(name), scope(scope), index(index) {
  this->id = ++Variable::nextId;
}

// needed exactly this way because Variable is used as key elem for maps if
// this needs to be changed then all maps need a key_comp function that does
// exactly this
bool Variable::operator<(Variable const other) const {
  auto pairLhs = std::make_pair(this->getNameWithIndex(), this->getScope());
  auto pairRhs = std::make_pair(other.getNameWithIndex(), other.getScope());

  return (pairLhs < pairRhs);
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
  value.append(std::to_string(this->scope->getDepth()));
  value.append(":");
  value.append(std::to_string(this->scope->getIndex()));

  return value;
}

Scope::ptr_t const Variable::getScope() const {
  if (this->isTemporary()) {
    assert((this->scope != std::make_shared<Scope>(0, 0)) &&
           "temp variable has wrong scope");
  }
  return this->scope;
}

// void Variable::setIndex(unsigned index) { this->index = index; }

bool Variable::isTemporary() const {
  return (getName().find("$t") != std::string::npos);
}
}
}
