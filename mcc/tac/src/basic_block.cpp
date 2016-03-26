#include "mcc/tac/basic_block.h"

namespace mcc {
    namespace tac {

        BasicBlock::BasicBlock() {

        }

        BasicBlock::BasicBlock(std::shared_ptr<Triple> start,
                std::shared_ptr<Triple> end) :
        blockStart(start),
        blockEnd(end) {

        }

        std::shared_ptr<Triple> BasicBlock::getStart() {
            return blockStart;
        }

        std::shared_ptr<Triple> BasicBlock::getEnd() {
            return blockEnd;
        }

    }
}

