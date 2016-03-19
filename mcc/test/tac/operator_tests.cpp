#include <gtest/gtest.h>

#include <string>

#include "mcc/tac/operator.h"

namespace mcc {
    namespace tac {

        TEST(Operator, ADD) {
            OperatorName name = OperatorName::ADD;
            Operator op = Operator(name);
            
            EXPECT_EQ(OperatorType::BINARY, op.getType());
            EXPECT_EQ(name, op.getName());
        }
        
        TEST(Operator, ASSIGN) {
            OperatorName name = OperatorName::ASSIGN;
            Operator op = Operator(name);
            
            EXPECT_EQ(OperatorType::UNARY, op.getType());
            EXPECT_EQ(name, op.getName());
        }
        
        TEST(Operator, JUMP) {
            OperatorName name = OperatorName::JUMP;
            Operator op = Operator(name);
            
            EXPECT_EQ(OperatorType::UNARY, op.getType());
            EXPECT_EQ(name, op.getName());
        }
        
        TEST(Operator, LABLE) {
            OperatorName name = OperatorName::LABLE;
            Operator op = Operator(name);
            
            EXPECT_EQ(OperatorType::UNARY, op.getType());
            EXPECT_EQ(name, op.getName());
        }
        
        TEST(Operator, MUL) {
            OperatorName name = OperatorName::MUL;
            Operator op = Operator(name);
            
            EXPECT_EQ(OperatorType::BINARY, op.getType());
            EXPECT_EQ(name, op.getName());
        }
        
        TEST(Operator, SUB) {
            OperatorName name = OperatorName::SUB;
            Operator op = Operator(name);
            
            EXPECT_EQ(OperatorType::BINARY, op.getType());
            EXPECT_EQ(name, op.getName());
        }
    }
}
