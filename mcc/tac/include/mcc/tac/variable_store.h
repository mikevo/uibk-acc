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
#include <string>
#include <utility>
#include <vector>

#include "mcc/tac/scope.h"
#include "mcc/tac/scope_node.h"
#include "mcc/tac/variable.h"

namespace mcc {
namespace tac {
class VariableStore : public Scope {
  typedef std::shared_ptr<Variable> VariableNode;
  typedef std::set<VariableNode> VariableNodeSet;
  typedef VariableNode const const_shared_ptr;
  typedef std::vector<VariableNodeSet::iterator>::size_type size_type;
  typedef std::map<VariableNode,
                   std::vector<VariableNodeSet::iterator>>::const_iterator
      const_iterator;
  typedef VariableNodeSet::const_iterator set_const_iterator;

 public:
  const_shared_ptr operator[](size_type const pos) const;
  size_type size() const;

  set_const_iterator begin() const;
  set_const_iterator end() const;

  VariableNode findAccordingVariable(std::string const name);
  VariableNode findVariable(std::string const name);

  void addVariable(VariableNode variable);
  bool removeVariable(VariableNode const variable);
  VariableNode renameVariable(VariableNode const variable);

 private:
  const_iterator find(VariableNode const variable) const;

  VariableNodeSet::iterator insertVariable(VariableNode variable);

  VariableNodeSet store;
  std::vector<VariableNodeSet::iterator> variableTable;
  std::map<VariableNode, std::vector<VariableNodeSet::iterator>> renameMap;
};
}
}

#endif /* INCLUDES_MCC_TAC_VARIABLE_STORE_H_ */
