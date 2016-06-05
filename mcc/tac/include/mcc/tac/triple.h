/*
 * File:   triple.h
 *
 * Created on March 18, 2016, 11:02 PM
 */

#ifndef MCC_TAC_TRIPLE_H
#define MCC_TAC_TRIPLE_H

#include <memory>
#include <string>

#include "mcc/tac/operand.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/variable.h"

namespace mcc {
namespace tac {

class Triple : public Operand {
 public:
  typedef std::shared_ptr<Triple> ptr_t;

  Triple(Operand::ptr_t arg, Scope::ptr_t scope);
  Triple(OperatorName op, Scope::ptr_t scope);
  Triple(Operator op, Operand::ptr_t arg, Scope::ptr_t scope);
  Triple(Operator op, Operand::ptr_t arg1, Operand::ptr_t arg2,
         Scope::ptr_t scope);
  std::string getName() const;
  unsigned getId() const;
  unsigned getBasicBlockId() const;
  void setBasicBlockId(unsigned blockId);
  virtual bool isLeaf() const override;
  virtual std::string getValue() const override;
  virtual std::string toString() const;
  void setTargetVariable(Variable::ptr_t var);
  Variable::ptr_t getTargetVariable() const;
  bool containsTargetVar(void) const;

  bool containsArg1() const;
  bool containsArg2() const;

  Operand::ptr_t getArg1() const;
  Operand::ptr_t getArg2() const;
  void setArg1(Operand::ptr_t arg);
  void setArg2(Operand::ptr_t arg);

  Operator getOperator() const;
  void setOperator(Operator);

 protected:
  void setName(const std::string name);

 private:
  Operator op;
  unsigned basicBlockId;
  Operand::ptr_t arg1, arg2;
  // if the triple produces a result than this variable is used to store
  // the result
  Variable::ptr_t targetVar;
  static unsigned nextId;
  unsigned id;
  // if the triple does not produce a result than this variable holds a
  // name like $L1 ore something like that
  std::string name;
};
}
}

#endif /* MCC_TAC_TRIPLE_H */
