#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/int_literal.h"

namespace mcc {
namespace tac {

TEST(IntLiteral, Leaf) {
  IntLiteral i = IntLiteral(42);
  EXPECT_EQ(true, i.isLeaf());
}

TEST(IntLiteral, Type) {
  IntLiteral i = IntLiteral(42);
  EXPECT_EQ(Type::INT, i.getType());
}

TEST(IntLiteral, Value) {
  int testValue = 42;

  IntLiteral i = IntLiteral(testValue);

  EXPECT_EQ(std::to_string(testValue), i.getValue());
}
}
}
