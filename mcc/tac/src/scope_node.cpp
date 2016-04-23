/*
 * scope_node.cpp
 *
 *  Created on: Apr 21, 2016
 */

#include "mcc/tac/scope_node.h"

#include <utility>

namespace mcc {
  namespace tac {

    ScopeNode::ScopeNode(unsigned depth, unsigned index) :
        scopeDepth(depth), scopeIndex(index), nextIndex(0) {

    }

    bool ScopeNode::operator<(ScopeNode const other) const {
      auto pairLhs = std::make_pair(this->getDepth(), this->getIndex());
      auto pairRhs = std::make_pair(other.getDepth(), other.getIndex());

      return (pairLhs < pairRhs);
    }

    unsigned ScopeNode::getDepth() const {
      return this->scopeDepth;
    }
    unsigned ScopeNode::getIndex() const {
      return this->scopeIndex;
    }

    unsigned ScopeNode::getNextIndex() {
      return nextIndex++;
    }
  }
}

