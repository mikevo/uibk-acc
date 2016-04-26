#include "mcc/lvn/lvn.h"

#include <cassert>
#include <unordered_map>

using namespace mcc::tac;

namespace mcc {
namespace lvn {
void LVN::transform(Tac& tac) {
  auto basicBlocks = tac.getBasicBlockIndex();
  std::unordered_map<std::string, std::shared_ptr<Operand>> valueMap;

  for (auto block : *basicBlocks.get()) {
    valueMap.clear();

    for (auto triple : block->getBlockMembers()) {
      if (triple->getOperator().getType() == OperatorType::BINARY) {
        std::string valueKey = triple->getArg1()->getValue();
        valueKey.append(triple->getOperator().toString());
        valueKey.append(triple->getArg2()->getValue());

        auto value = valueMap.find(valueKey);

        if (value != valueMap.end()) {
          auto target = triple->getTargetVariable();

          updateTriple(Operator(OperatorName::ASSIGN), target, value->second,
                       *triple);

        } else {
          if (typeid(*triple->getArg1()) == typeid(IntLiteral) &&
              typeid(*triple->getArg2()) == typeid(IntLiteral)) {
            auto result = evaluateInt(*triple);
            auto target = triple->getTargetVariable();
            valueMap.insert(std::make_pair(valueKey, result));
            updateTriple(Operator(OperatorName::ASSIGN), target, result,
                         *triple);
            std::cout << "bleeb" << std::endl;

          } else if (typeid(*triple->getArg1()) == typeid(FloatLiteral) &&
                     typeid(*triple->getArg2()) == typeid(FloatLiteral)) {
            auto result = evaluateFloat(*triple);
            auto target = triple->getTargetVariable();
            valueMap.insert(std::make_pair(valueKey, result));
            updateTriple(Operator(OperatorName::ASSIGN), target, result,
                         *triple);

          } else {
            auto target = triple->getTargetVariable();
            valueMap.insert(std::make_pair(valueKey, target));
          }
        }
      }
    }
  }
}

template <typename T>
T LVN::evaluateExpression(T arg1, T arg2, OperatorName op) {
  switch (op) {
    case OperatorName::ADD:
      return arg1 + arg2;

    case OperatorName::SUB:
      return arg1 - arg2;

    case OperatorName::MUL:
      return arg1 * arg2;

    case OperatorName::DIV:
      return arg1 / arg2;

    default:
      assert(false && "Unsupported binary operation");
  }
}

void LVN::updateTriple(Operator op, std::shared_ptr<Operand> arg1,
                       std::shared_ptr<Operand> arg2, Triple& triple) {
  triple.setArg1(arg1);
  triple.setArg2(arg2);
  triple.setOperator(op);
  triple.updateResultType(op);
}

std::shared_ptr<IntLiteral> LVN::evaluateInt(Triple& triple) {
  auto arg1 = std::static_pointer_cast<IntLiteral>(triple.getArg1());
  auto arg2 = std::static_pointer_cast<IntLiteral>(triple.getArg2());

  int val1 = arg1->value;
  int val2 = arg2->value;
  int result = evaluateExpression(val1, val2, triple.getOperator().getName());

  return std::make_shared<IntLiteral>(result);
}

std::shared_ptr<FloatLiteral> LVN::evaluateFloat(Triple& triple) {
  auto arg1 = std::static_pointer_cast<FloatLiteral>(triple.getArg1());
  auto arg2 = std::static_pointer_cast<FloatLiteral>(triple.getArg2());

  float val1 = arg1->value;
  float val2 = arg2->value;
  float result = evaluateExpression(val1, val2, triple.getOperator().getName());

  return std::make_shared<FloatLiteral>(result);
}
}
}
