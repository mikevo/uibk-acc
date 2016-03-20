#include <gtest/gtest.h>

#include "mcc/tac/tac.h"

#include "ast.h"

namespace mcc {
  namespace tac {

    TEST(Tac, IntLiteral) {
      int testValue = 42;
      ast::int_literal i = ast::int_literal(testValue);
      
      Tac tac;
      tac.convertAst(i);

      EXPECT_EQ("ASSIGN " + std::to_string(testValue), tac.toString());
    }
    
    TEST(Tac, FloatLiteral) {
      float testValue = 42.0;
      ast::float_literal f = ast::float_literal(testValue);
      
      Tac tac;
      tac.convertAst(f);

      EXPECT_EQ("ASSIGN " + std::to_string(testValue), tac.toString());
    }
  }
}

