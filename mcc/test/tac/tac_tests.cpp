#include <gtest/gtest.h>

#include "mcc/tac/tac.h"

#include <memory>
#include <string>

#include "ast.h"

namespace mcc {
  namespace tac {

    TEST(Tac, IntLiteral) {
      int testValue = 42;
      auto i = std::make_shared<ast::int_literal>(testValue);
      
      Tac tac;
      tac.convertAst(i);

      EXPECT_EQ(std::to_string(testValue), tac.toString());
    }
    
    TEST(Tac, FloatLiteral) {
      float testValue = 42.0;
      auto f = std::make_shared<ast::float_literal>(testValue);
      
      Tac tac;
      tac.convertAst(f);

      EXPECT_EQ(std::to_string(testValue), tac.toString());
    }
    
    TEST(Tac, Variable) {
      std::string testValue = "a";
      auto type = std::make_shared<ast::int_type>();
      
      auto var = std::make_shared<ast::variable>(type, testValue);
      
      Tac tac;
      tac.convertAst(var);

      EXPECT_EQ(testValue, tac.toString());
    }
  }
}

