#include <gtest/gtest.h>

#include "mcc/cfg/set_helper.h"

#include <set>

namespace mcc {
namespace cfg {

TEST(SetHelper, setIntersect) {
  std::set<unsigned> const setA{1, 2, 3, 4, 5, 6, 7, 8};
  std::set<unsigned> const setB{5, 7, 9, 10};

  std::set<unsigned> setResult;

  setResult = set_intersect(setA, setB);

  EXPECT_EQ(2, setResult.size());
  EXPECT_NE(setResult.find(5), setResult.end());
  EXPECT_NE(setResult.find(7), setResult.end());
}

TEST(SetHelper, setUnion) {
  std::set<unsigned> const setA{1, 2, 3, 4, 5, 6, 7, 8};
  std::set<unsigned> const setB{5, 7, 9, 10};

  std::set<unsigned> setResult;

  setResult = set_union(setA, setB);

  EXPECT_EQ(10, setResult.size());

  for (auto const u : setA) {
    EXPECT_NE(setResult.find(u), setResult.end());
  }

  for (auto const u : setB) {
    EXPECT_NE(setResult.find(u), setResult.end());
  }
}
}
}
