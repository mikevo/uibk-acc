/*
 * File:   variable.h
 * Author: Michael Voetter <Michael.Voetter@student.uibk.ac.at>
 *
 * Created on March 19, 2016, 2:50 PM
 */

#ifndef MCC_TAC_VARIABLE_LITERAL_H
#define MCC_TAC_VARIABLE_LITERAL_H

#include "mcc/tac/operand.h"

#include <memory>
#include <set>
#include <string>

namespace mcc {
namespace tac {
class Variable : public Operand {
 public:
  typedef std::shared_ptr<Variable> ptr_t;

  struct less {
    bool operator()(const ptr_t &lhs, const ptr_t &rhs) const {
      return *lhs.get() < *rhs.get();
    }
  };

  typedef std::set<ptr_t, less> set_t;

  // creates a temporary variable with scope (0,0)
  Variable(Type t, Scope::ptr_t scope);

  // creates a non-temporary variable with default index = 0
  Variable(Type t, std::string name, Scope::ptr_t scope, unsigned index = 0);

  bool operator<(Variable const other) const;

  unsigned getId() const;
  virtual std::string getName() const;
  std::string toString() const;
  std::string getNameWithIndex() const;
  virtual std::string getValue() const override;
  virtual bool isLeaf() const override;
  virtual Scope::ptr_t const getScope() const;
  //  void setIndex(unsigned);
  virtual bool isTemporary() const;
  void setTypeArgument();
  bool isArgument() const;
  virtual bool isArray() const;

 private:
  std::string name;
  unsigned index;
  static unsigned nextId;
  unsigned id;
  bool isArg;
};
}
}

#endif /* MCC_TAC_VARIABLE_LITERAL_H */
