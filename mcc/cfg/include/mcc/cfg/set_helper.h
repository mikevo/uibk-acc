/*
 * set_helper.h
 *
 *  Created on: Apr 19, 2016
 */

#ifndef INCLUDES_MCC_CFG_SET_HELPER_H_
#define INCLUDES_MCC_CFG_SET_HELPER_H_

#include "mcc/tac/tac.h"

#include <set>

namespace mcc {
namespace cfg {

template <class Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key> >
std::set<Key, Compare, Allocator> set_intersect(
    std::set<Key, Compare, Allocator> const &set1,
    std::set<Key, Compare, Allocator> const &set2) {
  std::set<Key, Compare, Allocator> out;

  if (set1.size() < set2.size()) {
    for (auto const &var : set2) {
      if (set1.find(var) != set1.end()) {
        out.insert(var);
      }
    }
  } else {
    for (auto const &var : set1) {
      if (set2.find(var) != set2.end()) {
        out.insert(var);
      }
    }
  }

  return out;
}

template <class Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key> >
std::set<Key, Compare, Allocator> set_union(
    std::set<Key, Compare, Allocator> const &set1,
    std::set<Key, Compare, Allocator> const &set2) {
  std::set<Key, Compare, Allocator> out(set1);

  for (auto const &var : set2) {
    out.insert(var);
  }

  return out;
}
}
}

#endif /* INCLUDES_MCC_CFG_SET_HELPER_H_ */
