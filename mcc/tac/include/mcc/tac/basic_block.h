/* 
 * File:   basic_block.h
 * Author: Philipp Stiegernigg
 *
 * Created on March 26, 2016, 11:40 AM
 */

#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include "mcc/tac/triple.h"
#include <memory>

namespace mcc {
    namespace tac {
        class BasicBlock {
        public:
            BasicBlock();
            BasicBlock(std::shared_ptr<Triple> start, std::shared_ptr<Triple> end);
            std::shared_ptr<Triple> getStart();
            std::shared_ptr<Triple> getEnd();
            
        private:
            std::shared_ptr<Triple> blockStart, blockEnd;
            
          
        };
    }
}



#endif /* BASIC_BLOCK_H */

