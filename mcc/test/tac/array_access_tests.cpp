#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/array.h"
#include "mcc/tac/array_access.h"
#include "mcc/tac/operand.h"
#include "mcc/tac/int_literal.h"

namespace mcc {
namespace tac {

TEST(ArrayAccess, Leaf) {
  auto array = std::make_shared<Array>(Type::INT, "a", 5);
  auto i = std::make_shared<IntLiteral>(2);

  ArrayAccess a = ArrayAccess(array, i);
  EXPECT_EQ(true, a.isLeaf());
}

TEST(ArrayAccess, Type) {
  std::string name = "a";
  std::size_t size = 5;

  auto array = std::make_shared<Array>(Type::INT, name, size);
  auto i = std::make_shared<IntLiteral>(2);

  ArrayAccess a = ArrayAccess(array, i);
  EXPECT_EQ(Type::INT, a.getType());
}

TEST(ArrayAccess, Value) {
  std::string name = "a";
  std::size_t size = 5;

  auto array = std::make_shared<Array>(Type::INT, name, size);
  auto i = std::make_shared<IntLiteral>(2);

  ArrayAccess a = ArrayAccess(array, i);

  auto value = name;
  value.append("[");
  value.append(i->getValue());
  value.append("]");

  EXPECT_EQ(value, a.getValue());
}

TEST(ArrayAccess, Name) {
  std::string name = "a";
  std::size_t size = 5;

  auto array = std::make_shared<Array>(Type::INT, name, size);
  auto i = std::make_shared<IntLiteral>(2);

  ArrayAccess a = ArrayAccess(array, i);

  EXPECT_EQ("a", a.getName());
}

TEST(ArrayAccess, Length) {
  std::string name = "a";
  std::size_t size = 5;

  auto array = std::make_shared<Array>(Type::INT, name, size);
  auto i = std::make_shared<IntLiteral>(2);

  ArrayAccess a = ArrayAccess(array, i);

  EXPECT_EQ(1, a.length());
}

TEST(ArrayAccess, IsTemporary) {
  std::string name = "a";
  std::size_t size = 5;

  auto array = std::make_shared<Array>(Type::INT, name, size);
  auto i = std::make_shared<IntLiteral>(2);

  ArrayAccess a = ArrayAccess(array, i);

  EXPECT_EQ(false, a.isTemporary());
}

TEST(ArrayAccess, IsArray) {
  std::string name = "a";
  std::size_t size = 5;

  auto array = std::make_shared<Array>(Type::INT, name, size);
  auto i = std::make_shared<IntLiteral>(2);

  ArrayAccess a = ArrayAccess(array, i);

  EXPECT_EQ(true, a.isArray());
}
}
}
