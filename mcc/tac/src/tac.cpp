#include "mcc/tac/tac.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <ostream>
#include <typeinfo>

#include "ast.h"
#include "mcc/tac/float_literal.h"
#include "mcc/tac/helper/ast_converters.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/label.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/variable.h"

namespace mcc {
namespace tac {

Tac::Tac() : currentBasicBlock(0), currentBasicBlockUsed(false) {
  this->variableStore = std::make_shared<VariableStore>();
  this->basicBlockIndex = std::make_shared<bbVector>();
  this->functionMap = std::make_shared<function_map_type>();
  this->functionPrototypeMap = std::make_shared<function_prototype_map_type>();
  this->functionReturnMap = std::make_shared<function_return_map_type>();
}

Tac::Tac(std::shared_ptr<ast::node> n)
    : currentBasicBlock(0), currentBasicBlockUsed(false) {
  this->variableStore = std::make_shared<VariableStore>();
  this->basicBlockIndex = std::make_shared<bbVector>();
  this->functionMap = std::make_shared<function_map_type>();
  this->functionPrototypeMap = std::make_shared<function_prototype_map_type>();
  this->functionReturnMap = std::make_shared<function_return_map_type>();
  this->convertAst(n);
}

void Tac::convertAst(std::shared_ptr<ast::node> n) {
  helper::convertNode(this, n);
}

void Tac::addLine(Triple::ptr_t line) {
  line->setBasicBlockId(currentBasicBlock);
  currentBasicBlockUsed = true;
  this->codeLines.push_back(line);
}

void Tac::addLine(Label::ptr_t line) {
  line->setBasicBlockId(currentBasicBlock);
  currentBasicBlockUsed = true;
  this->codeLines.push_back(line);
}

void Tac::addLine(Triple::ptr_t line, unsigned position) {
  assert(position >= 0 && "Position cannot be negative!");

  if (position > 0) {
    auto prevTriple = (*std::prev(this->codeLines.begin() + position));
    auto op = prevTriple->getOperator().getName();
    unsigned blockID = prevTriple->getBasicBlockId();
    if (op == OperatorName::JUMP || op == OperatorName::JUMPFALSE) {
      line->setBasicBlockId(blockID + 1);
    } else {
      line->setBasicBlockId(blockID);
    }
  } else {
    line->setBasicBlockId(0);
  }

  auto iterator = this->codeLines.begin();
  this->codeLines.insert(iterator + position, line);
}

VariableStore::ptr_t const Tac::getVariableStore() {
  return this->variableStore;
}

std::shared_ptr<Tac::function_map_type> Tac::getFunctionMap() {
  return this->functionMap;
}

void Tac::nextBasicBlock() {
  if (currentBasicBlockUsed) {
    ++currentBasicBlock;
    currentBasicBlockUsed = false;
  }
}

void Tac::createBasicBlockIndex() {
  basicBlockIndex->clear();

  auto currentBasicBlockId = codeLines.front()->getBasicBlockId();
  auto currentBasicBlock = std::make_shared<BasicBlock>(currentBasicBlockId);

  for (auto &triple : codeLines) {
    auto bbId = triple->getBasicBlockId();
    if (bbId != currentBasicBlockId) {
      basicBlockIndex->push_back(currentBasicBlock);
      currentBasicBlock = std::make_shared<BasicBlock>(bbId);
      currentBasicBlockId = bbId;
    }

    currentBasicBlock->push_back(triple);
  }

  // Add last block
  basicBlockIndex->push_back(currentBasicBlock);
}

std::string Tac::toString() const {
  std::string output;

  unsigned count = 0;

  for (auto &line : codeLines) {
    output.append(line->toString());

    auto size = codeLines.size();
    if (size > 1 && count < size - 1) output.append("\n");
    ++count;
  }

  return output;
}

unsigned Tac::basicBlockCount() { return basicBlockIndex->size(); }

const bb_type Tac::getBasicBlockIndex() {
  if (basicBlockIndex->empty()) createBasicBlockIndex();

  return basicBlockIndex;
}

void Tac::addToVarTable(Variable::ptr_t value) {
  this->variableStore->addVariable(value);
}

void Tac::removeFromVarTable(Variable::ptr_t const value) {
  this->variableStore->removeVariable(value);
}

Variable::ptr_t Tac::addVarRenaming(Variable::ptr_t const key) {
  return this->variableStore->renameVariable(key);
}

void Tac::addFunction(std::string key, Label::ptr_t label) {
  functionMap->insert(std::make_pair(key, label));
  currentFunctionLabel = label;
}

Label::ptr_t Tac::lookupFunction(std::string key) {
  auto value = functionMap->find(key);

  if (value != functionMap->end()) {
    return value->second;
  } else {
    return nullptr;
  }
}

void Tac::addReturn() {
  auto entry = functionReturnMap->find(currentFunctionLabel);

  if (entry != functionReturnMap->end()) {
    entry->second.insert(this->currentBasicBlock);
  } else {
    std::set<unsigned> list;
    list.insert(this->currentBasicBlock);
    auto pair = std::make_pair(currentFunctionLabel, list);

    functionReturnMap->insert(pair);
  }
}

std::set<unsigned> Tac::lookupFunctionReturn(Label::ptr_t key) {
  auto value = functionReturnMap->find(key);

  if (value != functionReturnMap->end()) {
    return value->second;
  } else {
    std::set<unsigned> empty;
    return empty;
  }
}

void Tac::addFunctionPrototype(std::string label, std::vector<Type> argList) {
  auto pair = std::make_pair(label, argList);

  this->functionPrototypeMap->insert(pair);
}

std::shared_ptr<Tac::function_prototype_map_type>
Tac::getFunctionPrototypeMap() {
  return this->functionPrototypeMap;
}

Tac::function_range_map_type Tac::getFunctionRangeMap() {
  function_range_map_type map;

  code_lines_iter start = this->codeLines.begin();
  Label::ptr_t currentLabel = std::static_pointer_cast<Label>(*start);

  for (auto it = start + 1; it != this->codeLines.end(); ++it) {
    if (helper::isType<Label>(*it)) {
      auto label = std::static_pointer_cast<Label>(*it);

      if (label->isFunctionEntry()) {
        auto range = boost::make_iterator_range(start, it);
        auto pair = std::make_pair(currentLabel, range);

        map.insert(pair);

        currentLabel = label;
        start = it;
      }
    }
  }

  auto range = boost::make_iterator_range(start, this->codeLines.end());
  auto pair = std::make_pair(currentLabel, range);

  map.insert(pair);

  return map;
}
}
}
