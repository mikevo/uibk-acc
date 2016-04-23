#include "mcc/lvn/lvn.h"

#include <cassert>
#include <unordered_map>
#include "mcc/tac/operator.h"
#include "mcc/tac/operand.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/float_literal.h"

using namespace mcc::tac;

namespace mcc {
    namespace lvn {

        void LVN::transform(Tac& tac) {

            //Evaluate all constant expressions
            for (auto& triple : tac.codeLines) {

                if (triple->getOperator().getType() == OperatorType::BINARY) {
                    if (triple->getOperator().getResultType() == Type::AUTO ||
                            triple->getOperator().getResultType() == Type::FLOAT) {
                        if (typeid (*triple->getArg1()) == typeid (IntLiteral) &&
                                typeid (*triple->getArg2()) == typeid (IntLiteral)) {

                            auto arg1 = std::static_pointer_cast<IntLiteral>(triple->getArg1());
                            auto arg2 = std::static_pointer_cast<IntLiteral>(triple->getArg2());
                            auto target = triple->getTargetVariable();

                            int val1 = arg1->value;
                            int val2 = arg2->value;
                            int result = evaluateExpression(val1, val2, triple->getOperator().getName());

                            if (target != nullptr) {
                                updateTriple(Operator(OperatorName::ASSIGN), target,
                                        std::make_shared<IntLiteral>(result), *triple);

                            } else {
                                updateTriple(Operator(OperatorName::ASSIGN), triple,
                                        std::make_shared<IntLiteral>(result), *triple);
                            }

                        } else if (typeid (*triple->getArg1()) == typeid (FloatLiteral) &&
                                typeid (*triple->getArg2()) == typeid (FloatLiteral)) {

                            auto arg1 = std::static_pointer_cast<FloatLiteral>(triple->getArg1());
                            auto arg2 = std::static_pointer_cast<FloatLiteral>(triple->getArg2());
                            auto target = triple->getTargetVariable();

                            float val1 = arg1->value;
                            float val2 = arg2->value;
                            float result = evaluateExpression(val1, val2, triple->getOperator().getName());

                            if (target != nullptr) {
                                updateTriple(Operator(OperatorName::ASSIGN), target,
                                        std::make_shared<FloatLiteral>(result), *triple);

                            } else {
                                updateTriple(Operator(OperatorName::ASSIGN), triple,
                                        std::make_shared<FloatLiteral>(result), *triple);
                            }

                        }

                    }

                }


            }

            //Eliminate redundant expressions
            auto basicBlocks = tac.getBasicBlockIndex();
            std::unordered_map<std::string, std::shared_ptr < Operand>> valueMap;

            for (auto block : *basicBlocks.get()) {
                valueMap.clear();

                for (auto& triple : block->getBlockMembers()) {
                    if (triple->getOperator().getType() == OperatorType::BINARY) {
                        std::string valueKey = triple->getArg1()->getValue();
                        valueKey.append(triple->getOperator().toString());
                        valueKey.append(triple->getArg2()->getValue());

                        auto value = valueMap.find(valueKey);

                        if (value != valueMap.end()) {
                            auto target = triple->getTargetVariable();

                            if (target != nullptr) {
                                updateTriple(Operator(OperatorName::ASSIGN), target,
                                        value->second, *triple);

                            } else {
                                updateTriple(Operator(OperatorName::ASSIGN), triple,
                                        value->second, *triple);
                            }

                        } else {
                            auto target = triple->getTargetVariable();
                            if (target != nullptr) {
                                valueMap.insert(std::make_pair(valueKey, target));
                            } else {
                                valueMap.insert(std::make_pair(valueKey, triple));
                            }

                        }


                    }
                }
            }
        }

        template<typename T >
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

            assert(false && "need new impl");
//            triple.arg1 = arg1;
//            triple.arg2 = arg2;
//            triple.op = op;
            triple.updateResultType(op);
            triple.setTargetVariable(nullptr);

        }

    }
}

