#include "mcc/tac/basic_block.h"

#include <string>

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
      blockMembers.push_back(line);
    }

    std::vector<std::shared_ptr<Triple>>::size_type BasicBlock::size() const {
      return blockMembers.size();
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

  }
}

