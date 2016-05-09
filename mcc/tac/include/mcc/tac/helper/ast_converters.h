/*
 * ast_converters.h
 *
 *  Created on: Apr 23, 2016
 */

#ifndef INCLUDES_MCC_TAC_HELPER_AST_CONVERTERS_H_
#define INCLUDES_MCC_TAC_HELPER_AST_CONVERTERS_H_

#include <memory>

#include "ast.h"
#include "mcc/tac/operand.h"
#include "mcc/tac/tac.h"

namespace mcc {
namespace tac {
namespace helper {
typedef std::shared_ptr<ast::node> AstNode;

Operand::ptr_t convertNode(Tac *t, AstNode n);

// base cases
Operand::ptr_t convertIntLiteral(Tac *t, AstNode n);
Operand::ptr_t convertFloatLiteral(Tac *t, AstNode n);
Operand::ptr_t convertVariable(Tac *t, AstNode n);

// step cases
Operand::ptr_t convertBinaryOp(Tac *t, AstNode n);
Operand::ptr_t convertUnaryOp(Tac *t, AstNode n);
Operand::ptr_t convertExprStmt(Tac *t, AstNode n);
Operand::ptr_t convertParenStmt(Tac *t, AstNode n);
Operand::ptr_t convertCompoundStmt(Tac *t, AstNode n);
Operand::ptr_t convertDeclStmt(Tac *t, AstNode n);
Operand::ptr_t convertIfStmt(Tac *t, AstNode n);
Operand::ptr_t convertWhileStmt(Tac *t, AstNode n);
Operand::ptr_t convertFunctionList(Tac *t, AstNode n);
Operand::ptr_t convertFunctionDef(Tac *t, AstNode n);
Operand::ptr_t convertfunctionCallExpr(Tac *t, AstNode n);
Operand::ptr_t convertReturnStmt(Tac *t, AstNode n);

template <typename T>
bool isType(const AstNode n) {
  return (typeid(*n.get()) == typeid(T));
}

template <typename T>
bool isType(const Operand::ptr_t n) {
  return (typeid(*n.get()) == typeid(T));
}

template <typename T>
bool isAvailable(const T v) {
  return (v != nullptr);
}
}
}
}

#endif /* INCLUDES_MCC_TAC_HELPER_AST_CONVERTERS_H_ */
