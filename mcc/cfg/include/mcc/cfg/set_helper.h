





/*
 * set_helper.h
 *
 *  Created on: Apr 19, 2016
 */

#ifndef INCLUDES_MCC_CFG_SET_HELPER_H_
#define INCLUDES_MCC_CFG_SET_HELPER_H_

#include "mcc/tac/tac.h"

#include <algorithm>
#include <set>

namespace mcc {
namespace cfg {

template <class Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key> >
std::set<Key, Compare, Allocator> set_intersect(
    std::set<Key, Compare, Allocator> const &set1,
    std::set<Key, Compare, Allocator> const &set2) {
  std::set<Key, Compare, Allocator> out;

  std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(),
                        std::inserter(out, out.end()), out.key_comp());

  return out;
}

template <class Key, class Compare = std::less<Key>,
          class Allocator = std::allocator<Key> >
std::set<Key, Compare, Allocator> set_union(
    std::set<Key, Compare, Allocator> const &set1,
    std::set<Key, Compare, Allocator> const &set2) {
  std::set<Key, Compare, Allocator> out;

  std::set_union(set1.begin(), set1.end(), set2.begin(), set2.end(),
                 std::inserter(out, out.end()), out.key_comp());

  return out;
}
}
}

#endif /* INCLUDES_MCC_CFG_SET_HELPER_H_ */
