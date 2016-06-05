#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/int_literal.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/triple.h"

using namespace mcc::tac;

namespace mcc {
namespace tac {

TEST(Triple, ID_Test) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  IntLiteral::ptr_t j = std::make_shared<IntLiteral>(42, scope);
  Operator op = Operator(OperatorName::NOT);

  Triple t1 = Triple(op, i, scope);
  Triple t2 = Triple(op, j, scope);

  EXPECT_LT(0, t1.getId());
  EXPECT_EQ(t1.getId() + 1, t2.getId());
}

TEST(Triple, Leaf) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  Operator op = Operator(OperatorName::NOT);

  Triple t1 = Triple(op, i, scope);

  EXPECT_EQ(false, t1.isLeaf());
}

TEST(Triple, BBDefaultId) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  Operator op = Operator(OperatorName::NOT);

  Triple t = Triple(op, i, scope);

  EXPECT_EQ(0, t.getBasicBlockId());
}

TEST(Triple, Value) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  Operator op = Operator(OperatorName::NOT);

  Triple t = Triple(op, i, scope);

  auto id = t.getTargetVariable()->getId();

  EXPECT_EQ("$t" + std::to_string(id), t.getValue());
}

TEST(Triple, Type) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral::ptr_t i = std::make_shared<IntLiteral>(42, scope);
  Operator op = Operator(OperatorName::NOT);

  Triple t = Triple(op, i, scope);

  EXPECT_EQ(Type::INT, t.getType());
}
}
}
