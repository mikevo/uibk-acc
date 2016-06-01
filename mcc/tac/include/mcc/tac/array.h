/*
 * array.h
 *
 *  Created on: Jun 1, 2016
 */

#ifndef INCLUDES_MCC_TAC_ARRAY_H_
#define INCLUDES_MCC_TAC_ARRAY_H_

#include "mcc/tac/operand.h"

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

  Array(Type t, std::string name, std::size_t size);

  bool operator<(Array const other) const;
  bool operator==(Array const other) const;
  bool operator!=(Array const other) const;

  virtual std::string getName() const;
  virtual std::size_t length() const override;
  virtual bool isLeaf() const override;
  virtual std::string getValue() const override;

 private:
  std::string name;
  std::size_t size;
};
}
}

#endif /* INCLUDES_MCC_TAC_ARRAY_H_ */
