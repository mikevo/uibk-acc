#include <map>

#include "mcc/tac/operator.h"

namespace mcc {
    namespace tac {

        namespace {
            const std::map<OperatorName, OperatorType> operatorMap{
                {OperatorName::ADD, OperatorType::BINARY},
                {OperatorName::ASSIGN, OperatorType::UNARY},
                {OperatorName::JUMP, OperatorType::UNARY},
                {OperatorName::LABLE, OperatorType::UNARY},
                {OperatorName::MUL, OperatorType::BINARY},
                {OperatorName::SUB, OperatorType::BINARY}
            };
        }

        Operator::Operator(OperatorName name) :
        name(name),
        type(operatorMap.at(name)) {
        }

        OperatorName Operator::getName() const {
            return name;
        }

        OperatorType Operator::getType() const {
            return type;
        }
    }
}