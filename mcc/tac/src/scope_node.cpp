/*
 * scope_node.cpp
 *
 *  Created on: Apr 21, 2016
 */

#include "mcc/tac/scope_node.h"

namespace mcc {
  namespace tac {

    ScopeNode::ScopeNode(unsigned depth, unsigned index) :
        scopeDepth(depth), scopeIndex(index) {

    }

    unsigned ScopeNode::getDepth() const {
      return this->scopeDepth;
    }
    unsigned ScopeNode::getIndex() const {
      return this->scopeIndex;
    }
  }
}

