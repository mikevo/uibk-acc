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

Tac::Tac(std::shared_ptr<ast::node> n) : currentBasicBlock(0) {
  this->variableStore = std::make_shared<VariableStore>();
  this->basicBlockIndex = std::make_shared<bbVector>();
  this->convertAst(n);
}

void Tac::convertAst(std::shared_ptr<ast::node> n) {
  helper::convertNode(this, n);
}

void Tac::addLine(std::shared_ptr<Triple> line) {
  line->setBasicBlockId(currentBasicBlock);
  this->codeLines.push_back(line);
}

void Tac::addLine(std::shared_ptr<Label> line) {
  line->setBasicBlockId(currentBasicBlock);
  this->codeLines.push_back(line);
}

std::shared_ptr<VariableStore> const Tac::getVariableStore() {
  return this->variableStore;
}

void Tac::nextBasicBlock() { ++currentBasicBlock; }

void Tac::createBasicBlockIndex() {
  basicBlockIndex->clear();

  auto currentBasicBlock = std::make_shared<BasicBlock>(0);
  auto currentBasicBlockId = codeLines.front()->getBasicBlockId();

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

void Tac::addToVarTable(VarTableValue value) {
  this->variableStore->addVariable(value);
}

void Tac::removeFromVarTable(VarTableValue const value) {
  this->variableStore->removeVariable(value);
}

VarTableValue Tac::addVarRenaming(VarTableValue const key) {
  return this->variableStore->renameVariable(key);
}
}
}
