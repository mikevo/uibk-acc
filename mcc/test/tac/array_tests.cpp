#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/array.h"
#include "mcc/tac/int_literal.h"

using namespace mcc::tac;

namespace mcc {
namespace tac {

TEST(Array, Leaf) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto size = std::make_shared<IntLiteral>(5, scope);
  Array a = Array(Type::INT, "a", size, scope);
  EXPECT_EQ(true, a.isLeaf());
}

TEST(Array, Type) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto size = std::make_shared<IntLiteral>(5, scope);
  Array a = Array(Type::INT, "a", size, scope);
  EXPECT_EQ(Type::INT, a.getType());
}

TEST(Array, Value) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  std::string name = "a";
  auto size = std::make_shared<IntLiteral>(5, scope);

  Array a = Array(Type::INT, name, size, scope);

  auto value = name;
  value.append("[");
  value.append(size->getValue());
  value.append("]");

  EXPECT_EQ(value, a.getValue());
}

TEST(Array, Name) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto size = std::make_shared<IntLiteral>(5, scope);
  Array a = Array(Type::INT, "a", size, scope);

  EXPECT_EQ("a", a.getName());
}

TEST(Array, Length) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto size = std::make_shared<IntLiteral>(5, scope);
  Array a = Array(Type::INT, "a", size, scope);

  EXPECT_EQ(size, a.length());
}
}
}
