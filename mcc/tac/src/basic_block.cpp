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

Triple::ptr_t BasicBlock::getStart() { return this->front(); }

Triple::ptr_t BasicBlock::getEnd() { return this->back(); }

const unsigned BasicBlock::getBlockId() const { return id; }

Triple::ptr_t BasicBlock::front() { return blockMembers.front(); }

Triple::ptr_t BasicBlock::back() { return blockMembers.back(); }

void BasicBlock::push_back(const Triple::ptr_t line) {
  if (line->containsTargetVar()) {
    auto target = line->getTargetVariable();
    defVar.insert(target);
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

std::vector<Triple::ptr_t>::size_type BasicBlock::size() const {
  return blockMembers.size();
}

std::vector<Triple::ptr_t> BasicBlock::getBlockMembers() const {
  return blockMembers;
}

std::string BasicBlock::toString() const {
  std::string output;

  output.append("BB-ID: " + std::to_string(id) + "\n");

  // TODO: is this const& fine? i'd say yes cause line is only used in loop
  for (auto const &line : blockMembers) {
    output.append(line->toString());
    output.append("\n");
  }

  return output;
}

Variable::set_t BasicBlock::getUeVar() const { return ueVar; }

Variable::set_t BasicBlock::getDefVar() const { return defVar; }
}
}
