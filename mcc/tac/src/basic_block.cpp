#include "mcc/tac/basic_block.h"

#include "mcc/tac/label.h"

namespace mcc {
  namespace tac {

    BasicBlock::BasicBlock() {

    }

    std::shared_ptr<Triple> BasicBlock::getStart() {
      return this->front();
    }

    std::shared_ptr<Triple> BasicBlock::getEnd() {
      return this->back();
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

  }
}

