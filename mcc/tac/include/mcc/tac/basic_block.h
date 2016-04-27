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
  typedef std::shared_ptr<Triple> TriplePtr;
  typedef std::shared_ptr<mcc::cfg::SubExpression> SubExpressionPtr;
  typedef std::set<SubExpressionPtr, mcc::cfg::SubExpression::less>
      SubExpressionSet;

  BasicBlock(const unsigned id);
  std::shared_ptr<Triple> getStart();
  std::shared_ptr<Triple> getEnd();
  const unsigned getBlockId() const;
  std::shared_ptr<Triple> front();
  std::shared_ptr<Triple> back();
  void push_back(const std::shared_ptr<Triple> line);
  std::vector<std::shared_ptr<Triple>>::size_type size() const;
  std::string toString() const;
  std::vector<std::shared_ptr<Triple>> getBlockMembers() const;

  Variable::Set getUeVar() const;
  Variable::Set getDefVar() const;
  SubExpressionSet getDeExpr() const;
  SubExpressionSet getKilledExpr() const;

 private:
  std::vector<TriplePtr> blockMembers;
  const unsigned id;
  Variable::Set defVar;
  Variable::Set ueVar;
  SubExpressionSet deExpr;
  SubExpressionSet killedExpr;
  std::map<Variable::Ptr, SubExpressionSet> varOccurrenceMap;
};

bool convertableToSubExpression(const std::shared_ptr<Triple> line);
}
}

#endif /* BASIC_BLOCK_H */
