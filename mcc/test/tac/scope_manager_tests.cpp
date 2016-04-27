/*
 * scope_manager_tests.cpp
 *
 *  Created on: Apr 21, 2016
 */

#include <gtest/gtest.h>

#include "mcc/tac/scope_manager.h"

namespace mcc {
namespace tac {
TEST(ScopeManager, Root) {
  ScopeManager s = ScopeManager();

  EXPECT_EQ(0, s.getCurrentScope()->getDepth());
  EXPECT_EQ(0, s.getCurrentScope()->getIndex());
}

TEST(ScopeManager, AddNewChild1) {
  ScopeManager s = ScopeManager();

  auto child = s.addNewChildScope();

  EXPECT_EQ(1, child->getDepth());
  EXPECT_EQ(0, child->getIndex());
}

TEST(ScopeManager, GoToParent1) {
  ScopeManager s = ScopeManager();

  EXPECT_EQ(false, s.goToParentScope());
}

TEST(ScopeManager, GoToParent2) {
  ScopeManager s = ScopeManager();

  s.addNewChildScope();

  EXPECT_EQ(true, s.goToParentScope());
}

TEST(ScopeManager, AddNewChild2) {
  ScopeManager s = ScopeManager();

  auto child = s.addNewChildScope();

  EXPECT_EQ(1, child->getDepth());
  EXPECT_EQ(0, child->getIndex());

  s.goToParentScope();

  child = s.addNewChildScope();

  EXPECT_EQ(1, child->getDepth());
  EXPECT_EQ(1, child->getIndex());
}

TEST(ScopeManager, AddSibling) {
  ScopeManager s = ScopeManager();

  auto child = s.addNewChildScope();

  EXPECT_EQ(1, child->getDepth());
  EXPECT_EQ(0, child->getIndex());

  auto sibling = s.addNewSiblingScope();

  EXPECT_EQ(1, sibling->getDepth());
  EXPECT_EQ(1, sibling->getIndex());
}

TEST(ScopeManager, CheckPoint1) {
  ScopeManager s = ScopeManager();

  auto child = s.addNewChildScope();

  s.goToCheckPoint();

  EXPECT_EQ(child, s.getCurrentScope());
}

TEST(ScopeManager, CheckPoint2) {
  ScopeManager s = ScopeManager();

  auto checkPoint = s.getCurrentScope();
  s.setCheckPoint();

  auto child = s.addNewChildScope();
  auto sibling = s.addNewSiblingScope();

  s.goToCheckPoint();

  EXPECT_NE(child, s.getCurrentScope());
  EXPECT_NE(sibling, s.getCurrentScope());
  EXPECT_EQ(checkPoint, s.getCurrentScope());
}
}
}
