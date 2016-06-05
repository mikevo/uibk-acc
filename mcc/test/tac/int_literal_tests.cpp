#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/int_literal.h"

using namespace mcc::tac;

namespace mcc {
namespace tac {

TEST(IntLiteral, Leaf) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral i = IntLiteral(42, scope);
  EXPECT_EQ(true, i.isLeaf());
}

TEST(IntLiteral, Type) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  IntLiteral i = IntLiteral(42, scope);
  EXPECT_EQ(Type::INT, i.getType());
}

TEST(IntLiteral, Value) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  int testValue = 42;

  IntLiteral i = IntLiteral(testValue, scope);

  EXPECT_EQ(std::to_string(testValue), i.getValue());
}
}
}
