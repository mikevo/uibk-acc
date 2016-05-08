#include "mcc/tac/label.h"

#include <cstddef>

namespace mcc {
namespace tac {

Label::Label() : Triple(OperatorName::LABEL) {
  this->setName("$L" + std::to_string(this->getId()));
}

Label::Label(std::string name) : Triple(OperatorName::LABEL) {
  this->setName(name);
}

bool Label::isLeaf() const { return true; }
}
}
