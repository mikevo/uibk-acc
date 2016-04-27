/*
 * scope.cpp
 *
 *  Created on: Apr 21, 2016
 */

#include "mcc/tac/scope.h"

#include <utility>

namespace mcc {
namespace tac {

Scope::Scope(unsigned depth, unsigned index)
    : scopeDepth(depth), scopeIndex(index), nextIndex(0) {}

bool Scope::operator<(Scope const other) const {
  auto pairLhs = std::make_pair(this->getDepth(), this->getIndex());
  auto pairRhs = std::make_pair(other.getDepth(), other.getIndex());

  return (pairLhs < pairRhs);
}

unsigned Scope::getDepth() const { return this->scopeDepth; }
unsigned Scope::getIndex() const { return this->scopeIndex; }

unsigned Scope::getNextIndex() { return nextIndex++; }
}
}
