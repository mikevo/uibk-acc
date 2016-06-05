/*
 * array.h
 *
 *  Created on: Jun 1, 2016
 */

#ifndef INCLUDES_MCC_TAC_ARRAY_H_
#define INCLUDES_MCC_TAC_ARRAY_H_

#include "mcc/tac/operand.h"
#include "variable.h"

namespace mcc {
namespace tac {
class Array : public Operand {
 public:
  typedef std::shared_ptr<Array> ptr_t;

  struct less {
    bool operator()(const ptr_t &lhs, const ptr_t &rhs) const {
      return *lhs.get() < *rhs.get();
    }
  };

  typedef std::set<ptr_t, less> set_t;

  Array(Type t, std::string name, Operand::ptr_t size, Scope::ptr_t scope);

  bool operator<(Array const other) const;
  bool operator==(Array const other) const;
  bool operator!=(Array const other) const;

  virtual std::string getName() const;
  virtual Operand::ptr_t length() const;
  virtual bool isLeaf() const override;
  virtual std::string getValue() const override;

 private:
  std::string name;
  Operand::ptr_t size;
};
}
}

#endif /* INCLUDES_MCC_TAC_ARRAY_H_ */
