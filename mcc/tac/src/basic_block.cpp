#include "mcc/tac/basic_block.h"

#include <iostream>
#include <string>

#include "mcc/cfg/cfg.h"
#include "mcc/cfg/set_helper.h"
#include "mcc/tac/helper/ast_converters.h"
#include "mcc/tac/label.h"

namespace mcc {
namespace tac {

BasicBlock::BasicBlock(const unsigned id) : id(id) {}

std::shared_ptr<Triple> BasicBlock::getStart() { return this->front(); }

std::shared_ptr<Triple> BasicBlock::getEnd() { return this->back(); }

const unsigned BasicBlock::getBlockId() const { return id; }

std::shared_ptr<Triple> BasicBlock::front() { return blockMembers.front(); }

std::shared_ptr<Triple> BasicBlock::back() { return blockMembers.back(); }

void BasicBlock::push_back(const std::shared_ptr<Triple> line) {
  SubExpressionPtr se;

  if (convertableToSubExpression(line)) {
    se = std::make_shared<mcc::cfg::SubExpression>(line);
    deExpr.insert(se);
  }

  if (line->containsTargetVar()) {
    auto target = line->getTargetVariable();
    defVar.insert(target);

    auto searchResult = this->varOccurrenceMap.find(target);

    if (searchResult != this->varOccurrenceMap.end()) {
      killedExpr = mcc::cfg::set_union(killedExpr, searchResult->second);
    }
  }

  if (convertableToSubExpression(line)) {
    for (auto const v : se->getVariables()) {
      auto searchResult = this->varOccurrenceMap.find(v);

      if (searchResult != this->varOccurrenceMap.end()) {
        searchResult->second.insert(se);
      } else {
        SubExpressionSet seSet;
        seSet.insert(se);

        this->varOccurrenceMap.insert(std::make_pair(v, seSet));
      }
    }
  }

  if (line->containsArg1()) {
    if (helper::isType<Triple>(line->getArg1())) {
      auto arg1 = std::static_pointer_cast<Triple>(line->getArg1());

      if (arg1->containsTargetVar()) {
        if (defVar.find(arg1->getTargetVariable()) == defVar.end()) {
          ueVar.insert(arg1->getTargetVariable());
        }
      }
    } else if (helper::isType<Variable>(line->getArg1())) {
      auto arg1 = std::static_pointer_cast<Variable>(line->getArg1());

      if (defVar.find(arg1) == defVar.end()) {
        ueVar.insert(arg1);
      }
    }
  }

  if (line->containsArg2()) {
    if (helper::isType<Triple>(line->getArg2())) {
      auto arg2 = std::static_pointer_cast<Triple>(line->getArg2());
      if (arg2->containsTargetVar()) {
        if (defVar.find(arg2->getTargetVariable()) == defVar.end()) {
          ueVar.insert(arg2->getTargetVariable());
        }
      }
    } else if (helper::isType<Variable>(line->getArg2())) {
      auto arg2 = std::static_pointer_cast<Variable>(line->getArg2());

      if (defVar.find(arg2) == defVar.end()) {
        ueVar.insert(arg2);
      }
    }
  }

  blockMembers.push_back(line);
}

std::vector<std::shared_ptr<Triple>>::size_type BasicBlock::size() const {
  return blockMembers.size();
}

std::vector<std::shared_ptr<Triple>> BasicBlock::getBlockMembers() const {
  return blockMembers;
}

std::string BasicBlock::toString() const {
  std::string output;

  output.append("BB-ID: " + std::to_string(id) + "\n");

  unsigned count = 0;

  // TODO: is this const& fine? i'd say yes cause line is only used in loop
  for (auto const &line : blockMembers) {
    output.append(line->toString());

    if (blockMembers.size() > 1) {
      output.append("\n");
    }

    ++count;
  }

  return output;
}

Variable::Set BasicBlock::getUeVar() const {
  return ueVar;
}

Variable::Set BasicBlock::getDefVar() const {
  return defVar;
}

BasicBlock::SubExpressionSet BasicBlock::getDeExpr() const { return deExpr; }

BasicBlock::SubExpressionSet BasicBlock::getKilledExpr() const {
  return killedExpr;
}

bool convertableToSubExpression(const std::shared_ptr<Triple> line) {
  if (line->containsArg1()) {
    switch (line->getOperator().getName()) {
      case OperatorName::LABEL:
      case OperatorName::JUMP:
      case OperatorName::JUMPFALSE:
        return false;
      default:
        return true;
    }
  } else {
    return false;
  }
}
}
}
