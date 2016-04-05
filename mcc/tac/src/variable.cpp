#include "mcc/tac/variable.h"

namespace mcc {
    namespace tac {

        Variable::Variable(Type t, std::string name) : 
        Operand(t), name(name), scope(0) {

        }

        bool Variable::isLeaf() const {
            return true;
        }

        std::string Variable::getName() const {
            return name;
        }

        std::string Variable::getValue() const {
            std::string value(this->getName());
            value.append(":");
            value.append(std::to_string(this->getScope()));
            
            return value;
        }

        int Variable::getScope() const {
            return this->scope;
        }

        void Variable::setScope(int scope) {
            this->scope = scope;
        }
    }
}