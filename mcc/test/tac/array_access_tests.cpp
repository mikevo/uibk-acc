#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/array.h"
#include "mcc/tac/array_access.h"
#include "mcc/tac/operand.h"
#include "mcc/tac/int_literal.h"

using namespace mcc::tac;

namespace mcc {
namespace tac {

TEST(ArrayAccess, Leaf) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  auto size = std::make_shared<IntLiteral>(5, scope);
  auto array = std::make_shared<Array>(Type::INT, "a", size, scope);
  auto i = std::make_shared<IntLiteral>(2, scope);

  ArrayAccess a = ArrayAccess(array, i, scope);
  EXPECT_EQ(true, a.isLeaf());
}

TEST(ArrayAccess, Type) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  std::string name = "a";
  auto size = std::make_shared<IntLiteral>(5, scope);

  auto array = std::make_shared<Array>(Type::INT, name, size, scope);
  auto i = std::make_shared<IntLiteral>(2, scope);

  ArrayAccess a = ArrayAccess(array, i, scope);
  EXPECT_EQ(Type::INT, a.getType());
}

TEST(ArrayAccess, Value) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  std::string name = "a";
  auto size = std::make_shared<IntLiteral>(5, scope);

  auto array = std::make_shared<Array>(Type::INT, name, size, scope);
  auto i = std::make_shared<IntLiteral>(2, scope);

  ArrayAccess a = ArrayAccess(array, i, scope);

  auto value = name;
  value.append("[");
  value.append(i->getValue());
  value.append("]");

  EXPECT_EQ(value, a.getValue());
}

TEST(ArrayAccess, Name) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  std::string name = "a";
  auto size = std::make_shared<IntLiteral>(5, scope);

  auto array = std::make_shared<Array>(Type::INT, name, size, scope);
  auto i = std::make_shared<IntLiteral>(2, scope);

  ArrayAccess a = ArrayAccess(array, i, scope);

  EXPECT_EQ("a", a.getName());
}

TEST(ArrayAccess, IsTemporary) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  std::string name = "a";
  auto size = std::make_shared<IntLiteral>(5, scope);

  auto array = std::make_shared<Array>(Type::INT, name, size, scope);
  auto i = std::make_shared<IntLiteral>(2, scope);

  ArrayAccess a = ArrayAccess(array, i, scope);

  EXPECT_EQ(false, a.isTemporary());
}

TEST(ArrayAccess, IsArray) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  std::string name = "a";
  auto size = std::make_shared<IntLiteral>(5, scope);

  auto array = std::make_shared<Array>(Type::INT, name, size, scope);
  auto i = std::make_shared<IntLiteral>(2, scope);

  ArrayAccess a = ArrayAccess(array, i, scope);

  EXPECT_EQ(true, a.isArray());
}
}
}
