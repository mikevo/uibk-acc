/*
 * File:   basic_block.h
 * Author: Philipp Stiegernigg
 *
 * Created on March 26, 2016, 11:40 AM
 */

#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include "mcc/cfg/sub_expression.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/variable.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace mcc {
namespace tac {
class BasicBlock {
 public:
  typedef std::shared_ptr<BasicBlock> ptr_t;

  BasicBlock(const unsigned id);
  Triple::ptr_t getStart();
  Triple::ptr_t getEnd();
  const unsigned getBlockId() const;
  Triple::ptr_t front();
  Triple::ptr_t back();
  void push_back(const Triple::ptr_t line);
  std::vector<Triple::ptr_t>::size_type size() const;
  std::string toString() const;
  std::vector<Triple::ptr_t> getBlockMembers() const;

  Variable::set_t getUeVar() const;
  Variable::set_t getDefVar() const;

 private:
  std::vector<Triple::ptr_t> blockMembers;
  const unsigned id;
  Variable::set_t defVar;
  Variable::set_t ueVar;
};
}
}

#endif /* BASIC_BLOCK_H */
