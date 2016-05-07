#include <map>

#include "mcc/tac/operator.h"

#include <string>

namespace mcc {
namespace tac {

namespace {
const std::map<OperatorName, OperatorType> operatorMap{
    {OperatorName::NOP, OperatorType::NONE},
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
    {OperatorName::LABEL, OperatorType::NONE},
    {OperatorName::MUL, OperatorType::BINARY},
    {OperatorName::SUB, OperatorType::BINARY},
    {OperatorName::MINUS, OperatorType::UNARY},
    {OperatorName::NOT, OperatorType::UNARY},
    {OperatorName::PUSH, OperatorType::UNARY},
    {OperatorName::POP, OperatorType::UNARY},
    {OperatorName::CALL, OperatorType::UNARY},
    {OperatorName::RET, OperatorType::RETURN}};

const std::map<OperatorName, Type> operatorResultType{
    {OperatorName::NOP, Type::AUTO},    {OperatorName::ADD, Type::AUTO},
    {OperatorName::ASSIGN, Type::AUTO}, {OperatorName::DIV, Type::FLOAT},
    {OperatorName::EQ, Type::BOOL},     {OperatorName::NE, Type::BOOL},
    {OperatorName::LE, Type::BOOL},     {OperatorName::GE, Type::BOOL},
    {OperatorName::LT, Type::BOOL},     {OperatorName::GT, Type::BOOL},
    {OperatorName::JUMP, Type::AUTO},   {OperatorName::JUMPFALSE, Type::AUTO},
    {OperatorName::LABEL, Type::AUTO},  {OperatorName::MUL, Type::AUTO},
    {OperatorName::SUB, Type::AUTO},    {OperatorName::MINUS, Type::AUTO},
    {OperatorName::PUSH, Type::AUTO},   {OperatorName::POP, Type::AUTO},
    {OperatorName::RET, Type::AUTO},    {OperatorName::CALL, Type::AUTO},  
    {OperatorName::NOT, Type::AUTO}};

const std::map<OperatorName, std::string> operatorName{
    {OperatorName::NOP, ""},         {OperatorName::ADD, " + "},
    {OperatorName::ASSIGN, " = "},   {OperatorName::DIV, " / "},
    {OperatorName::EQ, " == "},      {OperatorName::NE, " != "},
    {OperatorName::LE, " <= "},      {OperatorName::GE, " >= "},
    {OperatorName::LT, " < "},       {OperatorName::GT, " > "},
    {OperatorName::JUMP, "JUMP "},   {OperatorName::JUMPFALSE, "JUMPFALSE "},
    {OperatorName::LABEL, "LABEL "}, {OperatorName::MUL, " * "},
    {OperatorName::SUB, " - "},      {OperatorName::MINUS, "-"},
    {OperatorName::NOT, "!"}, {OperatorName::PUSH, "PUSH"},
    {OperatorName::POP, "POP"},  {OperatorName::CALL, "CALL"},
    {OperatorName::RET, "RET"}};
}

Operator::Operator(OperatorName name)
    : name(name),
      type(operatorMap.at(name)),
      resultType(operatorResultType.at(name)) {}

OperatorName Operator::getName() const { return name; }

OperatorType Operator::getType() const { return type; }

Type Operator::getResultType() const { return resultType; }

std::string Operator::toString() const { return operatorName.at(name); }
}
}
