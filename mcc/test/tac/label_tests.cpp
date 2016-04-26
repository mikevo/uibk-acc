#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/label.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/operator.h"

namespace mcc {
namespace tac {

TEST(Label, ID_Test) {
  Label t1 = Label();
  Label t2 = Label();

  EXPECT_LT(0, t1.getId());
  EXPECT_EQ(t1.getId() + 1, t2.getId());
}

TEST(Label, Leaf) {
  Label t1 = Label();

  EXPECT_EQ(true, t1.isLeaf());
}

TEST(Label, BBDefaultId) {
  Label t = Label();

  EXPECT_EQ(0, t.getBasicBlockId());
}

TEST(Label, Value) {
  Label t = Label();

  EXPECT_EQ("$L" + std::to_string(t.getId()), t.getValue());
}

TEST(Label, Type) {
  Label t = Label();

  EXPECT_EQ(Type::NONE, t.getType());
}

TEST(Label, OpType) {
  Label t = Label();

  EXPECT_EQ(OperatorName::LABEL, t.getOperator().getName());
}

TEST(Label, ToString) {
  Label t = Label();

  EXPECT_EQ("LABEL " + t.getValue(), t.toString());
}
}
}
