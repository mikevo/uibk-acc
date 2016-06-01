/*
 * array.cpp
 *
 *  Created on: Jun 1, 2016
 */

#include "mcc/tac/array.h"

#include <string>

namespace mcc {
namespace tac {
Array::Array(Type t, std::string name, std::size_t size)
    : Operand(t), name(name), size(size) {}

bool Array::operator<(Array const other) const {
  return (this->getName() < other.getName());
}

bool Array::operator==(Array const other) const {
  return (this->getName() == other.getName());
}

bool Array::operator!=(Array const other) const {
  return (this->getName() != other.getName());
}

std::string Array::getName() const { return this->name; }

std::size_t Array::length() const { return this->size; }

bool Array::isLeaf() const { return true; }

std::string Array::getValue() const {
  auto value = this->getName();
  value.append("[");
  value.append(std::to_string(this->length()));
  value.append("]");
  return value;
}
}
}