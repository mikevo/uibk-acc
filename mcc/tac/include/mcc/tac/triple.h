/*
 * File:   triple.h
 *
 * Created on March 18, 2016, 11:02 PM
 */

#ifndef MCC_TAC_TRIPLE_H
#define MCC_TAC_TRIPLE_H

#include <string>
#include <memory>

#include "mcc/tac/operator.h"
#include "mcc/tac/operand.h"
#include "mcc/tac/variable.h"

namespace mcc {
namespace tac {

class Triple : public Operand {
 public:
  Triple(std::shared_ptr<Operand> arg);
  Triple(OperatorName op);
  Triple(Operator op, std::shared_ptr<Operand> arg);
  Triple(Operator op, std::shared_ptr<Operand> arg1,
         std::shared_ptr<Operand> arg2);
  std::string getName() const;
  unsigned getId() const;
  unsigned getBasicBlockId() const;
  void setBasicBlockId(unsigned blockId);
  virtual bool isLeaf() const override;
  virtual std::string getValue() const override;
  virtual std::string toString() const;
  void setTargetVariable(std::shared_ptr<Variable> var);
  std::shared_ptr<Variable> getTargetVariable() const;
  bool containsTargetVar(void) const;

  bool containsArg1() const;
  bool containsArg2() const;

  std::shared_ptr<Operand> getArg1() const;
  std::shared_ptr<Operand> getArg2() const;
  void setArg1(std::shared_ptr<Operand> arg);
  void setArg2(std::shared_ptr<Operand> arg);

  Operator getOperator() const;
  void setOperator(Operator);

 protected:
  void setName(const std::string name);

 private:
  Operator op;
  unsigned basicBlockId;
  std::shared_ptr<Operand> arg1, arg2;
  // if the triple produces a result than this variable is used to store
  // the result
  std::shared_ptr<Variable> targetVar;
  static unsigned nextId;
  unsigned id;
  // if the triple does not produce a result than this variable holds a
  // name like $L1 ore something like that
  std::string name;
};
}
}

#endif /* MCC_TAC_TRIPLE_H */
