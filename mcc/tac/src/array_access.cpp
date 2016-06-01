/*
 * ArrayAccess_access.cpp
 *
 *  Created on: Jun 1, 2016
 */

#include "mcc/tac/array_access.h"

#include <cassert>
#include <string>

#include "mcc/tac/scope.h"

namespace mcc {
namespace tac {
ArrayAccess::ArrayAccess(Array::ptr_t array, Operand::ptr_t pos)
    : Variable(array->getType(), array->getName(),
               std::make_shared<Scope>(0, 0)),
      array(array) {
  assert(pos->getType() == Type::INT && "array position not an INT type");
  this->pos = pos;
}

bool ArrayAccess::operator<(ArrayAccess const other) const {
  auto pairLhs = std::make_pair(this->getName(), this->getPos());

  return (this->getName() < other.getName());
}

bool ArrayAccess::operator==(ArrayAccess const other) const {
  return (this->getName() == other.getName());
}

bool ArrayAccess::operator!=(ArrayAccess const other) const {
  return (this->getName() != other.getName());
}

std::string ArrayAccess::getName() const { return this->array->getName(); }

Operand::ptr_t ArrayAccess::getPos() const { return this->pos; }

bool ArrayAccess::isLeaf() const { return true; }

std::string ArrayAccess::getValue() const {
  auto value = this->getName();
  value.append("[");
  value.append(this->getPos()->getValue());
  value.append("]");
  return value;
}
}
}