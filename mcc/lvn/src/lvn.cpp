#include "mcc/lvn/lvn.h"

#include <cassert>
#include <unordered_map>

using namespace mcc::tac;
using namespace mcc::lvn;

namespace mcc {
    namespace lvn {

        std::map<Variable::ptr_t, Variable::ptr_t> LVN::tempVarAssign;
        std::map<Variable::ptr_t, unsigned> LVN::tempVarTacPos;
        Variable::set_t LVN::tempVarUsed;

        void LVN::transform(Tac &tac) {
            auto basicBlocks = tac.getBasicBlockIndex();
            std::unordered_map<std::string, std::shared_ptr < Operand>> valueMap;
            unsigned currentPos = 0;

            for (auto block : *basicBlocks.get()) {
                valueMap.clear();
                LVN::tempVarAssign.clear();
                LVN::tempVarTacPos.clear();

                for (auto triple : block->getBlockMembers()) {
                    ++currentPos;
                    if (triple->getOperator().getType() == OperatorType::BINARY) {
                        std::string valueKey = triple->getArg1()->getValue();
                        valueKey.append(triple->getOperator().toString());
                        valueKey.append(triple->getArg2()->getValue());

                        auto value = valueMap.find(valueKey);

                        if (value != valueMap.end()) {
                            auto target = triple->getTargetVariable();
                            if (typeid (*value->second.get()) == typeid (Variable)) {
                                auto var = std::static_pointer_cast<Variable>(value->second);
                                LVN::tempVarUsed.insert(var);
                            }
                            updateTriple(Operator(OperatorName::ASSIGN), target, value->second,
                                    *triple);

                        } else {
                            if (typeid (*triple->getArg1()) == typeid (IntLiteral) &&
                                    typeid (*triple->getArg2()) == typeid (IntLiteral)) {
                                auto result = evaluateInt(*triple);
                                auto target = triple->getTargetVariable();
                                valueMap.insert(std::make_pair(valueKey, result));
                                updateTriple(Operator(OperatorName::ASSIGN), target, result,
                                        *triple);


                            } else if (typeid (*triple->getArg1()) == typeid (FloatLiteral) &&
                                    typeid (*triple->getArg2()) == typeid (FloatLiteral)) {
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
                    assert(false && "Unsupported binary operation");
            }
        }

        void LVN::updateTriple(Operator op, Operand::ptr_t arg1,
                Operand::ptr_t arg2, Triple &triple) {
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

        bool LVN::addTempVarAssignment(Variable::ptr_t keyVar, Tac &tac, unsigned offset) {
            auto var = LVN::tempVarAssign.find(keyVar);
            if (var != LVN::tempVarAssign.end()) {
                auto tacPos = LVN::tempVarTacPos.find(keyVar);
                if (tacPos != LVN::tempVarTacPos.end()) {
                    auto triple = std::make_shared<Triple>(Operator(OperatorName::ASSIGN),
                            keyVar, var->second);
                    triple->setTargetVariable(keyVar);
                    tac.addLine(triple, tacPos->second + offset);
                    tac.getVariableStore()->addVariable(keyVar);
                    return true;
                }

                assert(false && "LVN tempVar without tac position!");

            }


            return false;
        }

        Variable::ptr_t LVN::addTempVar(unsigned position,
                Variable::ptr_t var) {
            auto tempVar = std::make_shared<Variable>(var->getType());

            LVN::tempVarAssign.insert(std::make_pair(tempVar, var));
            LVN::tempVarTacPos.insert(std::make_pair(tempVar, position));

            return tempVar;
        }

        void LVN::updateTac(Tac& tac) {
            unsigned numInserted = 0;
            for(auto var : LVN::tempVarUsed) {
                
                if(addTempVarAssignment(var, tac, numInserted)) {
                    ++numInserted;
                }
               
            }
            
            tac.createBasicBlockIndex();
        }

    }
}
