#include <gtest/gtest.h>

#include "mcc/tac/tac.h"

#include <memory>
#include <string>

#include "ast.h"
#include "parser.h"
#include "mcc/tac/type.h"

namespace mcc {
  namespace tac {
    
    TEST(Tac, ADD) {
      auto tree = parser::parse(R"(1 + 2;)");
      
      std::string expectedValue = "1 + 2";
       
      Tac tac;
      tac.convertAst(tree);
      
      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, SUB) {
      auto tree = parser::parse(R"(1 - 2;)");
      
      std::string expectedValue = "1 - 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, MUL) {
      auto tree = parser::parse(R"(1 * 2;)");
      
      std::string expectedValue = "1 * 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, DIV) {
      auto tree = parser::parse(R"(1 / 2;)");
      
      std::string expectedValue = "1 / 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, EQ) {
      auto tree = parser::parse(R"(1 == 2;)");
      
      std::string expectedValue = "1 == 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, NE) {
      auto tree = parser::parse(R"(1 != 2;)");
      
      std::string expectedValue = "1 != 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, LE) {
      auto tree = parser::parse(R"(1 <= 2;)");
      
      std::string expectedValue = "1 <= 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, GE) {
      auto tree = parser::parse(R"(1 >= 2;)");
      
      std::string expectedValue = "1 >= 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, LT) {
      auto tree = parser::parse(R"(1 < 2;)");
      
      std::string expectedValue = "1 < 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, GT) {
      auto tree = parser::parse(R"(1 > 2;)");
      
      std::string expectedValue = "1 > 2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::BOOL, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, DeclatrationInt) {
      auto tree = parser::parse(R"(int a = 1;)");
      
      std::string expectedValue = "a = 1";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, DeclatrationFloat) {
      auto tree = parser::parse(R"(float b = 1.0;)");
      
      std::string expectedValue = "b = " + std::to_string(1.0);
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::FLOAT, tac.codeLines.back().get()->getType());
    }
    
    TEST(Tac, Minus) {
      auto tree = parser::parse(R"(-2;)");
      
      std::string expectedValue = "-2";
       
      Tac tac;
      tac.convertAst(tree);

      EXPECT_EQ(expectedValue, tac.toString());
      EXPECT_EQ(Type::INT, tac.codeLines.back().get()->getType());
    }
  }
}

