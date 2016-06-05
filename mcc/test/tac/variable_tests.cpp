#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/variable.h"

using namespace mcc::tac;

namespace mcc {
namespace tac {

TEST(Variable, Id) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Variable v1 = Variable(Type::FLOAT, scope);
  Variable v2 = Variable(Type::FLOAT, scope);

  EXPECT_EQ(v1.getId() + 1, v2.getId());
}

TEST(Variable, Leaf) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Variable v = Variable(Type::INT, scope);
  EXPECT_EQ(true, v.isLeaf());
}

TEST(Variable, Type) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Variable v = Variable(Type::INT, scope);
  EXPECT_EQ(Type::INT, v.getType());
}

TEST(Variable, Value) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  std::string testValue = "testVar0:0:0";

  Variable v = Variable(Type::FLOAT, "testVar", std::make_shared<Scope>(0, 0));

  EXPECT_EQ(testValue, v.getValue());
}

TEST(Variable, Name) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Variable v1 = Variable(Type::FLOAT, scope);

  EXPECT_EQ("$t" + std::to_string(v1.getId()), v1.getName());
}

TEST(Variable, IsTemporary) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Variable v1 = Variable(Type::FLOAT, scope);

  EXPECT_EQ(true, v1.isTemporary());

  Variable v2 = Variable(Type::FLOAT, "testVar", std::make_shared<Scope>(0, 0));

  EXPECT_EQ(false, v2.isTemporary());
}

TEST(Variable, IsArray) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Variable v1 = Variable(Type::FLOAT, scope);

  EXPECT_EQ(false, v1.isArray());
}
}
}
