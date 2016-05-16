#include "mcc/tac/label.h"

#include <cstddef>

namespace mcc {
namespace tac {

Label::Label() : Triple(OperatorName::LABEL), functionEntryLabel(false) {
  this->setName("$L" + std::to_string(this->getId()));
}

Label::Label(std::string functionName)
    : Triple(OperatorName::LABEL), functionEntryLabel(true) {
  this->setName(functionName);
}

bool Label::isLeaf() const { return true; }

bool Label::isFunctionEntry() const { return this->functionEntryLabel; }

std::ostream& Label::operator<<(std::ostream& os) const {
  os << this->getName();

  return os;
}

bool Label::operator<(Label const other) const {
  return this->getName() < other.getName();
}
}
}
