#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/float_literal.h"
#include "mcc/tac/scope.h"

using namespace mcc::tac;

namespace mcc {
namespace tac {

TEST(FloatLiteral, Leaf) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  FloatLiteral i = FloatLiteral(42.0, scope);
  EXPECT_EQ(true, i.isLeaf());
}

TEST(FloatLiteral, Type) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  FloatLiteral i = FloatLiteral(42.0, scope);
  EXPECT_EQ(Type::FLOAT, i.getType());
}

TEST(FloatLiteral, Value) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  float testValue = 42.0;

  FloatLiteral f = FloatLiteral(testValue, scope);

  EXPECT_EQ(std::to_string(testValue), f.getValue());
}
}
}
