/*
 * ast_converters.cpp
 *
 *  Created on: Apr 23, 2016
 */

#include "mcc/tac/helper/ast_converters.h"

#include <cassert>
#include <map>

#include "ast.h"
#include "mcc/tac/array.h"
#include "mcc/tac/array_access.h"
#include "mcc/tac/float_literal.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/label.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/variable.h"

namespace mcc {
namespace tac {
namespace helper {
static const std::map<ast::binary_operand, OperatorName> binaryOperatorMap{
    {ast::binary_operand::ADD, OperatorName::ADD},
    {ast::binary_operand::SUB, OperatorName::SUB},
    {ast::binary_operand::MUL, OperatorName::MUL},
    {ast::binary_operand::DIV, OperatorName::DIV},
    {ast::binary_operand::EQ, OperatorName::EQ},
    {ast::binary_operand::NE, OperatorName::NE},
    {ast::binary_operand::LE, OperatorName::LE},
    {ast::binary_operand::GE, OperatorName::GE},
    {ast::binary_operand::LT, OperatorName::LT},
    {ast::binary_operand::GT, OperatorName::GT},
    {ast::binary_operand::ASSIGN, OperatorName::ASSIGN}};

static const std::map<ast::unary_operand, OperatorName> unaryOperatorMap{
    {ast::unary_operand::MINUS, OperatorName::MINUS},
    {ast::unary_operand::NOT, OperatorName::NOT}};

Type getType(ast::type &type) {
  if (typeid(type) == typeid(ast::int_type &)) return Type::INT;
  if (typeid(type) == typeid(ast::float_type &)) return Type::FLOAT;
  if (typeid(type) == typeid(ast::void_type &)) return Type::NONE;

  std::cout << type << std::endl;
  assert(false && "Unknown data type");
  return Type::NONE;
}

std::vector<Tac::type_size_type> convertArgList(ast::param_list argList) {
  std::vector<Tac::type_size_type> argv;

  for (auto arg : argList) {
    std::size_t size = arg->length;
    auto pair = std::make_pair(getType(*arg->paramVar->var_type), size);
    argv.push_back(pair);
  }

  return argv;
}

Operand::ptr_t convertNode(Tac *t, AstNode n) {
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
  if (isType<ast::function_list>(n)) return convertFunctionList(t, n);
  if (isType<ast::function_def>(n)) return convertFunctionDef(t, n);
  if (isType<ast::function_prototype>(n)) return convertFunctionPrototype(t, n);
  if (isType<ast::function_call_expr>(n)) return convertfunctionCallExpr(t, n);
  if (isType<ast::return_stmt>(n)) return convertReturnStmt(t, n);
  if (isType<ast::array_decl_stmt>(n)) return convertArrayDeclStmt(t, n);
  if (isType<ast::array_access>(n)) return convertArrayAccess(t, n);

  // Debugging output; this is only printed if something goes terribly
  // wrong
  auto &node = *n.get();
  std::cout << typeid(node).name() << std::endl;
  assert(false && "Unknown node type");
  return nullptr;
}

Operand::ptr_t convertIntLiteral(Tac *t, AstNode n) {
  int v = std::static_pointer_cast<ast::int_literal>(n)->value;
  return std::make_shared<IntLiteral>(v,
                                      t->getVariableStore()->getCurrentScope());
}

Operand::ptr_t convertFloatLiteral(Tac *t, AstNode n) {
  float v = std::static_pointer_cast<ast::float_literal>(n)->value;
  return std::make_shared<FloatLiteral>(
      v, t->getVariableStore()->getCurrentScope());
}

Operand::ptr_t convertVariable(Tac *t, AstNode n) {
  auto v = std::static_pointer_cast<ast::variable>(n);
  return t->getVariableStore()->findAccordingVariable(v->name);
}

Operand::ptr_t convertBinaryOp(Tac *t, AstNode n) {
  auto v = std::static_pointer_cast<ast::binary_operation>(n);

  auto lhs = convertNode(t, v->lhs);
  auto rhs = convertNode(t, v->rhs);

  bool setTarVar = false;
  Variable::ptr_t variable;

  if (*v->op.get() == ast::binary_operand::ASSIGN) {
    auto check = isType<Variable>(lhs) || isType<ArrayAccess>(lhs);
    assert(check && "arg1 needs to be a variable for ASSIGN triples");

    variable = std::static_pointer_cast<Variable>(lhs);

    setTarVar = true;
    lhs = variable;
  }

  auto op = Operator(binaryOperatorMap.at(*v->op.get()));
  auto triple = std::make_shared<Triple>(
      op, lhs, rhs, t->getVariableStore()->getCurrentScope());

  // ensure that an assign has the same variable in arg1 and targetVar
  if (setTarVar) triple->setTargetVariable(variable);
  t->addToVarTable(triple->getTargetVariable());
  t->addLine(triple);

  return triple;
}

Operand::ptr_t convertUnaryOp(Tac *t, AstNode n) {
  auto v = std::static_pointer_cast<ast::unary_operation>(n);
  auto lhs = convertNode(t, v->sub);

  auto op = Operator(unaryOperatorMap.at(*v->op.get()));
  auto var = std::make_shared<Triple>(op, lhs,
                                      t->getVariableStore()->getCurrentScope());

  t->addToVarTable(var->getTargetVariable());
  t->addLine(var);

  return var;
}

Operand::ptr_t convertExprStmt(Tac *t, AstNode n) {
  auto v = std::static_pointer_cast<ast::expr_stmt>(n);
  return convertNode(t, v->sub);
}

Operand::ptr_t convertParenStmt(Tac *t, AstNode n) {
  auto v = std::static_pointer_cast<ast::paren_expr>(n);
  return convertNode(t, v->sub);
}

Operand::ptr_t convertCompoundStmt(Tac *t, AstNode n) {
  auto v = std::static_pointer_cast<ast::compound_stmt>(n);
  auto statements = v->statements;

  t->getVariableStore()->addNewChildScope();

  for (auto const &s : statements) {
    convertNode(t, s);
  }

  t->getVariableStore()->goToParentScope();

  return nullptr;
}

Operand::ptr_t convertDeclStmt(Tac *t, AstNode n) {
  auto v = std::static_pointer_cast<ast::decl_stmt>(n);
  auto tempVar = v->var;

  auto type = getType(*tempVar->var_type);
  auto variable = std::make_shared<Variable>(
      type, tempVar->name, t->getVariableStore()->getCurrentScope());

  if (isAvailable(v->init_expr)) {
    auto initExpression = convertNode(t, v->init_expr);
    t->addToVarTable(variable);

    if (initExpression->isLeaf()) {
      auto op = Operator(OperatorName::ASSIGN);
      auto var =
          std::make_shared<Triple>(op, variable, initExpression,
                                   t->getVariableStore()->getCurrentScope());
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
      } else
        assert(false && "Unknown subtype");
    }
  }

  t->addToVarTable(variable);

  return variable;
}

Operand::ptr_t convertIfStmt(Tac *t, AstNode n) {
  auto stmt = std::static_pointer_cast<ast::if_stmt>(n);

  auto condition = convertNode(t, stmt->condition);

  auto falseLabel =
      std::make_shared<Label>(t->getVariableStore()->getCurrentScope());
  auto jfOp = Operator(OperatorName::JUMPFALSE);
  auto var = std::make_shared<Triple>(jfOp, condition, falseLabel,
                                      t->getVariableStore()->getCurrentScope());

  t->addLine(var);
  t->nextBasicBlock();

  convertNode(t, stmt->then_stmt);

  std::shared_ptr<Label> trueLabel;

  if (isAvailable(stmt->else_stmt)) {
    trueLabel =
        std::make_shared<Label>(t->getVariableStore()->getCurrentScope());
    auto jOp = Operator(OperatorName::JUMP);
    auto trueJump = std::make_shared<Triple>(
        jOp, trueLabel, t->getVariableStore()->getCurrentScope());
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

Operand::ptr_t convertWhileStmt(Tac *t, AstNode n) {
  auto whileStmt = std::static_pointer_cast<ast::while_stmt>(n);

  auto againLabel =
      std::make_shared<Label>(t->getVariableStore()->getCurrentScope());
  t->nextBasicBlock();
  t->addLine(againLabel);

  auto condition = convertNode(t, whileStmt->condition);

  auto exitLabel =
      std::make_shared<Label>(t->getVariableStore()->getCurrentScope());
  auto jfOp = Operator(OperatorName::JUMPFALSE);
  auto whileJump = std::make_shared<Triple>(
      jfOp, condition, exitLabel, t->getVariableStore()->getCurrentScope());
  t->addLine(whileJump);
  t->nextBasicBlock();

  convertNode(t, whileStmt->stmt);

  auto jOp = Operator(OperatorName::JUMP);
  auto againJump = std::make_shared<Triple>(
      jOp, againLabel, t->getVariableStore()->getCurrentScope());
  t->addLine(againJump);
  t->nextBasicBlock();
  t->addLine(exitLabel);

  return nullptr;
}

Operand::ptr_t convertFunctionList(Tac *t, AstNode n) {
  auto v = std::static_pointer_cast<ast::function_list>(n);

  for (auto fun : v->functions) {
    convertNode(t, fun);
  }

  return nullptr;
}

Operand::ptr_t convertFunctionPrototype(Tac *t, AstNode n) {
  auto proto = std::static_pointer_cast<ast::function_prototype>(n);
  auto dummyLabel = std::make_shared<Label>(
      proto->name, t->getVariableStore()->getCurrentScope());
  t->addFunction(proto->name, dummyLabel);
  t->addFunctionPrototype(proto->name, convertArgList(proto->parameters));
  return nullptr;
}

Operand::ptr_t convertFunctionDef(Tac *t, AstNode n) {
  auto function = std::static_pointer_cast<ast::function_def>(n);
  auto codelines = t->codeLines.size();

  // Add label as entry point to function
  t->nextBasicBlock();
  auto functionDef = t->lookupFunction(function->name);

  if (functionDef) {
    t->addLine(functionDef);
  } else {
    auto entryLabel = std::make_shared<Label>(
        function->name, t->getVariableStore()->getCurrentScope());
    t->addFunction(function->name, entryLabel);
    auto argList = convertArgList(function->parameters);
    t->addFunctionPrototype(function->name, argList);
    t->addLine(entryLabel);
  }

  t->getVariableStore()->addNewChildScope();

  // Declare and init parameters as variables
  for (auto param : function->parameters) {
    sptr<ast::expression> dummy;
    auto var = std::static_pointer_cast<Variable>(convertNode(
        t, std::make_shared<ast::decl_stmt>(param->paramVar, dummy)));

    var->setTypeArgument();
    t->addToVarTable(var);

    auto op = Operator(OperatorName::POP);
    auto varAssignment = std::make_shared<Triple>(
        op, var, t->getVariableStore()->getCurrentScope());
    varAssignment->setTargetVariable(var);
    t->addLine(varAssignment);
  }

  // Convert function body
  std::vector<Triple::ptr_t> retStatements;
  for (auto statement : function->body->statements) {
    convertNode(t, statement);
  }

  // Check for return value mismatches
  auto it = t->codeLines.begin() + codelines;
  while (it != t->codeLines.end()) {
    if ((*it)->getOperator().getType() == OperatorType::RETURN) {
      retStatements.push_back(*it);
    }

    ++it;
  }

  Type retType;
  if (function->returnType != nullptr) {
    retType = getType(*function->returnType);
  } else {
    retType = Type::NONE;
  }

  if (retType != Type::NONE && retStatements.size() == 0) {
    assert(false && "Non void function without return value");
  }

  for (auto retStmt : retStatements) {
    if (retStmt->getType() != retType) {
      assert(false && "Return value type mismatch");
    }
  }

  t->getVariableStore()->goToParentScope();
  return nullptr;
}

Operand::ptr_t convertfunctionCallExpr(Tac *t, AstNode n) {
  auto functionCall = std::static_pointer_cast<ast::function_call_expr>(n);

  if (functionCall->arguments.size() !=
      functionCall->function->parameters.size()) {
    assert(false &&
           "Number of call arguments differ from function parameter count");
  }

  ast::expr_list::reverse_iterator revIt = functionCall->arguments.rbegin();
  // ast::param_list::reverse_iterator revItParam =
  //     functionCall->function->parameters.rbegin();
  while (revIt != functionCall->arguments.rend()) {
    auto argValue = convertNode(t, *revIt);

    // TODO check if this assertion is necessary, if not delete it to allow
    // different datatypes as function parameters
    //    if (argValue->getType() !=
    //    getType(*(*revItParam)->paramVar->var_type)) {
    //      assert(false && "Argument type mismatch");
    //    }

    auto op = Operator(OperatorName::PUSH);
    auto argPush = std::make_shared<Triple>(
        op, argValue, t->getVariableStore()->getCurrentScope());
    t->addLine(argPush);

    ++revIt;
  }

  auto functionName = functionCall->function->name;
  auto functionEntry = t->lookupFunction(functionName);

  if (functionEntry == nullptr) {
    assert(false && "Call of undeclared function!");
  }

  Type type;
  type = getType(*functionCall->function->returnType);

  auto op = Operator(OperatorName::CALL, type);
  auto callTriple = std::make_shared<Triple>(
      op, functionEntry, t->getVariableStore()->getCurrentScope());
  callTriple->setType(type);

  t->addLine(callTriple);
  t->nextBasicBlock();

  return callTriple;
}

Operand::ptr_t convertReturnStmt(Tac *t, AstNode n) {
  auto ret = std::static_pointer_cast<ast::return_stmt>(n);
  if (ret->returnValue != nullptr) {
    auto returnValue = convertNode(t, ret->returnValue);
    auto op = Operator(OperatorName::RET);
    auto retTriple = std::make_shared<Triple>(
        op, returnValue, t->getVariableStore()->getCurrentScope());
    t->addLine(retTriple);
    t->addReturn();
    t->nextBasicBlock();
    return retTriple;

  } else {
    auto op = Operator(OperatorName::RET);
    auto retTriple = std::make_shared<Triple>(
        op, nullptr, t->getVariableStore()->getCurrentScope());
    retTriple->setType(Type::NONE);
    t->addLine(retTriple);
    t->addReturn();
    t->nextBasicBlock();
    return retTriple;
  }
}

Operand::ptr_t convertArrayDeclStmt(Tac *t, AstNode n) {
  auto a = std::static_pointer_cast<ast::array_decl_stmt>(n);
  auto array = convertArray(t, a->decl_array);
  t->addToArraySet(array);

  return array;
}

Operand::ptr_t convertArrayAccess(Tac *t, AstNode n) {
  auto a = std::static_pointer_cast<ast::array_access>(n);

  auto array = convertArray(t, a->m_array);
  auto pos = convertNode(t, a->access_expr);

  return std::make_shared<ArrayAccess>(
      array, pos, t->getVariableStore()->getCurrentScope());
}

Operand::ptr_t convertArray(Tac *t, AstNode n) {
  auto a = std::static_pointer_cast<ast::array>(n);
  return convertArray(t, a);
}

Array::ptr_t convertArray(Tac *t, std::shared_ptr<ast::array> a) {
  auto type = getType(*a->array_type);
  auto name = a->name;

  auto size = convertNode(t, a->array_size);
  if (size != nullptr) {
    return std::make_shared<Array>(type, name, size,
                                   t->getVariableStore()->getCurrentScope());
  }

  assert(false && "Could not evaluate array size expression");
  return nullptr;
}
}
}
}
