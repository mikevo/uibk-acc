/*
 * File:   operand.h
 *
 * Created on March 19, 2016, 1:44 PM
 */

#ifndef MCC_TAC_OPERAND_H
#define MCC_TAC_OPERAND_H

#include <memory>
#include <string>
#include <set>

#include "mcc/tac/operator.h"
#include "mcc/tac/scope.h"
#include "mcc/tac/type.h"

namespace mcc {
namespace tac {

class Operand {
 public:
  typedef std::shared_ptr<Operand> ptr_t;

  Operand(Scope::ptr_t scope);
  Operand(Type t, Scope::ptr_t scope);
  virtual void updateResultType(Operator op);
  virtual Type getType() const;
  virtual void setType(Type t);
  virtual std::string getValue() const = 0;
  virtual bool isLeaf() const = 0;
  Scope::ptr_t const getScope() const;

 private:
  Type t;
  Scope::ptr_t scope;
};
}
}

#endif /* MCC_TAC_OPERAND_H */
