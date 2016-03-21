/* 
 * File:   tac.h
 *
 * Created on March 18, 2016, 11:01 PM
 */

#ifndef MCC_TAC_H
#define MCC_TAC_H

#include <list>
#include <memory>
#include <string>

#include "mcc/tac/operand.h"
#include "mcc/tac/label.h"
#include "mcc/tac/triple.h"
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
      std::string toString() const;

      std::list<std::shared_ptr<Triple>> codeLines;
    };
  }
}

#endif /* MCC_TAC_H */

