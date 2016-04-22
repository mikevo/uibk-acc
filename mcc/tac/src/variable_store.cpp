/*
 * variable_store.cpp
 *
 *  Created on: Apr 22, 2016
 */

#include "mcc/tac/variable_store.h"

#include <cassert>

#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {

    VariableStore::const_reference VariableStore::operator[](
        VariableStore::size_type pos) const {

      auto it = this->variableTable.at(pos);

      return *it;
    }

    VariableStore::size_type VariableStore::size() const {
      return this->variableTable.size();
    }

    void VariableStore::addVariable(
        VariableStore::VariableNode const& variable) {
      auto it = this->insertVariable(variable);

      std::vector<VariableStore::VariableNodeSet::iterator> varVector;
      varVector.push_back(it);

      this->renameMap.insert(std::make_pair(variable, varVector));
    }

    bool VariableStore::removeVariable(
        VariableStore::VariableNode const& variable) {
      auto var = renameMap.find(variable);

      if (var != renameMap.end()) {
        for (auto const& v : var->second) {
          for (auto it = this->variableTable.begin();
              it != this->variableTable.end(); ++it) {
            if (*it == v) {
              this->variableTable.erase(it);
            }
          }
        }

        this->renameMap.erase(var->first);

        return true;
      }

      return false;
    }

    VariableStore::VariableNode
    const& VariableStore::renameVariable(
        VariableStore::VariableNode const& variable) {

      assert(!variable->isTemporary() && "temp variables can't be renamed");

      auto valuePair = this->renameMap.find(variable);

      if (valuePair != renameMap.end()) {
        auto& varVector = valuePair->second;
        auto cloneVar = std::make_shared<Variable>(variable->getType(),
            variable->getName());
        cloneVar->setScope(variable->getScope());
        cloneVar->setIndex(varVector.size());
        auto it = this->insertVariable(cloneVar);
        varVector.push_back(it);

        return *it;
      }

      assert(false && "Variable to rename does not exist!");
      return variable;
    }

    VariableStore::VariableNodeSet::iterator VariableStore::insertVariable(
        VariableStore::VariableNode const& variable) {
      auto it = this->store.insert(variable);
      this->variableTable.push_back(it.first);
      return it.first;
    }
  }
}
