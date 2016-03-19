#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/triple.h"
#include "mcc/tac/int_literal.h"
#include "mcc/tac/operator.h"

namespace mcc {
    namespace tac {
        TEST(Triple, ID_Test) {
            IntLiteral i = IntLiteral(42);
            Operator op = Operator(OperatorName::ASSIGN);
            
            Triple t1 = Triple(op, &i);
            Triple t2 = Triple(op, &i);

            EXPECT_EQ(1, t1.getId());
            EXPECT_EQ(2, t2.getId());
        }
        
        TEST(Triple, BBDefaultId) {
            IntLiteral i = IntLiteral(42);
            Operator op = Operator(OperatorName::ASSIGN);
            
            Triple t = Triple(op, &i);
            
            EXPECT_EQ(0, t.basicBlockId);
        }
        
        TEST(Triple, Value) {
            IntLiteral i = IntLiteral(42);
            Operator op = Operator(OperatorName::ASSIGN);
            
            Triple t = Triple(op, &i);
            
            EXPECT_EQ("t" + std::to_string(t.getId()), t.getValue());
        }
    }
}

