/*
 * File:   int_literal.h
 * Author: Michael Voetter <Michael.Voetter@student.uibk.ac.at>
 *
 * Created on March 19, 2016, 2:50 PM
 */

#ifndef MCC_TAC_VARIABLE_LITERAL_H
#define MCC_TAC_VARIABLE_LITERAL_H

#include "mcc/tac/operand.h"
#include "mcc/tac/scope.h"

#include <memory>
#include <string>

namespace mcc {
namespace tac {
class Variable : public Operand {
 public:
  Variable(Type t);
  Variable(Type t, std::string name);
  bool operator<(Variable const other) const;

  struct less {
    bool operator()(const std::shared_ptr<Variable> &lhs,
                    const std::shared_ptr<Variable> &rhs) const {
      return *lhs.get() < *rhs.get();
    }
  };

  unsigned getId() const;
  virtual std::string getName() const;
  std::string getNameWithIndex() const;
  virtual std::string getValue() const override;
  virtual bool isLeaf() const override;
  std::shared_ptr<Scope> const getScope() const;
  void setScope(std::shared_ptr<Scope> const scope);
  void setIndex(unsigned);
  bool isTemporary() const;

  std::string name;

 private:
  std::shared_ptr<Scope> scope;
  unsigned index;
  static unsigned nextId;
  unsigned id;
};
}
}

#endif /* MCC_TAC_VARIABLE_LITERAL_H */
