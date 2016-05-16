#include "mcc/lvn/lvn.h"

#include <cassert>
#include <unordered_map>

#include <mcc/tac/helper/ast_converters.h>

using namespace mcc::tac;
using namespace mcc::lvn;

namespace mcc {
namespace lvn {

namespace {
bool isValidOperator(Operator op) {
  switch (op.getName()) {
    case OperatorName::ADD:
    case OperatorName::SUB:
    case OperatorName::MUL:
    case OperatorName::DIV:
      return true;

    default:
      return false;
  }
}
}

std::map<tempAssignKey, tempAssignValue> LVN::tempVarAssign;
Variable::set_t LVN::tempVarUsed;

void LVN::transform(Tac &tac) {
  auto basicBlocks = tac.getBasicBlockIndex();
  std::unordered_map<std::string, std::shared_ptr<Operand>> valueMap;
  unsigned currentPos = 0;

  for (auto block : *basicBlocks.get()) {
    valueMap.clear();
    LVN::tempVarAssign.clear();
    LVN::tempVarUsed.clear();

    for (auto triple : block->getBlockMembers()) {
      ++currentPos;
      if (triple->getOperator().getType() == OperatorType::BINARY &&
          triple->getOperator().getName() != OperatorName::JUMPFALSE) {
        std::string valueKey = triple->getArg1()->getValue();
        valueKey.append(triple->getOperator().toString());
        valueKey.append(triple->getArg2()->getValue());

        auto value = valueMap.find(valueKey);

        if (value != valueMap.end()) {
          auto target = triple->getTargetVariable();
          if (helper::isType<Variable>(value->second)) {
            auto var = std::static_pointer_cast<Variable>(value->second);
            LVN::tempVarUsed.insert(var);
          }
          updateTriple(Operator(OperatorName::ASSIGN), target, value->second,
                       *triple);

        } else if (isValidOperator(triple->getOperator())) {
          if (helper::isType<IntLiteral>(triple->getArg1()) &&
              helper::isType<IntLiteral>(triple->getArg2())) {
            auto result = evaluateInt(*triple);
            auto target = triple->getTargetVariable();
            valueMap.insert(std::make_pair(valueKey, result));
            updateTriple(Operator(OperatorName::ASSIGN), target, result,
                         *triple);

          } else if (helper::isType<FloatLiteral>(triple->getArg1()) &&
                     helper::isType<FloatLiteral>(triple->getArg2())) {
            auto result = evaluateFloat(*triple);
            auto target = triple->getTargetVariable();
            valueMap.insert(std::make_pair(valueKey, result));
            updateTriple(Operator(OperatorName::ASSIGN), target, result,
                         *triple);

          } else {
            auto target = addTempVar(currentPos, triple->getTargetVariable());
            valueMap.insert(std::make_pair(valueKey, target));
          }
        }
      }
    }

    updateTac(tac);
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
      // FIXME: there are additional operations!
      assert(false && "Unsupported binary operation");
  }
}

void LVN::updateTriple(Operator op, Operand::ptr_t arg1, Operand::ptr_t arg2,
                       Triple &triple) {
  triple.setArg1(arg1);
  triple.setArg2(arg2);
  triple.setOperator(op);
  triple.updateResultType(op);
}

IntLiteral::ptr_t LVN::evaluateInt(Triple &triple) {
  auto arg1 = std::static_pointer_cast<IntLiteral>(triple.getArg1());
  auto arg2 = std::static_pointer_cast<IntLiteral>(triple.getArg2());

  int val1 = arg1->value;
  int val2 = arg2->value;
  int result = evaluateExpression(val1, val2, triple.getOperator().getName());

  return std::make_shared<IntLiteral>(result);
}

FloatLiteral::ptr_t LVN::evaluateFloat(Triple &triple) {
  auto arg1 = std::static_pointer_cast<FloatLiteral>(triple.getArg1());
  auto arg2 = std::static_pointer_cast<FloatLiteral>(triple.getArg2());

  float val1 = arg1->value;
  float val2 = arg2->value;
  float result = evaluateExpression(val1, val2, triple.getOperator().getName());

  return std::make_shared<FloatLiteral>(result);
}

void LVN::addTempVarAssignment(Variable::ptr_t keyVar, tempAssignValue keyValue,
                               Tac &tac, unsigned offset) {
  auto triple = std::make_shared<Triple>(Operator(OperatorName::ASSIGN), keyVar,
                                         keyValue.first);
  triple->setTargetVariable(keyVar);
  tac.addLine(triple, keyValue.second + offset);
  tac.getVariableStore()->addVariable(keyVar);
}

Variable::ptr_t LVN::addTempVar(unsigned position, Variable::ptr_t var) {
  auto tempVar = std::make_shared<Variable>(var->getType());

  LVN::tempVarAssign.insert(
      std::make_pair(tempVar, std::make_pair(var, position)));

  return tempVar;
}

void LVN::updateTac(Tac &tac) {
  unsigned numInserted = 0;
  for (auto &varAssign : LVN::tempVarAssign) {
    auto var = LVN::tempVarUsed.find(varAssign.first);

    if (var != LVN::tempVarUsed.end()) {
      addTempVarAssignment(varAssign.first, varAssign.second, tac, numInserted);
      ++numInserted;
    }
  }

  tac.createBasicBlockIndex();
}
}
}
