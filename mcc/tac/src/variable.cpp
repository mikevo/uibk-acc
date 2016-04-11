#include "mcc/tac/variable.h"

namespace mcc {
    namespace tac {

        Variable::Variable(Type t, std::string name) : 
        Operand(t), name(name), scope(std::pair<unsigned, unsigned>(0,0)) {

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
            value.append(std::to_string(scope.first));
             value.append(":");
            value.append(std::to_string(scope.second));
            
            return value;
        }

        int Variable::getScope() const {
            return this->scope.first;
        }

        void Variable::setScope(std::pair<unsigned, unsigned> scope) {
            this->scope = scope;
        }
    }
}