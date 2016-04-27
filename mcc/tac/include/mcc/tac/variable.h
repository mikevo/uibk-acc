/*
 * File:   variable.h
 * Author: Michael Voetter <Michael.Voetter@student.uibk.ac.at>
 *
 * Created on March 19, 2016, 2:50 PM
 */

#ifndef MCC_TAC_VARIABLE_LITERAL_H
#define MCC_TAC_VARIABLE_LITERAL_H

#include "mcc/tac/operand.h"
#include "mcc/tac/scope.h"

#include <memory>
#include <set>
#include <string>

namespace mcc {
namespace tac {
class Variable : public Operand {
 public:
  typedef std::shared_ptr<Variable> Ptr;

  struct less {
    bool operator()(const Ptr &lhs,
                    const Ptr &rhs) const {
      return *lhs.get() < *rhs.get();
    }
  };

  typedef std::set<Ptr, Variable::less> Set;

  // creates a temporary variable with scope (0,0)
  Variable(Type t);

  // creates a non-temporary variable with default index = 0
  Variable(Type t, std::string name, std::shared_ptr<Scope> scope,
           unsigned index = 0);

  bool operator<(Variable const other) const;


  unsigned getId() const;
  virtual std::string getName() const;
  std::string getNameWithIndex() const;
  virtual std::string getValue() const override;
  virtual bool isLeaf() const override;
  std::shared_ptr<Scope> const getScope() const;
  //  void setIndex(unsigned);
  bool isTemporary() const;


 private:
  std::string name;
  std::shared_ptr<Scope> scope;
  unsigned index;
  static unsigned nextId;
  unsigned id;
};
}
}

#endif /* MCC_TAC_VARIABLE_LITERAL_H */
