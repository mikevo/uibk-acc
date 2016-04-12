#include "mcc/tac/tac.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <ostream>
#include <typeinfo>

#include "ast.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/float_literal.h"
#include "mcc/tac/variable.h"
#include "mcc/tac/label.h"

namespace mcc {
    namespace tac {
        namespace {

            static const std::map<ast::binary_operand, OperatorName> binaryOperatorMap{
                { ast::binary_operand::ADD, OperatorName::ADD},
                {
                    ast::binary_operand::SUB, OperatorName::SUB
                },
                {
                    ast::binary_operand::MUL, OperatorName::MUL
                },
                {
                    ast::binary_operand::DIV, OperatorName::DIV
                },
                {
                    ast::binary_operand::EQ, OperatorName::EQ
                },
                {
                    ast::binary_operand::NE, OperatorName::NE
                },
                {
                    ast::binary_operand::LE, OperatorName::LE
                },
                {
                    ast::binary_operand::GE, OperatorName::GE
                },
                {
                    ast::binary_operand::LT, OperatorName::LT
                },
                {
                    ast::binary_operand::GT, OperatorName::GT
                },
                {
                    ast::binary_operand::ASSIGN, OperatorName::ASSIGN
                }
            };

            static const std::map<ast::unary_operand, OperatorName> unaryOperatorMap{
                { ast::unary_operand::MINUS, OperatorName::MINUS},
                {
                    ast::unary_operand::NOT, OperatorName::NOT
                }
            };

            Type convertType(ast::type& type) {
                if (typeid (type) == typeid (ast::int_type&)) {
                    return Type::INT;
                }

                if (typeid (type) == typeid (ast::float_type&)) {
                    return Type::FLOAT;
                }

                assert(false && "Unknown data type");
                return Type::NONE;
            }

            std::shared_ptr<Operand> convertNode(Tac *tac,
                    std::shared_ptr<ast::node> n) {
                if (typeid (*n.get()) == typeid (ast::int_literal)) {
                    int v = std::static_pointer_cast<ast::int_literal>(n).get()->value;

                    return std::make_shared<IntLiteral>(v);
                }

                if (typeid (*n.get()) == typeid (ast::float_literal)) {
                    float v = std::static_pointer_cast<ast::float_literal>(n).get()->value;

                    return std::make_shared<FloatLiteral>(v);
                }

                if (typeid (*n.get()) == typeid (ast::variable)) {

                    auto v = std::static_pointer_cast<ast::variable>(n);
                    unsigned tempScopeDepth = tac->getCurrentScope().first;
                    unsigned tempScopeIndex = tac->getCurrentScope().second;

                    while (tac->getCurrentScope().first != 0) {

                        auto key = std::make_pair(v->name, std::make_pair(tac->getCurrentScope().first, tac->getCurrentScope().second));
                        auto mapVar = tac->getVarTable().find(key);

                        if (mapVar != tac->getVarTable().end()) {
                            tac->setScope(tempScopeDepth, tempScopeIndex, 0);
                            return mapVar->second.back();
                        }

                        tac->leaveScope();
                    }

                    auto key = std::make_pair(v->name, std::make_pair(0, 0));
                    auto mapVar = tac->getVarTable().find(key);

                    if (mapVar != tac->getVarTable().end()) {
                        tac->setScope(tempScopeDepth, tempScopeIndex, 0);
                        return mapVar->second.back();
                    }

                    std::cout << v->name << ":" << tac->getCurrentScope().first << ":" << tac->getCurrentScope().second << std::endl;
                    assert(false && "Usage of undeclared variable");
                }

                if (typeid (*n.get()) == typeid (ast::binary_operation)) {
                    auto v = std::static_pointer_cast<ast::binary_operation>(n);

                    auto lhs = convertNode(tac, v.get()->lhs);
                    auto rhs = convertNode(tac, v.get()->rhs);

                    auto var = std::make_shared<Triple>(
                            Operator(binaryOperatorMap.at(*v.get()->op.get())), lhs, rhs);

                    tac->addLine(var);

                    return var;
                }

                if (typeid (*n.get()) == typeid (ast::unary_operation)) {
                    auto v = std::static_pointer_cast<ast::unary_operation>(n);
                    auto lhs = convertNode(tac, v.get()->sub);

                    auto var = std::make_shared<Triple>(
                            Operator(unaryOperatorMap.at(*v.get()->op.get())), lhs);

                    tac->addLine(var);

                    return var;
                }

                if (typeid (*n.get()) == typeid (ast::expr_stmt)) {
                    auto v = std::static_pointer_cast<ast::expr_stmt>(n);

                    return convertNode(tac, v.get()->sub);
                }

                if (typeid (*n.get()) == typeid (ast::paren_expr)) {
                    auto v = std::static_pointer_cast<ast::paren_expr>(n);
                    return convertNode(tac, v.get()->sub);
                }

                if (typeid (*n.get()) == typeid (ast::compound_stmt)) {
                    tac->enterScope();
                    auto v = std::static_pointer_cast<ast::compound_stmt>(n);
                    auto statements = v.get()->statements;

                    std::for_each(statements.begin(), statements.end(),
                            [&](std::shared_ptr<ast::node> const &s) {
                                convertNode(tac, s);
                            });

                    tac->leaveScope();
                    return nullptr;
                }

                if (typeid (*n.get()) == typeid (ast::decl_stmt)) {
                    auto v = std::static_pointer_cast<ast::decl_stmt>(n);
                    auto tempVar = v.get()->var;

                    auto lhs = std::make_shared<Variable>(
                            convertType(*tempVar.get()->var_type.get()), tempVar.get()->name);
                    lhs->setScope(tac->getCurrentScope());

                    auto initStmt = v.get()->init_expr;
                    if (initStmt != nullptr) {
                        auto rhs = convertNode(tac, v.get()->init_expr);
                        tac->addToVarTable(
                                std::make_pair(lhs->getName(), tac->getCurrentScope()), lhs);

                        if (rhs.get()->isLeaf()) {
                            auto var = std::make_shared<Triple>(
                                    Operator(OperatorName::ASSIGN), lhs, rhs);

                            tac->addLine(var);
                            return var;
                        } else {
                            // if rhs is an expression
                            if (typeid (*rhs.get()) == typeid (Triple)) {
                                auto var = std::static_pointer_cast<Triple>(rhs);
                                var->setName(lhs.get()->getValue());
                                var->setTargetVariable(lhs);
                                return var;
                            } else {
                                assert(false && "Unknown subtype");
                            }
                        }

                    } else {
                        tac->addToVarTable(
                                std::make_pair(lhs->getName(), tac->getCurrentScope()), lhs);
                    }

                    return lhs;
                }

                if (typeid (*n.get()) == typeid (ast::if_stmt)) {
                    auto stmt = std::static_pointer_cast<ast::if_stmt>(n);

                    auto condition = convertNode(tac, stmt.get()->condition);

                    auto falseLabel = std::make_shared<Label>();
                    auto var = std::make_shared<Triple>(Operator(OperatorName::JUMPFALSE),
                            condition, falseLabel);

                    tac->addLine(var);
                    tac->nextBasicBlock();

                    convertNode(tac, stmt.get()->then_stmt);

                    std::shared_ptr<Label> trueLabel;

                    if (stmt->else_stmt != nullptr) {
                        trueLabel = std::make_shared<Label>();
                        auto trueJump = std::make_shared<Triple>(
                                Operator(OperatorName::JUMP), trueLabel);
                        tac->addLine(trueJump);
                    }

                    tac->nextBasicBlock();
                    tac->addLine(falseLabel);

                    if (stmt->else_stmt != nullptr) {
                        convertNode(tac, stmt.get()->else_stmt);
                        tac->nextBasicBlock();
                        tac->addLine(trueLabel);
                    }

                    return nullptr;
                }

                std::cout << typeid (*n.get()).name() << std::endl;
                assert(false && "Unknown node type");
                return nullptr;
            }
        }

        Tac::Tac() :
        currentBasicBlock(0),
        lastScopeDepth(0),
        scopeDepth(0),
        scopeIndex(0) {
        }

        void Tac::convertAst(std::shared_ptr<ast::node> n) {
            convertNode(this, n);
        }

        void Tac::addLine(std::shared_ptr<Triple> line) {
            line->setBasicBlockId(currentBasicBlock);
            this->codeLines.push_back(line);
        }

        void Tac::addLine(std::shared_ptr<Label> line) {
            line->setBasicBlockId(currentBasicBlock);
            this->codeLines.push_back(line);
        }

        void Tac::nextBasicBlock() {
            ++currentBasicBlock;
        }

        void Tac::enterScope() {
            ++scopeDepth;
            scopeIndex = scopeIndexMap[scopeDepth];
     
            if (scopeDepth == lastScopeDepth) {
                ++scopeIndex;
                scopeIndexMap[scopeDepth] = scopeIndex;
            }

        }

        void Tac::leaveScope() {
            lastScopeDepth = scopeDepth;
            --scopeDepth;
            scopeIndex = scopeIndexMap[scopeDepth];
           
        }
        
        void Tac::setScope(unsigned depth, unsigned index, unsigned lastDepth) {
            scopeDepth = depth;
            scopeIndex = index;
            lastScopeDepth = lastDepth;
        }

        void Tac::createBasicBlockIndex() {
            basicBlockIndex.clear();

            auto currentBasicBlock = std::make_shared<BasicBlock>(0);
            auto currentBasicBlockId = codeLines.front().get()->basicBlockId;

            for (auto& triple : codeLines) {
                if (triple.get()->basicBlockId != currentBasicBlockId) {
                    basicBlockIndex.push_back(currentBasicBlock);
                    currentBasicBlock = std::make_shared<BasicBlock>(triple.get()->basicBlockId);
                    currentBasicBlockId = triple.get()->basicBlockId;
                }

                currentBasicBlock.get()->push_back(triple);
            }

            // Add last block
            basicBlockIndex.push_back(currentBasicBlock);
        }

        std::string Tac::toString() const {
            std::string output;

            unsigned count = 0;

            for (auto &line : codeLines) {
                output.append(line.get()->toString());

                if (codeLines.size() > 1 && count < codeLines.size() - 1) {
                    output.append("\n");
                }

                ++count;
            }

            return output;
        }

        unsigned Tac::basicBlockCount() {
            return basicBlockIndex.size();
        }

        const std::vector<std::shared_ptr<BasicBlock>>&Tac::getBasicBlockIndex() const {
            return basicBlockIndex;
        }

        const std::map<VarTableKey, std::vector<VarTableValue>>& Tac::getVarTable() {
            return varTable;
        }

        void Tac::addToVarTable(VarTableKey key, VarTableValue value) {
            std::vector<VarTableValue> valueVec;
            valueVec.push_back(value);
            
            varTable.insert(std::make_pair(key, valueVec));
        }

        std::pair<unsigned, unsigned> Tac::getCurrentScope() {
            return std::make_pair(scopeDepth, scopeIndex);
        }
    }
}
