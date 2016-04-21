/*
 * scope_tests.cpp
 *
 *  Created on: Apr 21, 2016
 */

#include <gtest/gtest.h>

#include "mcc/tac/scope.h"

namespace mcc {
  namespace tac {
    TEST(Scope, Root) {
      Scope s = Scope();

      EXPECT_EQ(0, s.getCurrentScope()->getDepth());
      EXPECT_EQ(0, s.getCurrentScope()->getIndex());
    }

    TEST(Scope, AddNewChild1) {
      Scope s = Scope();

      auto child = s.addNewChild();

      EXPECT_EQ(1, child->getDepth());
      EXPECT_EQ(0, child->getIndex());
    }

    TEST(Scope, GoToParent1) {
      Scope s = Scope();

      EXPECT_EQ(false, s.goToParent());
    }

    TEST(Scope, GoToParent2) {
      Scope s = Scope();

      s.addNewChild();

      EXPECT_EQ(true, s.goToParent());
    }

    TEST(Scope, AddNewChild2) {
      Scope s = Scope();

      auto child = s.addNewChild();

      EXPECT_EQ(1, child->getDepth());
      EXPECT_EQ(0, child->getIndex());

      s.goToParent();

      child = s.addNewChild();

      EXPECT_EQ(1, child->getDepth());
      EXPECT_EQ(1, child->getIndex());
    }

    TEST(Scope, AddSibling) {
      Scope s = Scope();

      auto child = s.addNewChild();

      EXPECT_EQ(1, child->getDepth());
      EXPECT_EQ(0, child->getIndex());

      auto sibling = s.addNewSibling();

      EXPECT_EQ(1, sibling->getDepth());
      EXPECT_EQ(1, sibling->getIndex());
    }

    TEST(Scope, CheckPoint1) {
      Scope s = Scope();

      auto child = s.addNewChild();

      s.goToCheckPoint();

      EXPECT_EQ(child, s.getCurrentScope());
    }

    TEST(Scope, CheckPoint2) {
      Scope s = Scope();

      auto checkPoint = s.getCurrentScope();
      s.setCheckPoint();

      auto child = s.addNewChild();
      auto sibling = s.addNewSibling();

      s.goToCheckPoint();

      EXPECT_NE(child, s.getCurrentScope());
      EXPECT_NE(sibling, s.getCurrentScope());
      EXPECT_EQ(checkPoint, s.getCurrentScope());
    }

  }
}

