#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {
    unsigned Variable::nextId = 1;

    Variable::Variable(Type t) :
        Variable(t, "") {
      // necessary to do it after the called constructor has finished
      this->name = "$t" + std::to_string(this->id);
    }

    Variable::Variable(Type t, std::string name) :
        Operand(t), name(name), index(0) {
      this->id = ++Variable::nextId;

      // TODO: wrong scope info
      this->scope = std::make_shared<ScopeNode>(0, 0);
    }
    unsigned Variable::getId() const {
      return this->id;
    }

    bool Variable::isLeaf() const {
      return true;
    }

    std::string Variable::getName() const {
      return name;
    }

    std::string Variable::getValue() const {
      std::string value(this->getName());
      value.append(std::to_string(index));
      value.append(":");
      value.append(std::to_string(this->scope->getDepth()));
      value.append(":");
      value.append(std::to_string(this->scope->getIndex()));

      return value;
    }

    std::shared_ptr<ScopeNode> const& Variable::getScope() const {
      return this->scope;
    }

    void Variable::setScope(std::shared_ptr<ScopeNode> const& scope) {
      this->scope = scope;
    }

    void Variable::setIndex(unsigned index) {
      this->index = index;
    }

    unsigned Variable::getIndex() {
      return index;
    }

    bool Variable::isTemporary() const {
      return (getName().find("$t") != std::string::npos);
    }
  }
}
