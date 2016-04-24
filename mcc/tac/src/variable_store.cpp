/*
 * variable_store.cpp
 *
 *  Created on: Apr 22, 2016
 */

#include "mcc/tac/variable_store.h"

#include <cassert>
#include <iostream>

#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {

    VariableStore::const_shared_ptr VariableStore::operator[](
        VariableStore::size_type const pos) const {

      auto it = this->variableTable.at(pos);

      return *it;
    }

    VariableStore::size_type VariableStore::size() const {
      return this->variableTable.size();
    }

    VariableStore::set_const_iterator VariableStore::begin() const {
      return this->store.begin();
    }
    VariableStore::set_const_iterator VariableStore::end() const {
      return this->store.end();
    }

    VariableStore::const_iterator VariableStore::find(
        VariableStore::VariableNode const variable) const {

      // FIXME: bad workaround
      std::map<VariableNode, std::vector<VariableNodeSet::iterator>>::const_iterator mapEntry;

      // TODO: is this const& fine? i'd say yes cause line is only used in loop
      for (auto const& v : this->renameMap) {
        if (v.first->getValue() == variable->getValue()) {
          return this->renameMap.find(v.first);
        }
      }

      return this->renameMap.find(variable);
    }

    void VariableStore::addVariable(VariableStore::VariableNode variable) {
      auto it = this->insertVariable(variable);

      std::vector<VariableStore::VariableNodeSet::iterator> varVector;
      varVector.push_back(it);

      this->renameMap.insert(std::make_pair(variable, varVector));
    }

    bool VariableStore::removeVariable(
        VariableStore::VariableNode const variable) {
      auto var = renameMap.find(variable);

      if (var != renameMap.end()) {
        for (auto v : var->second) {
          for (auto it = this->variableTable.begin();
              it != this->variableTable.end(); ++it) {
            if (*it == v) {
              this->variableTable.erase(it);
              this->store.erase(v);
            }
          }
        }

        this->renameMap.erase(var->first);

        return true;
      }

      return false;
    }

    VariableStore::VariableNode VariableStore::renameVariable(
        VariableStore::VariableNode const variable) {

      assert(!variable->isTemporary() && "temp variables can't be renamed");

      // FIXME: bad workaround
      auto key = std::make_shared<Variable>(variable->getType(),
          variable->getName());
      key->setScope(variable->getScope());

      // TODO: is this const& fine? i'd say yes cause line is only used in loop
      for (auto const& v : this->renameMap) {
        if (v.first->getValue() == key->getValue()) {
          auto valuePair = this->renameMap.find(v.first);

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
        }
      }

      assert(false && "Variable to rename does not exist!");
      return variable;
    }

    VariableStore::VariableNode VariableStore::findAccordingVariable(
        std::string const name) {
      this->setCheckPoint();

      do {
        auto key = std::make_shared<Variable>(Type::AUTO, name);
        key->setScope(this->getCurrentScope());
        auto mapVar = this->find(key);

        if (mapVar != this->renameMap.end()) {
          this->goToCheckPoint();
          auto vector = mapVar->second;
          auto result = vector.back();
          return *result;
        }
      } while (this->goToParent());

      // Debugging output; this is only printed if something goes terribly
      // wrong
      std::cout << name << ":" << this->getCurrentScope()->getDepth() << ":"
          << this->getCurrentScope()->getIndex() << std::endl;
      assert(false && "Usage of undeclared variable");
    }

    VariableStore::VariableNode VariableStore::findVariable(
        std::string const name) {
      auto const key = std::make_shared<Variable>(Type::AUTO, name);
      key->setScope(this->getCurrentScope());

      for (auto const& v : this->renameMap) {
        if (v.first->getName() == key->getName()) {
          return v.first;
        }
      }

      // Debugging output; this is only printed if something goes terribly
      // wrong
      std::cout << name << ":" << this->getCurrentScope()->getDepth() << ":"
          << this->getCurrentScope()->getIndex() << std::endl;
      assert(false && "Usage of undeclared variable");
    }

    VariableStore::VariableNodeSet::iterator VariableStore::insertVariable(
        VariableStore::VariableNode variable) {
      auto it = this->store.insert(variable);
      this->variableTable.push_back(it.first);
      return it.first;
    }
  }
}
