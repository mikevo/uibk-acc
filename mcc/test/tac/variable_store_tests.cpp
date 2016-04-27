/*
 * variable_store_tests.cpp
 *
 *  Created on: Apr 22, 2016
 */

#include <gtest/gtest.h>

#include "mcc/tac/variable_store.h"

#include "mcc/tac/scope.h"
#include "mcc/tac/variable.h"

namespace mcc {
namespace tac {

TEST(VariableStore, VariableTableEmpty) {
  VariableStore store;

  EXPECT_EQ(0, store.size());
}

TEST(VariableStore, addVariable) {
  VariableStore store;

  auto var0 = std::make_shared<Variable>(Type::FLOAT);
  auto var1 = std::make_shared<Variable>(Type::INT);

  store.addVariable(var0);

  EXPECT_EQ(1, store.size());
  EXPECT_EQ(var0, store[0]);

  store.addVariable(var1);

  EXPECT_EQ(2, store.size());
  EXPECT_EQ(var0, store[0]);
  EXPECT_EQ(var1, store[1]);
}

TEST(VariableStore, renameVariable) {
  VariableStore store;

  auto var0 = std::make_shared<Variable>(Type::FLOAT, "a",
                                         std::make_shared<Scope>(0, 0));
  auto var1 =
      std::make_shared<Variable>(Type::INT, "b", std::make_shared<Scope>(1, 0));

  store.addVariable(var0);

  EXPECT_EQ(1, store.size());
  EXPECT_EQ(var0, store[0]);

  store.addVariable(var1);

  EXPECT_EQ(2, store.size());
  EXPECT_EQ(var0, store[0]);
  EXPECT_EQ(var1, store[1]);

  auto rvar0 = store.renameVariable(var0);

  EXPECT_EQ(3, store.size());
  EXPECT_EQ(var0, store[0]);
  EXPECT_EQ(var1, store[1]);
  EXPECT_EQ(rvar0, store[2]);

  EXPECT_EQ("a0:0:0", var0->getValue());
  EXPECT_EQ("a1:0:0", rvar0->getValue());

  auto rvar1 = store.renameVariable(var1);

  EXPECT_EQ(4, store.size());
  EXPECT_EQ(var0, store[0]);
  EXPECT_EQ(var1, store[1]);
  EXPECT_EQ(rvar0, store[2]);
  EXPECT_EQ(rvar1, store[3]);

  EXPECT_EQ("a0:0:0", var0->getValue());
  EXPECT_EQ("a1:0:0", rvar0->getValue());
  EXPECT_EQ("b0:1:0", var1->getValue());
  EXPECT_EQ("b1:1:0", rvar1->getValue());
}

TEST(VariableStore, removeVariable) {
  VariableStore store;

  auto var0 =
      std::make_shared<Variable>(Type::FLOAT, "a", store.getCurrentScope());
  store.addNewChildScope();
  auto var1 =
      std::make_shared<Variable>(Type::INT, "b", store.getCurrentScope());

  store.addVariable(var0);
  store.addVariable(var1);

  auto rvar0 = store.renameVariable(var0);
  auto rrvar0 = store.renameVariable(var0);
  auto rvar1 = store.renameVariable(var1);

  // TODO: change rename representation to allow removing renamed variables
  // as well if necessary
  EXPECT_EQ(false, store.removeVariable(rvar0));
  EXPECT_EQ(false, store.removeVariable(rvar1));

  EXPECT_EQ(5, store.size());
  EXPECT_EQ(var0, store[0]);
  EXPECT_EQ(var1, store[1]);
  EXPECT_EQ(rvar0, store[2]);
  EXPECT_EQ(rrvar0, store[3]);
  EXPECT_EQ(rvar1, store[4]);

  EXPECT_EQ("a0:0:0", var0->getValue());
  EXPECT_EQ("a1:0:0", rvar0->getValue());
  EXPECT_EQ("a2:0:0", rrvar0->getValue());
  EXPECT_EQ("b0:1:0", var1->getValue());
  EXPECT_EQ("b1:1:0", rvar1->getValue());

  EXPECT_EQ(true, store.removeVariable(var0));

  EXPECT_EQ(2, store.size());
  EXPECT_EQ(var1, store[0]);
  EXPECT_EQ(rvar1, store[1]);

  EXPECT_EQ("b0:1:0", var1->getValue());
  EXPECT_EQ("b1:1:0", rvar1->getValue());
}

TEST(VariableStore, findAccordingVariable) {
  VariableStore store;

  auto var0 =
      std::make_shared<Variable>(Type::FLOAT, "a", store.getCurrentScope());
  store.addNewChildScope();
  auto var1 =
      std::make_shared<Variable>(Type::INT, "b", store.getCurrentScope());

  store.addVariable(var0);
  store.addVariable(var1);

  auto rvar0 = store.renameVariable(var0);
  auto rvar1 = store.renameVariable(var1);

  EXPECT_EQ(4, store.size());
  EXPECT_EQ(var0, store[0]);
  EXPECT_EQ(var1, store[1]);
  EXPECT_EQ(rvar0, store[2]);
  EXPECT_EQ(rvar1, store[3]);

  EXPECT_EQ("a0:0:0", var0->getValue());
  EXPECT_EQ("a1:0:0", rvar0->getValue());
  EXPECT_EQ("b0:1:0", var1->getValue());
  EXPECT_EQ("b1:1:0", rvar1->getValue());

  store.addNewChildScope();

  auto result = store.findAccordingVariable("b");

  EXPECT_EQ(rvar1, result);
}

TEST(VariableStore, findVariable) {
  VariableStore store;

  auto var0 =
      std::make_shared<Variable>(Type::FLOAT, "a", store.getCurrentScope());
  store.addNewChildScope();
  auto var1 =
      std::make_shared<Variable>(Type::INT, "b", store.getCurrentScope());

  store.addVariable(var0);
  store.addVariable(var1);

  auto rvar0 = store.renameVariable(var0);
  auto rvar1 = store.renameVariable(var1);

  EXPECT_EQ(4, store.size());
  EXPECT_EQ(var0, store[0]);
  EXPECT_EQ(var1, store[1]);
  EXPECT_EQ(rvar0, store[2]);
  EXPECT_EQ(rvar1, store[3]);

  EXPECT_EQ("a0:0:0", var0->getValue());
  EXPECT_EQ("a1:0:0", rvar0->getValue());
  EXPECT_EQ("b0:1:0", var1->getValue());
  EXPECT_EQ("b1:1:0", rvar1->getValue());

  store.addNewChildScope();

  auto result = store.findVariable("b");

  EXPECT_EQ(var1, result);
}
}
}
