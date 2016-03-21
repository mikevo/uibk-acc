#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/float_literal.h"

namespace mcc {
  namespace tac {

    TEST(FloatLiteral, Leaf) {
      FloatLiteral i = FloatLiteral(42.0);
      EXPECT_EQ(true, i.isLeaf());
    }

    TEST(FloatLiteral, Type) {
      FloatLiteral i = FloatLiteral(42.0);
      EXPECT_EQ(Type::FLOAT, i.getType());
    }

    TEST(FloatLiteral, Value) {
      float testValue = 42.0;

      FloatLiteral f = FloatLiteral(testValue);

      EXPECT_EQ(std::to_string(testValue), f.getValue());
    }
  }
}



