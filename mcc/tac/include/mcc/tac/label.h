/*
 * File:   triple.h
 *
 * Created on March 18, 2016, 11:02 PM
 */

#ifndef MCC_TAC_LABEL_H
#define MCC_TAC_LABEL_H

#include <ostream>
#include <string>

#include "mcc/tac/triple.h"

#include <memory>

namespace mcc {
namespace tac {

class Label : public Triple {
 public:
  typedef std::shared_ptr<Label> ptr_t;

  struct less {
    bool operator()(const ptr_t& lhs, const ptr_t& rhs) const {
      return *lhs.get() < *rhs.get();
    }
  };

  Label();
  Label(std::string functionName);
  virtual bool isLeaf() const override final;
  bool isFunctionEntry() const;

  std::ostream& operator<<(std::ostream& os) const;
  bool operator<(Label const other) const;

 private:
  bool functionEntryLabel;
};
}
}

#endif /* MCC_TAC_LABEL_H */
