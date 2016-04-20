#include "mcc/tac/basic_block.h"

#include <string>
#include <iostream>

#include "mcc/tac/label.h"

namespace mcc {
  namespace tac {

    BasicBlock::BasicBlock(const unsigned id) :
        id(id) {

    }

    std::shared_ptr<Triple> BasicBlock::getStart() {
      return this->front();
    }

    std::shared_ptr<Triple> BasicBlock::getEnd() {
      return this->back();
    }

    const unsigned BasicBlock::getBlockId() const {
      return id;
    }

    std::shared_ptr<Triple>& BasicBlock::front() {
      return blockMembers.front();
    }

    std::shared_ptr<Triple>& BasicBlock::back() {
      return blockMembers.back();
    }

    void BasicBlock::push_back(const std::shared_ptr<Triple> line) {
      if (line->containsArg1()) {
        if (typeid (*line->getArg1().get()) == typeid(Triple)) {
          auto arg1 = std::static_pointer_cast<Triple>(line->getArg1());

          if (arg1->getTargetVariable() != nullptr) {
            if (defVar.find(arg1->getTargetVariable()) == defVar.end()) {
              ueVar.insert(arg1->getTargetVariable());
            }
          }
        }
      }

      if (line->containsArg2()) {
        if (typeid (*line->getArg2().get()) == typeid(Triple)) {
          auto arg2 = std::static_pointer_cast<Triple>(line->getArg2());
          if (arg2->getTargetVariable() != nullptr) {
            if (defVar.find(arg2->getTargetVariable()) == defVar.end()) {
              ueVar.insert(arg2->getTargetVariable());
            }
          }
        }
      }

      if (line->getTargetVariable() != nullptr) {
        defVar.insert(line->getTargetVariable());
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

      for (auto &line : blockMembers) {
        output.append(line.get()->toString());

        if (blockMembers.size() > 1) {
          output.append("\n");
        }

        ++count;
      }

      return output;
    }

    std::set<std::shared_ptr<Variable>> BasicBlock::getUeVar() const {
      return ueVar;
    }

    std::set<std::shared_ptr<Variable>> BasicBlock::getDefVar() const {
      return defVar;
    }
  }
}

