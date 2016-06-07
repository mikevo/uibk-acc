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

#include "boost/range/irange.hpp"

#include "ast.h"
#include "mcc/tac/array.h"
#include "mcc/tac/basic_block.h"
#include "mcc/tac/label.h"
#include "mcc/tac/operand.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/variable_store.h"
#include "variable.h"

namespace mcc {
namespace tac {

typedef std::vector<BasicBlock::ptr_t> bbVector;
typedef std::shared_ptr<bbVector> bb_type;

class Tac {
  typedef std::map<std::string, Label::ptr_t> function_map_type;
  typedef std::map<Label::ptr_t, std::set<unsigned>> function_return_map_type;

 public:
  typedef std::pair<Type, std::size_t> type_size_type;
  typedef std::map<std::string, std::vector<type_size_type>>
      function_prototype_map_type;
  typedef std::vector<Triple::ptr_t>::iterator code_lines_iter;
  typedef boost::iterator_range<code_lines_iter> code_lines_range;
  typedef std::map<Label::ptr_t, code_lines_range> function_range_map_type;
  typedef std::vector<std::pair<Array::ptr_t, Triple::ptr_t>>
      array_decl_vec_type;
  typedef std::shared_ptr<array_decl_vec_type> array_decl_vec_ptr_type;

  Tac();
  Tac(std::shared_ptr<ast::node> n);

  void addLine(Triple::ptr_t line);
  void addLine(Triple::ptr_t line, unsigned position);
  void addLine(Label::ptr_t line);
  void nextBasicBlock();
  unsigned basicBlockCount();

  VariableStore::ptr_t const getVariableStore();
  std::shared_ptr<function_map_type> getFunctionMap();

  const bb_type getBasicBlockIndex();
  void createBasicBlockIndex();
  void addToVarTable(Variable::ptr_t value);
  void addToArrayDeclVec(Array::ptr_t array, Triple::ptr_t triple);
  array_decl_vec_ptr_type const getArrayDeclVec();
  array_decl_vec_type::iterator findArrayDecl(Array::ptr_t arr);
  Variable::ptr_t addVarRenaming(Variable::ptr_t const key);
  void removeFromVarTable(Variable::ptr_t const value);
  void addFunction(std::string key, Label::ptr_t);
  Label::ptr_t lookupFunction(std::string key);

  void addReturn();
  std::set<unsigned> lookupFunctionReturn(Label::ptr_t);

  void addFunctionPrototype(std::string label,
                            std::vector<type_size_type> argList);
  std::shared_ptr<function_prototype_map_type> getFunctionPrototypeMap();

  std::string toString() const;

  function_range_map_type getFunctionRangeMap();

  std::vector<Triple::ptr_t> codeLines;

 private:
  VariableStore::ptr_t variableStore;
  Label::ptr_t currentFunctionLabel;
  std::shared_ptr<function_map_type> functionMap;
  std::shared_ptr<function_prototype_map_type> functionPrototypeMap;
  std::shared_ptr<function_return_map_type> functionReturnMap;
  void convertAst(std::shared_ptr<ast::node> n);
  unsigned currentBasicBlock;
  bool currentBasicBlockUsed;
  bb_type basicBlockIndex;
  array_decl_vec_ptr_type arrayDeclVec;
};
}
}

#endif /* MCC_TAC_H */
