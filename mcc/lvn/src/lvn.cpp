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

                if (triple->op.getType() == OperatorType::BINARY) {

                    if (triple->op.getResultType() == Type::AUTO) {
                        if (typeid (*triple->arg1) == typeid (IntLiteral) &&
                                typeid (*triple->arg2) == typeid (IntLiteral)) {

                            auto arg1 = std::static_pointer_cast<IntLiteral>(triple->arg1);
                            auto arg2 = std::static_pointer_cast<IntLiteral>(triple->arg2);
                            auto target = triple->getTargetVariable();
                         
                            int val1 = arg1->value;
                            int val2 = arg2->value;
                            int result = evaluateExpression(val1, val2, triple->op.getName());
                            
                            if(target != nullptr) {
                            triple.reset(new Triple(Operator(OperatorName::ASSIGN), target,
                                    std::make_shared<IntLiteral>(result)));
                            } else {
                                 triple.reset(new Triple(Operator(OperatorName::ASSIGN), triple,
                                    std::make_shared<IntLiteral>(result)));
                            }

                        }
                        
                        else if (typeid (*triple->arg1) == typeid (FloatLiteral) &&
                                typeid (*triple->arg2) == typeid (FloatLiteral)) {

                            auto arg1 = std::static_pointer_cast<FloatLiteral>(triple->arg1);
                            auto arg2 = std::static_pointer_cast<FloatLiteral>(triple->arg2);
                            auto target = triple->getTargetVariable();

                            float val1 = arg1->value;
                            float val2 = arg2->value;
                            float result = evaluateExpression(val1, val2, triple->op.getName());
                            
                            if(target != nullptr) {
                            triple.reset(new Triple(Operator(OperatorName::ASSIGN), target,
                                    std::make_shared<FloatLiteral>(result)));
                            } else {
                                 triple.reset(new Triple(Operator(OperatorName::ASSIGN), triple,
                                    std::make_shared<FloatLiteral>(result)));
                            }

                        }

                    }

                }


            }
            
            if(tac.getBasicBlockIndex().size() == 0) {
               
            }
            
            auto& basicBlocks = tac.getBasicBlockIndex();
            std::unordered_map<std::string, std::shared_ptr<Operand>> valueMap;
            
            for(auto& block : basicBlocks) {
                valueMap.clear();
                for(auto& triple : block->getBlockMembers()) {
                    if (triple->op.getType() == OperatorType::BINARY) {
                        std::string valueKey = triple->arg1->getValue();
                        valueKey.append(triple->op.toString());
                        valueKey.append(triple->arg2->getValue());
                       
                        
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

    }
}

