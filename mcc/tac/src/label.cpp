#include "mcc/tac/label.h"

#include <cstddef>

namespace mcc {
  namespace tac {

    Label::Label() : Triple(OperatorName::LABEL) {
      this->setName("$L" + std::to_string(getId()));
    }
    
    bool Label::isLeaf() const {
      return true;
    }
  }
}