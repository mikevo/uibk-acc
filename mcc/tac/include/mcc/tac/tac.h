/* 
 * File:   tac.h
 *
 * Created on March 18, 2016, 11:01 PM
 */

#ifndef MCC_TAC_H
#define MCC_TAC_H

#include <vector>
#include <memory>
#include <string>

#include "mcc/tac/operand.h"
#include "mcc/tac/label.h"
#include "mcc/tac/triple.h"
#include "mcc/tac/basic_block.h"
#include "ast.h"

namespace mcc {
  namespace tac {

    class Tac {
    public:
      Tac();
      void convertAst(std::shared_ptr<ast::node> n);
      void addLine(std::shared_ptr<Operand> line);
      void addLine(std::shared_ptr<Triple> line);
      void addLine(std::shared_ptr<Label> line);
      void nextBasicBlock();
      void createBasicBlockIndex();
      unsigned basicBlockCount();
      const std::vector<BasicBlock>& getBasicBlockIndex() const;
      
      std::string toString() const;
      std::vector<std::shared_ptr<Triple>> codeLines;
      
      
    private:
        unsigned currBasicBlockId;
        std::vector<BasicBlock> basicBlockIndex;
    };
  }
}

#endif /* MCC_TAC_H */

