/*
 * File:   tac.h
 *
 * Created on March 18, 2016, 11:01 PM
 */

#ifndef MCC_TAC_H
#define MCC_TAC_H

#include <memory>
#include <string>
#include <vector>

#include "ast.h"
#include "mcc/tac/basic_block.h"
#include "mcc/tac/label.h"
#include "mcc/tac/operand.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/variable_store.h"
#include "variable.h"

namespace mcc {
namespace tac {

// pair of name and scope level
typedef std::vector<BasicBlock::ptr_t> bbVector;
typedef std::shared_ptr<bbVector> bb_type;

class Tac {
 public:
  Tac(std::shared_ptr<ast::node> n);

  void addLine(Triple::ptr_t line);
  void addLine(Triple::ptr_t line, unsigned position);
  void addLine(Label::ptr_t line);
  void nextBasicBlock();
  unsigned basicBlockCount();

  VariableStore::ptr_t const getVariableStore();

  const bb_type getBasicBlockIndex();
  void createBasicBlockIndex();
  void addToVarTable(Variable::ptr_t value);
  Variable::ptr_t addVarRenaming(Variable::ptr_t const key);
  void removeFromVarTable(Variable::ptr_t const value);

  std::string toString() const;
  std::vector<Triple::ptr_t> codeLines;

 private:
  VariableStore::ptr_t variableStore;
  void convertAst(std::shared_ptr<ast::node> n);
  unsigned currentBasicBlock;
  bool currentBasicBlockUsed;
  bb_type basicBlockIndex;
};
}
}

#endif /* MCC_TAC_H */
