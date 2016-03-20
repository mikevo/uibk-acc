#include <gtest/gtest.h>

#include "mcc/tac/tac.h"

#include <memory>
#include <string>

#include "ast.h"
#include "parser.h"

namespace mcc {
  namespace tac {
    
    TEST(Tac, ADD) {
      auto tree = parser::parse(R"(1 + 2;)");
      
      std::string expectedValue = "1 + 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
    }
    
    TEST(Tac, SUB) {
      auto tree = parser::parse(R"(1 - 2;)");
      
      std::string expectedValue = "1 - 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
    }
    
    TEST(Tac, MUL) {
      auto tree = parser::parse(R"(1 * 2;)");
      
      std::string expectedValue = "1 * 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
    }
    
    TEST(Tac, DIV) {
      auto tree = parser::parse(R"(1 / 2;)");
      
      std::string expectedValue = "1 / 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
    }
    
    TEST(Tac, Declatration) {
      auto tree = parser::parse(R"(int a = 1;)");
      
      std::string expectedValue = "a = 1";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
    }
  }
}

