/*
 * scope_node_tests.cpp
 *
 *  Created on: Apr 21, 2016
 */

#include <gtest/gtest.h>

#include "mcc/tac/scope_node.h"

namespace mcc {
  namespace tac {
    TEST(ScopeNode, Depth) {
      ScopeNode s = ScopeNode(1, 2);

      EXPECT_EQ(1, s.getDepth());
    }

    TEST(ScopeNode, Index) {
      ScopeNode s = ScopeNode(1, 2);

      EXPECT_EQ(2, s.getIndex());
    }

    TEST(ScopeNode, NextIndex) {
      ScopeNode s = ScopeNode(1, 2);

      EXPECT_EQ(0, s.getNextIndex());
      EXPECT_EQ(1, s.getNextIndex());
    }
  }
}

