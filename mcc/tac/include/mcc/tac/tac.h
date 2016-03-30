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
#include "variable.h"



namespace mcc {
    namespace tac {

        typedef std::pair<std::string, unsigned> VarTableKey;
        typedef std::shared_ptr<Variable> VarTableValue;

        class Tac {
        public:
            Tac();
            void convertAst(std::shared_ptr<ast::node> n);
            void addLine(std::shared_ptr<Operand> line);
            void addLine(std::shared_ptr<Triple> line);
            void addLine(std::shared_ptr<Label> line);
            void nextBasicBlock();
            void enterScope();
            void leaveScope();
            void createBasicBlockIndex();
            unsigned basicBlockCount();
            signed getCurrentScope();

            const std::vector<BasicBlock>& getBasicBlockIndex() const;
            const std::map<VarTableKey, VarTableValue>& getVarTable();
            void addToVarTable(VarTableKey key, VarTableValue value);


            std::string toString() const;
            std::vector<std::shared_ptr<Triple>> codeLines;

        private:
            unsigned currentBasicBlock;
            signed currentScope;
            std::vector<BasicBlock> basicBlockIndex;
            std::map<VarTableKey, VarTableValue> varTable;
        };
    }
}

#endif /* MCC_TAC_H */

