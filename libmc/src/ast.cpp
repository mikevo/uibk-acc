#include "ast.h"

#include <cassert>
#include <iostream>

namespace ast {

bool node::operator!=(const node& other) const { return !(*this == other); }

namespace {

template <typename T>
bool list_equal(const T& lhs, const T& rhs) {
  if (lhs.size() != rhs.size()) return false;
  for (size_t i = 0; i < lhs.size(); ++i) {
    if (*lhs[i] != *rhs[i]) return false;
  }
  return true;
}
}

bool operator==(const node_list& lhs, const node_list& rhs) {
  return list_equal(lhs, rhs);
}

bool operator==(const expr_list& lhs, const expr_list& rhs) {
  return list_equal(lhs, rhs);
}

bool operator==(const stmt_list& lhs, const stmt_list& rhs) {
  return list_equal(lhs, rhs);
}

bool operator==(const param_list& lhs, const param_list& rhs) {
  return list_equal(lhs, rhs);
}

bool int_type::operator==(const node& other) const {
  return typeid(other) == typeid(int_type);
}

std::ostream& int_type::print_to(std::ostream& stream) const {
  return stream << "int";
}

bool float_type::operator==(const node& other) const {
  return typeid(other) == typeid(float_type);
}

std::ostream& float_type::print_to(std::ostream& stream) const {
  return stream << "float";
}

bool int_literal::operator==(const node& other) const {
  return typeid(other) == typeid(int_literal) &&
         dynamic_cast<const int_literal&>(other).value == value;
}

std::ostream& int_literal::print_to(std::ostream& stream) const {
  return stream << value;
}

bool float_literal::operator==(const node& other) const {
  return typeid(other) == typeid(float_literal) &&
         dynamic_cast<const float_literal&>(other).value == value;
}

std::ostream& float_literal::print_to(std::ostream& stream) const {
  return stream << value;
}

bool variable::operator==(const node& other) const {
  if (typeid(other) != typeid(variable)) return false;
  auto o = dynamic_cast<const variable&>(other);
  return *o.var_type == *var_type && o.name == name;
}

std::ostream& variable::print_to(std::ostream& stream) const {
  return stream << name;
}

std::ostream& operator<<(std::ostream& stream, const binary_operand& op) {
  for (const auto& pair : binary_operand_map) {
    if (pair.second == op) return stream << pair.first;
  }
  assert(false && "Unsupported binary operand");
  return stream;
}

bool binary_operation::operator==(const node& other) const {
  if (typeid(other) != typeid(binary_operation)) return false;
  auto o = dynamic_cast<const binary_operation&>(other);
  return *o.op == *op && *o.lhs == *lhs && *o.rhs == *rhs;
}

std::ostream& binary_operation::print_to(std::ostream& stream) const {
  stream << "(";
  lhs->print_to(stream);
  stream << *op;
  rhs->print_to(stream);
  return stream << ")";
}

std::ostream& operator<<(std::ostream& stream, const unary_operand& op) {
  switch (op) {
    case unary_operand::MINUS:
      return stream << "-";
    case unary_operand::NOT:
      return stream << "!";
  };
  assert(false && "Unsupported unary operand");
  return stream;
}

bool unary_operation::operator==(const node& other) const {
  if (typeid(other) != typeid(unary_operation)) return false;
  auto o = dynamic_cast<const unary_operation&>(other);
  return *o.op == *op && *o.sub == *sub;
}

std::ostream& unary_operation::print_to(std::ostream& stream) const {
  stream << *op;
  sub->print_to(stream);
  return stream;
}

bool paren_expr::operator==(const node& other) const {
  if (typeid(other) != typeid(paren_expr)) return false;
  auto o = dynamic_cast<const paren_expr&>(other);
  return *o.sub == *sub;
}

std::ostream& paren_expr::print_to(std::ostream& stream) const {
  stream << "(";
  sub->print_to(stream);
  return stream << ")";
}

bool expr_stmt::operator==(const node& other) const {
  if (typeid(other) != typeid(expr_stmt)) return false;
  auto o = dynamic_cast<const expr_stmt&>(other);
  return *o.sub == *sub;
}

std::ostream& expr_stmt::print_to(std::ostream& stream) const {
  sub->print_to(stream);
  return stream << ";\n";
}

bool compound_stmt::operator==(const node& other) const {
  if (typeid(other) != typeid(compound_stmt)) return false;
  auto o = dynamic_cast<const compound_stmt&>(other);
  return statements == o.statements;
}

std::ostream& compound_stmt::print_to(std::ostream& stream) const {
  stream << "{\n";
  for (auto stmt : statements) {
    stmt->print_to(stream);
  }
  return stream << "}\n";
}

bool if_stmt::operator==(const node& other) const {
  if (typeid(other) != typeid(if_stmt)) return false;
  auto o = dynamic_cast<const if_stmt&>(other);
  return *o.condition == *condition && *o.then_stmt == *then_stmt &&
         *o.else_stmt == *else_stmt;
}

std::ostream& if_stmt::print_to(std::ostream& stream) const {
  stream << "if(";
  condition->print_to(stream);
  stream << ") ";
  then_stmt->print_to(stream);
  if (else_stmt) {
    stream << "else ";
    return else_stmt->print_to(stream);
  } else {
    return stream;
  }
}

bool decl_stmt::operator==(const node& other) const {
  if (typeid(other) != typeid(decl_stmt)) return false;
  auto o = dynamic_cast<const decl_stmt&>(other);
  return *o.var == *var && *o.init_expr == *init_expr;
}

std::ostream& decl_stmt::print_to(std::ostream& stream) const {
  var->var_type->print_to(stream);
  stream << " ";
  var->print_to(stream);
  if (init_expr) {
    stream << " = ";
    init_expr->print_to(stream);
  }
  return stream << ";\n";
}

bool while_stmt::operator==(const node& other) const {
  if (typeid(other) != typeid(while_stmt)) return false;
  auto o = dynamic_cast<const while_stmt&>(other);
  return *o.condition == *condition && *o.stmt == *stmt;
}

std::ostream& while_stmt::print_to(std::ostream& stream) const {
  stream << "while(";
  condition->print_to(stream);
  stream << ") ";
  stmt->print_to(stream);
  return stream;
}

bool return_stmt::operator==(const node& other) const {
  if (typeid(other) != typeid(return_stmt)) return false;
  auto o = dynamic_cast<const return_stmt&>(other);
  return *o.returnValue == *returnValue;
}

std::ostream& return_stmt::print_to(std::ostream& stream) const {
  if (returnValue != nullptr) {
    stream << "return ";
    returnValue->print_to(stream);
  } else {
    stream << "return";
  }

  stream << ";\n";

  return stream;
}

bool function_call_expr::operator==(const node& other) const {
  if (typeid(other) != typeid(function_call_expr)) return false;
  auto o = dynamic_cast<const function_call_expr&>(other);
  return o.arguments == arguments && *o.function == *function;
}

std::ostream& function_call_expr::print_to(std::ostream& stream) const {
  stream << function->name << "(";

  if (!arguments.empty()) {
    auto it = arguments.begin();
    while (it != std::prev(arguments.end())) {
      (*it)->print_to(stream);
      stream << ", ";
      ++it;
    }

    (*it)->print_to(stream);
  }

  stream << ")";
  return stream;
}

bool parameter::operator==(const node& other) const {
  if (typeid(other) != typeid(parameter)) return false;
  auto o = dynamic_cast<const parameter&>(other);
  return *o.paramVar == *paramVar;
}

std::ostream& parameter::print_to(std::ostream& stream) const {
  paramVar->var_type->print_to(stream);
  stream << " " << paramVar->name;
  return stream;
}

bool function_def::operator==(const node& other) const {
  if (typeid(other) != typeid(function_def)) return false;
  auto o = dynamic_cast<const function_def&>(other);
  return *o.body == *body && o.name == name && o.parameters == parameters &&
         *o.returnType == *returnType;
}

std::ostream& function_def::print_to(std::ostream& stream) const {
  if (returnType != nullptr) {
    returnType->print_to(stream);
  } else {
    stream << "void";
  }

  stream << " " << name << "(";

  if (!parameters.empty()) {
    auto it = parameters.begin();
    while (it != std::prev(parameters.end())) {
      (*it)->print_to(stream);
      stream << ", ";
      ++it;
    }

    (*it)->print_to(stream);
  }

  stream << ") ";
  body->print_to(stream);
  return stream;
}

bool function_prototype::operator==(const node& other) const {
  if (typeid(other) != typeid(function_prototype)) return false;
  auto o = dynamic_cast<const function_prototype&>(other);
  return o.name == name && o.parameters == parameters &&
         *o.returnType == *returnType;
}

std::ostream& function_prototype::print_to(std::ostream& stream) const {
  if (returnType != nullptr) {
    returnType->print_to(stream);
  } else {
    stream << "void";
  }

  stream << " " << name << "(";

  if (!parameters.empty()) {
    auto it = parameters.begin();
    while (it != std::prev(parameters.end())) {
      (*it)->print_to(stream);
      stream << ", ";
      ++it;
    }

    (*it)->print_to(stream);
  }

  stream << ");";
  return stream;
}

bool function_list::operator==(const node& other) const {
  if (typeid(other) != typeid(function_list())) return false;
  auto o = dynamic_cast<const function_list&>(other);
  return functions == o.functions;
}

std::ostream& function_list::print_to(std::ostream& stream) const {
  for (auto function : functions) {
    function->print_to(stream);
    stream << "\n";
  }

  return stream;
}
}

bool operator==(const sptr<ast::node>& lhs, const sptr<ast::node>& rhs) {
  return typeid(lhs) == typeid(rhs) && lhs && rhs && *lhs == *rhs;
}

std::ostream& operator<<(std::ostream& stream, const ast::node& node) {
  return node.print_to(stream);
}
