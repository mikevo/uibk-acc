/*
 * variable_store.h
 *
 *  Created on: Apr 22, 2016
 */

#ifndef INCLUDES_MCC_TAC_VARIABLE_STORE_H_
#define INCLUDES_MCC_TAC_VARIABLE_STORE_H_

#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "mcc/tac/scope_node.h"
#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {
    class VariableStore {
        typedef std::shared_ptr<Variable> VariableNode;
        typedef std::set<VariableNode> VariableNodeSet;
        typedef VariableNode const& const_reference;
        typedef std::vector<VariableNodeSet::iterator>::size_type size_type;

      public:
        const_reference operator[](size_type pos) const;
        size_type size() const;

        void addVariable(VariableNode const& variable);
        bool removeVariable(VariableNode const& variable);
        VariableNode const& renameVariable(VariableNode const& variable);

      private:
        VariableNodeSet::iterator insertVariable(
            VariableNode const& variable);

        VariableNodeSet store;
        std::vector<VariableNodeSet::iterator> variableTable;
        std::map<VariableNode, std::vector<VariableNodeSet::iterator>> renameMap;
    };
  }
}

#endif /* INCLUDES_MCC_TAC_VARIABLE_STORE_H_ */
