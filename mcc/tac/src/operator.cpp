#include <map>

#include "mcc/tac/operator.h"

#include <string>

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
            
            const std::map<OperatorName, std::string> operatorName{
                {OperatorName::ADD, " + "},
                {OperatorName::ASSIGN, "ASSIGN "},
                {OperatorName::JUMP, "JUMP "},
                {OperatorName::LABLE, "LABLE "},
                {OperatorName::MUL, " * "},
                {OperatorName::SUB, " - "}
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
        
        std::string Operator::toString() const {
          return operatorName.at(name);
        }
    }
}