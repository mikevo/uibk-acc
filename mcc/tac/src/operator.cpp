#include <map>

#include "mcc/tac/operator.h"

#include <string>

namespace mcc {
  namespace tac {

    namespace {
      const std::map<OperatorName, OperatorType> operatorMap{
        {OperatorName::NOP, OperatorType::LINE},
        {OperatorName::ADD, OperatorType::BINARY},
        {OperatorName::ASSIGN, OperatorType::BINARY},
        {OperatorName::DIV, OperatorType::BINARY},
        {OperatorName::EQ, OperatorType::BINARY},
        {OperatorName::NE, OperatorType::BINARY},
        {OperatorName::LE, OperatorType::BINARY},
        {OperatorName::GE, OperatorType::BINARY},
        {OperatorName::LT, OperatorType::BINARY},
        {OperatorName::GT, OperatorType::BINARY},
        {OperatorName::JUMP, OperatorType::UNARY},
        {OperatorName::JUMPFALSE, OperatorType::BINARY},
        {OperatorName::LABEL, OperatorType::UNARY},
        {OperatorName::MUL, OperatorType::BINARY},
        {OperatorName::SUB, OperatorType::BINARY},
        {OperatorName::MINUS, OperatorType::UNARY}
      };

      const std::map<OperatorName, Type> operatorResultType{
        {OperatorName::NOP, Type::NONE},
        {OperatorName::ADD, Type::NONE},
        {OperatorName::ASSIGN, Type::NONE},
        {OperatorName::DIV, Type::NONE},
        {OperatorName::EQ, Type::BOOL},
        {OperatorName::NE, Type::BOOL},
        {OperatorName::LE, Type::BOOL},
        {OperatorName::GE, Type::BOOL},
        {OperatorName::LT, Type::BOOL},
        {OperatorName::GT, Type::BOOL},
        {OperatorName::JUMP, Type::BOOL},
        {OperatorName::JUMPFALSE, Type::BOOL},
        {OperatorName::LABEL, Type::BOOL},
        {OperatorName::MUL, Type::NONE},
        {OperatorName::SUB, Type::NONE},
        {OperatorName::MINUS, Type::NONE}
      };

      const std::map<OperatorName, std::string> operatorName{
        {OperatorName::NOP, ""},
        {OperatorName::ADD, " + "},
        {OperatorName::ASSIGN, " = "},
        {OperatorName::DIV, " / "},
        {OperatorName::EQ, " == "},
        {OperatorName::NE, " != "},
        {OperatorName::LE, " <= "},
        {OperatorName::GE, " >= "},
        {OperatorName::LT, " < "},
        {OperatorName::GT, " > "},
        {OperatorName::JUMP, "JUMP "},
        {OperatorName::JUMPFALSE, "JUMPFALSE "},
        {OperatorName::LABEL, "LABEL "},
        {OperatorName::MUL, " * "},
        {OperatorName::SUB, " - "},
        {OperatorName::MINUS, "-"}
      };
    }

    Operator::Operator(OperatorName name) :
    name(name),
    type(operatorMap.at(name)),
    resultType(operatorResultType.at(name)) {
    }

    OperatorName Operator::getName() const {
      return name;
    }

    OperatorType Operator::getType() const {
      return type;
    }

    Type Operator::getResultType() const {
      return resultType;
    }

    std::string Operator::toString() const {
      return operatorName.at(name);
    }
  }
}