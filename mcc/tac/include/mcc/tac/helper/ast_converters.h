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
      typedef std::shared_ptr<Operand> ReturnType;
      typedef std::shared_ptr<ast::node> AstNode;

      ReturnType convertNode(Tac *t, AstNode n);

      // base cases
      ReturnType convertIntLiteral(Tac *t, AstNode n);
      ReturnType convertFloatLiteral(Tac *t, AstNode n);
      ReturnType convertVariable(Tac *t, AstNode n);

      //step cases
      ReturnType convertBinaryOp(Tac *t, AstNode n);
      ReturnType convertUnaryOp(Tac *t, AstNode n);
      ReturnType convertExprStmt(Tac *t, AstNode n);
      ReturnType convertParenStmt(Tac *t, AstNode n);
      ReturnType convertCompoundStmt(Tac *t, AstNode n);
      ReturnType convertDeclStmt(Tac *t, AstNode n);
      ReturnType convertIfStmt(Tac *t, AstNode n);
      ReturnType convertWhileStmt(Tac *t, AstNode n);


      template<typename T>
      bool isType(const AstNode n) {
        return (typeid (*n.get()) == typeid(T));
      }

      template<typename T>
      bool isType(const ReturnType n) {
        return (typeid (*n.get()) == typeid(T));
      }

      template<typename T>
      bool isAvailable(const T v) {
        return (v != nullptr);
      }
    }
  }
}

#endif /* INCLUDES_MCC_TAC_HELPER_AST_CONVERTERS_H_ */
