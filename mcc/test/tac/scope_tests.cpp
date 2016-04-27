/*
 * scope_tests.cpp
 *
 *  Created on: Apr 21, 2016
 */

#include <gtest/gtest.h>

#include "mcc/tac/scope.h"

namespace mcc {
namespace tac {
TEST(Scope, Depth) {
  Scope s = Scope(1, 2);

  EXPECT_EQ(1, s.getDepth());
}

TEST(Scope, Index) {
  Scope s = Scope(1, 2);

  EXPECT_EQ(2, s.getIndex());
}

TEST(Scope, NextIndex) {
  Scope s = Scope(1, 2);

  EXPECT_EQ(0, s.getNextIndex());
  EXPECT_EQ(1, s.getNextIndex());
}
}
}
