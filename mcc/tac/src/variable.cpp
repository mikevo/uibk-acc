#include "mcc/tac/variable.h"

namespace mcc {
namespace tac {
unsigned Variable::nextId = 1;

Variable::Variable(Type t) : Variable(t, "") {
  // necessary to do it after the called constructor has finished
  this->name = "$t" + std::to_string(this->id);
}

Variable::Variable(Type t, std::string name)
    : Operand(t), name(name), index(0) {
  this->id = ++Variable::nextId;

  // TODO: wrong scope info
  this->scope = std::make_shared<Scope>(0, 0);
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

std::string Variable::getNameWithIndex() const {
  return this->getName() + std::to_string(index);
}

std::string Variable::getValue() const {
  std::string value(this->getNameWithIndex());
  value.append(":");
  value.append(std::to_string(this->scope->getDepth()));
  value.append(":");
  value.append(std::to_string(this->scope->getIndex()));

  return value;
}

std::shared_ptr<Scope> const Variable::getScope() const { return this->scope; }

void Variable::setScope(std::shared_ptr<Scope> const scope) {
  this->scope = scope;
}

void Variable::setIndex(unsigned index) { this->index = index; }

bool Variable::isTemporary() const {
  return (getName().find("$t") != std::string::npos);
}
}
}
