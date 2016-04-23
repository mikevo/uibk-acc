/*
 * ast_converters.cpp
 *
 *  Created on: Apr 23, 2016
 */

#include "mcc/tac/helper/ast_converters.h"

#include <cassert>
#include <map>

#include "ast.h"
#include "mcc/tac/float_literal.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/label.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {
    namespace helper {
      static const std::map<ast::binary_operand, OperatorName> binaryOperatorMap {
          { ast::binary_operand::ADD, OperatorName::ADD }, {
              ast::binary_operand::SUB, OperatorName::SUB }, {
              ast::binary_operand::MUL, OperatorName::MUL }, {
              ast::binary_operand::DIV, OperatorName::DIV }, {
              ast::binary_operand::EQ, OperatorName::EQ }, {
              ast::binary_operand::NE, OperatorName::NE }, {
              ast::binary_operand::LE, OperatorName::LE }, {
              ast::binary_operand::GE, OperatorName::GE }, {
              ast::binary_operand::LT, OperatorName::LT }, {
              ast::binary_operand::GT, OperatorName::GT }, {
              ast::binary_operand::ASSIGN, OperatorName::ASSIGN } };

      static const std::map<ast::unary_operand, OperatorName> unaryOperatorMap {
          { ast::unary_operand::MINUS, OperatorName::MINUS }, {
              ast::unary_operand::NOT, OperatorName::NOT } };

      Type getType(ast::type& type) {
        if (typeid(type) == typeid(ast::int_type&)) return Type::INT;
        if (typeid(type) == typeid(ast::float_type&)) return Type::FLOAT;
        assert(false && "Unknown data type");
        return Type::NONE;
      }

      ReturnType convertNode(Tac *t, AstNode n) {
        if (isType<ast::int_literal>(n)) return convertIntLiteral(t, n);
        if (isType<ast::float_literal>(n)) return convertFloatLiteral(t, n);
        if (isType<ast::variable>(n)) return convertVariable(t, n);
        if (isType<ast::binary_operation>(n)) return convertBinaryOp(t, n);
        if (isType<ast::unary_operation>(n)) return convertUnaryOp(t, n);
        if (isType<ast::expr_stmt>(n)) return convertExprStmt(t, n);
        if (isType<ast::paren_expr>(n)) return convertParenStmt(t, n);
        if (isType<ast::compound_stmt>(n)) return convertCompoundStmt(t, n);
        if (isType<ast::decl_stmt>(n)) return convertDeclStmt(t, n);
        if (isType<ast::if_stmt>(n)) return convertIfStmt(t, n);
        if (isType<ast::while_stmt>(n)) return convertWhileStmt(t, n);

        // Debugging output; this is only printed if something goes terribly
        // wrong
        std::cout << typeid(*n.get()).name() << std::endl;
        assert(false && "Unknown node type");
        return nullptr;
      }

      ReturnType convertIntLiteral(Tac *t, AstNode n) {
        int v = std::static_pointer_cast<ast::int_literal>(n)->value;
        return std::make_shared<IntLiteral>(v);
      }

      ReturnType convertFloatLiteral(Tac *t, AstNode n) {
        float v = std::static_pointer_cast<ast::float_literal>(n)->value;
        return std::make_shared<FloatLiteral>(v);
      }

      ReturnType convertVariable(Tac *t, AstNode n) {
        auto v = std::static_pointer_cast<ast::variable>(n);
        return t->getVariableStore()->findAccordingVariable(v->name);
      }

      ReturnType convertBinaryOp(Tac *t, AstNode n) {
        auto v = std::static_pointer_cast<ast::binary_operation>(n);

        auto lhs = convertNode(t, v->lhs);
        auto rhs = convertNode(t, v->rhs);

        bool setTarVar = false;
        VarTableValue variable;

        if (*v->op.get() == ast::binary_operand::ASSIGN) {
          auto check = isType<Variable>(lhs);
          assert(check && "arg1 needs to be a variable for ASSIGN triples");

          auto var = std::static_pointer_cast<Variable>(lhs);
          variable = t->addVarRenaming(var);
          setTarVar = true;
          lhs = variable;
        }

        auto op = Operator(binaryOperatorMap.at(*v->op.get()));
        auto triple = std::make_shared<Triple>(op, lhs, rhs);

        // ensure that an assign has the same variable in arg1 and targetVar
        if (setTarVar) triple->setTargetVariable(variable);
        t->addToVarTable(triple->getTargetVariable());
        t->addLine(triple);

        return triple;
      }

      ReturnType convertUnaryOp(Tac *t, AstNode n) {
        auto v = std::static_pointer_cast<ast::unary_operation>(n);
        auto lhs = convertNode(t, v->sub);

        auto op = Operator(unaryOperatorMap.at(*v->op.get()));
        auto var = std::make_shared<Triple>(op, lhs);

        t->addToVarTable(var->getTargetVariable());
        t->addLine(var);

        return var;
      }

      ReturnType convertExprStmt(Tac *t, AstNode n) {
        auto v = std::static_pointer_cast<ast::expr_stmt>(n);
        return convertNode(t, v->sub);
      }

      ReturnType convertParenStmt(Tac *t, AstNode n) {
        auto v = std::static_pointer_cast<ast::paren_expr>(n);
        return convertNode(t, v->sub);
      }

      ReturnType convertCompoundStmt(Tac *t, AstNode n) {
        auto v = std::static_pointer_cast<ast::compound_stmt>(n);
        auto statements = v->statements;

        t->getVariableStore()->addNewChild();

        for (auto const& s : statements) {
          convertNode(t, s);
        }

        t->getVariableStore()->goToParent();

        return nullptr;
      }

      ReturnType convertDeclStmt(Tac *t, AstNode n) {
        auto v = std::static_pointer_cast<ast::decl_stmt>(n);
        auto tempVar = v->var;

        auto type = getType(*tempVar->var_type.get());
        auto variable = std::make_shared<Variable>(type, tempVar->name);
        variable->setScope(t->getVariableStore()->getCurrentScope());

        if (isAvailable(v->init_expr)) {
          auto initExpression = convertNode(t, v->init_expr);
          t->addToVarTable(variable);

          if (initExpression->isLeaf()) {
            auto op = Operator(OperatorName::ASSIGN);
            auto var = std::make_shared<Triple>(op, variable, initExpression);
            var->setTargetVariable(variable);

            t->addLine(var);

            return var;
          } else {
            if (isType<Triple>(initExpression)) {
              auto triple = std::static_pointer_cast<Triple>(initExpression);
              auto hasTarVar = triple->containsTargetVar();
              assert(hasTarVar && "should contain a variable");

              t->removeFromVarTable(triple->getTargetVariable());
              triple->setTargetVariable(variable);

              return triple;
            } else assert(false && "Unknown subtype");
          }
        }

        t->addToVarTable(variable);

        return variable;
      }

      ReturnType convertIfStmt(Tac *t, AstNode n) {
        auto stmt = std::static_pointer_cast<ast::if_stmt>(n);

        auto condition = convertNode(t, stmt->condition);

        auto falseLabel = std::make_shared<Label>();
        auto jfOp = Operator(OperatorName::JUMPFALSE);
        auto var = std::make_shared<Triple>(jfOp, condition, falseLabel);

        t->addLine(var);
        t->nextBasicBlock();

        convertNode(t, stmt->then_stmt);

        std::shared_ptr<Label> trueLabel;

        if (isAvailable(stmt->else_stmt)) {
          trueLabel = std::make_shared<Label>();
          auto jOp = Operator(OperatorName::JUMP);
          auto trueJump = std::make_shared<Triple>(jOp, trueLabel);
          t->addLine(trueJump);
        }

        t->nextBasicBlock();
        t->addLine(falseLabel);

        if (isAvailable(stmt->else_stmt)) {
          convertNode(t, stmt->else_stmt);
          t->nextBasicBlock();
          t->addLine(trueLabel);
        }

        return nullptr;
      }

      ReturnType convertWhileStmt(Tac *t, AstNode n) {
        auto whileStmt = std::static_pointer_cast<ast::while_stmt>(n);

        auto condition = convertNode(t, whileStmt->condition);

        auto againLable = std::make_shared<Label>();
        t->nextBasicBlock();
        t->addLine(againLable);

        auto exitLable = std::make_shared<Label>();
        auto jfOp = Operator(OperatorName::JUMPFALSE);
        auto whileJump = std::make_shared<Triple>(jfOp, condition, exitLable);
        t->addLine(whileJump);
        t->nextBasicBlock();

        auto stmt = convertNode(t, whileStmt->stmt);
        
        auto cond =  std::static_pointer_cast<Triple>(condition);
        
        if(isType<Variable>(cond->getArg1())) {
            
            auto asgnOp = Operator(OperatorName::ASSIGN);
            auto var =  std::static_pointer_cast<Variable>(cond->getArg1()); 
            auto assignVar = t->getVariableStore()->findAccordingVariable(var->name);
            
            if(var.get() != assignVar.get()) {
                  auto whileSSA = std::make_shared<Triple>(asgnOp, var, assignVar);
                  t->addLine(whileSSA);
            }
            
        }
        
        if(isType<Variable>(cond->getArg2())) {
            
            auto asgnOp = Operator(OperatorName::ASSIGN);
            auto var =  std::static_pointer_cast<Variable>(cond->getArg2()); 
            auto assignVar = t->getVariableStore()->findAccordingVariable(var->name);
            
            if(var.get() != assignVar.get()) {
                  auto whileSSA = std::make_shared<Triple>(asgnOp, var, assignVar);
                  t->addLine(whileSSA);
            }
            
        }

        auto jOp = Operator(OperatorName::JUMP);
        auto againJump = std::make_shared<Triple>(jOp, againLable);
        t->addLine(againJump);
        t->nextBasicBlock();
        t->addLine(exitLable);

        return nullptr;
      }
    }
  }
}
