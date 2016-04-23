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
#include "mcc/tac/variable_store.h"
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

            std::shared_ptr<VariableStore> const getVariableStore();
           
            const std::vector<std::shared_ptr<BasicBlock>> getBasicBlockIndex();
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
            std::vector<std::shared_ptr<BasicBlock>> basicBlockIndex;
        };
    }
}

#endif /* MCC_TAC_H */

