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
template <typename T>
std::set<T> set_intersect(std::set<T> const &set1, std::set<T> const &set2) {
  std::set<T> out;

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

template <typename T, typename C>
std::set<T, C> set_intersect(std::set<T, C> const &set1,
                             std::set<T, C> const &set2) {
  std::set<T, C> out;

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

template <typename T>
std::set<T> set_union(std::set<T> const &set1, std::set<T> const &set2) {
  std::set<T> out(set1);

  for (auto const &var : set2) {
    out.insert(var);
  }

  return out;
}

template <typename T, typename C>
std::set<T, C> set_union(std::set<T, C> const &set1,
                         std::set<T, C> const &set2) {
  std::set<T, C> out(set1);

  for (auto const &var : set2) {
    out.insert(var);
  }

  return out;
}
}
}

#endif /* INCLUDES_MCC_CFG_SET_HELPER_H_ */
