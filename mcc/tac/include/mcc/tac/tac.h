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
#include "mcc/tac/scope.h"
#include "ast.h"
#include "variable.h"



namespace mcc {
    namespace tac {

        // pair of name and scope level
        typedef std::pair<std::string, std::shared_ptr<ScopeNode>> VarTableKey;
        typedef std::shared_ptr<Variable> VarTableValue;

        class Tac {
        public:
            Tac(std::shared_ptr<ast::node> n);
            void addLine(std::shared_ptr<Triple> line);
            void addLine(std::shared_ptr<Label> line);
            void nextBasicBlock();
            unsigned basicBlockCount();

            Scope& getScope();
           
            const std::map<VarTableKey, std::vector<VarTableValue>>& getVarTable();
            const std::vector<std::shared_ptr<BasicBlock>>& getBasicBlockIndex();
            void addToVarTable(Tac *tac, VarTableValue value);
            VarTableValue addVarRenaming(VarTableKey key);
            void removeFromVarTable(Tac* tac, VarTableValue value);


            std::string toString() const;
            std::vector<std::shared_ptr<Triple>> codeLines;

        private:
            void convertAst(std::shared_ptr<ast::node> n);
            void createBasicBlockIndex();
            unsigned currentBasicBlock;
            Scope scope;
            std::vector<std::shared_ptr<BasicBlock>> basicBlockIndex;
            std::map<VarTableKey, std::vector<VarTableValue>> varTable;
        };
    }
}

#endif /* MCC_TAC_H */

