/*
 * scope_node.h
 *
 *  Created on: Apr 21, 2016
 *      Author: mike
 */

#ifndef INCLUDES_MCC_TAC_SCOPE_NODE_H_
#define INCLUDES_MCC_TAC_SCOPE_NODE_H_

namespace mcc {
  namespace tac {
    class ScopeNode {
      public:
        ScopeNode(unsigned depth, unsigned index);

        unsigned getDepth() const;
        unsigned getIndex() const;
        unsigned getNextIndex();

      private:
        unsigned scopeDepth;
        unsigned scopeIndex;
        unsigned nextIndex;
    };
  }
}

#endif /* INCLUDES_MCC_TAC_SCOPE_NODE_H_ */
