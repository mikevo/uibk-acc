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

#include <map>
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
        std::shared_ptr<Triple> front();
        std::shared_ptr<Triple> back();
        void push_back(const std::shared_ptr<Triple> line);
        std::vector<std::shared_ptr<Triple>>::size_type size() const;
        std::string toString() const;
        std::vector<std::shared_ptr<Triple>> getBlockMembers() const;
        std::set<std::shared_ptr<Variable>> getUeVar() const;
        std::set<std::shared_ptr<Variable>> getDefVar() const;


      private:
        typedef std::shared_ptr<Variable> VariablePtr;
        typedef std::shared_ptr<Triple> TriplePtr;
        typedef std::set<VariablePtr> VariableSet;

        std::vector<TriplePtr> blockMembers;
        const unsigned id;
        VariableSet defVar;
        VariableSet ueVar;
        std::map<VariablePtr, std::set<TriplePtr>> varOccurrenceMap;
    };
  }
}

#endif /* BASIC_BLOCK_H */

