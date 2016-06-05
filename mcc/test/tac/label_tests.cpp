#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/int_literal.h"
#include "mcc/tac/label.h"
#include "mcc/tac/operator.h"

using namespace mcc::tac;

namespace mcc {
namespace tac {

TEST(Label, ID_Test) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Label t1 = Label(scope);
  Label t2 = Label(scope);

  EXPECT_LT(0, t1.getId());
  EXPECT_EQ(t1.getId() + 1, t2.getId());
}

TEST(Label, Leaf) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Label t1 = Label(scope);

  EXPECT_EQ(true, t1.isLeaf());
}

TEST(Label, BBDefaultId) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Label t = Label(scope);

  EXPECT_EQ(0, t.getBasicBlockId());
}

TEST(Label, Value) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Label t = Label(scope);

  EXPECT_EQ("$L" + std::to_string(t.getId()), t.getValue());
}

TEST(Label, Type) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Label t = Label(scope);

  EXPECT_EQ(Type::NONE, t.getType());
}

TEST(Label, OpType) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Label t = Label(scope);

  EXPECT_EQ(OperatorName::LABEL, t.getOperator().getName());
}

TEST(Label, ToString) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  Label t = Label(scope);

  EXPECT_EQ("LABEL " + t.getValue(), t.toString());
}
}
}
