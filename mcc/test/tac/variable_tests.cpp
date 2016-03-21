#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/variable.h"

namespace mcc {
  namespace tac {

    TEST(Variable, Leaf) {
      Variable v = Variable(Type::INT, "testVar");
      EXPECT_EQ(true, v.isLeaf());
    }

    TEST(Variable, Type) {
      Variable v = Variable(Type::INT, "testVar");
      EXPECT_EQ(Type::INT, v.getType());
    }

    TEST(Variable, Value) {
      std::string testValue = "testVar";

      Variable v = Variable(Type::FLOAT, testValue);

      EXPECT_EQ(testValue, v.getValue());
    }
  }
}



