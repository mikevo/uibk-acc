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

#include "mcc/tac/scope_manager.h"
#include "mcc/tac/scope.h"
#include "mcc/tac/variable.h"

namespace mcc {
namespace tac {
class VariableStore : public ScopeManager {
  typedef Variable::ptr_t const const_shared_ptr;
  typedef std::map<Variable::ptr_t, std::vector<Variable::set_t::iterator>,
                   Variable::less> VariableMap;

  typedef std::vector<Variable::set_t::iterator>::size_type size_type;
  typedef VariableMap::const_iterator const_iterator;
  typedef Variable::set_t::const_iterator set_const_iterator;

 public:
  typedef std::shared_ptr<VariableStore> ptr_t;

  const_shared_ptr operator[](size_type const pos) const;
  size_type size() const;

  set_const_iterator begin() const;
  set_const_iterator end() const;

  Variable::ptr_t findAccordingVariable(std::string const name);
  Variable::ptr_t findVariable(std::string const name);

  void addVariable(Variable::ptr_t variable);
  bool removeVariable(Variable::ptr_t const variable);
  Variable::ptr_t renameVariable(Variable::ptr_t const variable);

 private:
  const_iterator find(Variable::ptr_t const variable) const;

  Variable::set_t::iterator insertVariable(Variable::ptr_t variable);

  Variable::set_t store;
  std::vector<Variable::set_t::iterator> variableTable;
  VariableMap renameMap;
};
}
}

#endif /* INCLUDES_MCC_TAC_VARIABLE_STORE_H_ */
