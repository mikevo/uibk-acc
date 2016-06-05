#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/array.h"
#include "mcc/tac/int_literal.h"

namespace mcc {
namespace tac {

TEST(Array, Leaf) {
  auto size = std::make_shared<IntLiteral>(5);
  Array a = Array(Type::INT, "a", size);
  EXPECT_EQ(true, a.isLeaf());
}

TEST(Array, Type) {
  auto size = std::make_shared<IntLiteral>(5);
  Array a = Array(Type::INT, "a", size);
  EXPECT_EQ(Type::INT, a.getType());
}

TEST(Array, Value) {
  std::string name = "a";
 auto size = std::make_shared<IntLiteral>(5);

  Array a = Array(Type::INT, name, size);

  auto value = name;
  value.append("[");
  value.append(size->getValue());
  value.append("]");

  EXPECT_EQ(value, a.getValue());
}

TEST(Array, Name) {
  auto size = std::make_shared<IntLiteral>(5);
  Array a = Array(Type::INT, "a", size);

  EXPECT_EQ("a", a.getName());
}

TEST(Array, Length) {
  auto size = std::make_shared<IntLiteral>(5);
  Array a = Array(Type::INT, "a", size);

  EXPECT_EQ(size, a.length());
}
}
}
