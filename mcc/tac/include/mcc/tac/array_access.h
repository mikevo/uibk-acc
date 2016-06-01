/*
 * array_access.h
 *
 *  Created on: Jun 1, 2016
 */

#ifndef INCLUDES_MCC_TAC_ARRAY_ACCESS_H_
#define INCLUDES_MCC_TAC_ARRAY_ACCESS_H_

#include "mcc/tac/array.h"
#include "mcc/tac/variable.h"

namespace mcc {
namespace tac {
class ArrayAccess : public Variable {
 public:
  typedef std::shared_ptr<ArrayAccess> ptr_t;

  struct less {
    bool operator()(const ptr_t &lhs, const ptr_t &rhs) const {
      return *lhs.get() < *rhs.get();
    }
  };

  typedef std::set<ptr_t, less> set_t;

  ArrayAccess(Array::ptr_t array, Operand::ptr_t pos);

  bool operator<(ArrayAccess const other) const;
  bool operator==(ArrayAccess const other) const;
  bool operator!=(ArrayAccess const other) const;

  virtual std::string getName() const override;
  virtual bool isLeaf() const override;
  virtual std::string getValue() const override;

  Operand::ptr_t getPos() const;

 private:
  Array::ptr_t array;
  Operand::ptr_t pos;
};
}
}

#endif /* INCLUDES_MCC_TAC_ARRAY_ACCESS_H_ */
