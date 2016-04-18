/* 
 * File:   basic_block.h
 * Author: Philipp Stiegernigg
 *
 * Created on March 26, 2016, 11:40 AM
 */

#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H


#include "mcc/tac/triple.h"
#include "mcc/tac/variable.h"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace mcc {
  namespace tac {
    class BasicBlock {
      public:
        BasicBlock(const unsigned id);
        std::shared_ptr<Triple> getStart();
        std::shared_ptr<Triple> getEnd();
        const unsigned getBlockId() const;
        std::shared_ptr<Triple>& front();
        std::shared_ptr<Triple>& back();
        void push_back(const std::shared_ptr<Triple> line);
        std::vector<std::shared_ptr<Triple>>::size_type size() const;
        std::string toString() const;
        std::vector<std::shared_ptr<Triple>> getBlockMembers() const;
        std::set<std::shared_ptr<Variable>> getUeVar() const;
        std::set<std::shared_ptr<Variable>> getDefVar() const;


      private:
        std::vector<std::shared_ptr<Triple>> blockMembers;
        const unsigned id;
        std::set<std::shared_ptr<Variable>> defVar;
        std::set<std::shared_ptr<Variable>> ueVar;
    };
  }
}

#endif /* BASIC_BLOCK_H */

