#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/array.h"

namespace mcc {
namespace tac {

TEST(Array, Leaf) {
  Array a = Array(Type::INT, "a", 5);
  EXPECT_EQ(true, a.isLeaf());
}

TEST(Array, Type) {
  Array a = Array(Type::INT, "a", 5);
  EXPECT_EQ(Type::INT, a.getType());
}

TEST(Array, Value) {
  std::string name = "a";
  std::size_t size = 5;

  Array a = Array(Type::INT, name, size);

  auto value = name;
  value.append("[");
  value.append(std::to_string(size));
  value.append("]");

  EXPECT_EQ(value, a.getValue());
}

TEST(Array, Name) {
  Array a = Array(Type::INT, "a", 5);

  EXPECT_EQ("a", a.getName());
}

TEST(Array, Length) {
  std::size_t size = 5;

  Array a = Array(Type::INT, "a", size);

  EXPECT_EQ(size, a.length());
}
}
}
