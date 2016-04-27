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
typedef std::pair<std::string, std::shared_ptr<Scope>> VarTableKey;
typedef std::shared_ptr<Variable> VarTableValue;
typedef std::vector<std::shared_ptr<BasicBlock>> bbVector;
typedef std::shared_ptr<bbVector> bb_type;

class Tac {
 public:
  Tac(std::shared_ptr<ast::node> n);
  void addLine(std::shared_ptr<Triple> line);
  void addLine(std::shared_ptr<Label> line);
  void nextBasicBlock();
  unsigned basicBlockCount();

  std::shared_ptr<VariableStore> const getVariableStore();

  const bb_type getBasicBlockIndex();
  void addToVarTable(VarTableValue value);
  VarTableValue addVarRenaming(VarTableValue const key);
  void removeFromVarTable(VarTableValue const value);

  std::string toString() const;
  std::vector<std::shared_ptr<Triple>> codeLines;

 private:
  std::shared_ptr<VariableStore> variableStore;
  void convertAst(std::shared_ptr<ast::node> n);
  void createBasicBlockIndex();
  unsigned currentBasicBlock;
  bb_type basicBlockIndex;
};
}
}

#endif /* MCC_TAC_H */
