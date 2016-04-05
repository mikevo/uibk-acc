#include <gtest/gtest.h>

#include "mcc/tac/tac.h"

#include <memory>
#include <string>

#include "ast.h"
#include "parser.h"
#include "mcc/tac/type.h"
#include "test_utils.h"

namespace mcc {
  namespace tac {
     
    TEST(VarTable, VariableLink) {
       auto tree = parser::parse(R"(
        {
            float x=1.0;
            float y = 3.0;
            x + y;

        })");


      Tac tac;
      tac.convertAst(tree);
      
      EXPECT_EQ(tac.getVarTable().size(), 2);
      
      auto x1 = std::static_pointer_cast<Variable>(tac.codeLines[0]->arg1);
      auto y1 = std::static_pointer_cast<Variable>(tac.codeLines[1]->arg1);
      
      auto x2 = std::static_pointer_cast<Variable>(tac.codeLines[2]->arg1);
      auto y2 = std::static_pointer_cast<Variable>(tac.codeLines[2]->arg2);
      
      EXPECT_EQ(x1.get(), x2.get());
      EXPECT_EQ(y1.get(), y2.get());
    }
    
     TEST(VarTable, VariableShadowing) {
       auto tree = parser::parse(R"(
        {
            int x=1;

            {
                float x=5.0;
                
                {
                    int x=x+x; 
                }
                
                
            }
        })");


      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(tac.getVarTable().size(), 3);
      
      auto x1 = std::static_pointer_cast<Variable>(tac.codeLines[0]->arg1);
      auto x2 = std::static_pointer_cast<Variable>(tac.codeLines[1]->arg1);
      auto x3 = std::static_pointer_cast<Variable>(tac.codeLines[2]->arg1);
     
      EXPECT_NE(x1.get(), x2.get());
      EXPECT_NE(x1.get(), x3.get());
      EXPECT_NE(x2.get(), x3.get());
      
      EXPECT_EQ(Type::INT, x1->getType());
      EXPECT_EQ(Type::FLOAT, x2->getType());
      EXPECT_EQ(Type::INT, x3->getType());
    }
  }
}



