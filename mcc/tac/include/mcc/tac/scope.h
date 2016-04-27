/*
 * scope.h
 *
 *  Created on: Apr 21, 2016
 */

#ifndef INCLUDES_MCC_TAC_SCOPE_H_
#define INCLUDES_MCC_TAC_SCOPE_H_

#include <memory>

namespace mcc {
namespace tac {
class Scope {
 public:
  typedef std::shared_ptr<Scope> ptr_t;

  Scope(unsigned depth, unsigned index);

  bool operator<(Scope const other) const;
  bool operator==(Scope const other) const;
  bool operator!=(Scope const other) const;

  unsigned getDepth() const;
  unsigned getIndex() const;
  unsigned getNextIndex();

 private:
  unsigned scopeDepth;
  unsigned scopeIndex;
  unsigned nextIndex;
};
}
}

#endif /* INCLUDES_MCC_TAC_SCOPE_H_ */
