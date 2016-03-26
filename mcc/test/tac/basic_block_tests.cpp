#include <gtest/gtest.h>

#include "mcc/tac/tac.h"

#include <memory>
#include <string>

#include "ast.h"
#include "parser.h"
#include "mcc/tac/type.h"

namespace mcc {
  namespace tac {

    TEST(BasicBlock, OneBlock) {
      auto tree = parser::parse(R"(1 + 2;)");

      Tac tac;
      tac.convertAst(tree);
      tac.createBasicBlockIndex();
      
      EXPECT_EQ(tac.basicBlockCount(), 1);
      
      auto firstBlock = tac.getBasicBlockIndex()[0];

      EXPECT_EQ(firstBlock.getStart()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.getEnd()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.getStart()->getType(), Type::INT);
      EXPECT_EQ(firstBlock.getStart()->getId(), firstBlock.getEnd()->getId());
      
    }
    
     TEST(BasicBlock, ThreeBlocks) {
       auto tree = parser::parse(R"(
        {
          int a = 0;

          if(1 <= 2) {
            a = 1;
          } else {
            a = 2;
          }
        })");


      Tac tac;
      tac.convertAst(tree);
      tac.createBasicBlockIndex();
      
      EXPECT_EQ(tac.basicBlockCount(), 3);
      

      auto firstBlock = tac.getBasicBlockIndex()[0];
      auto secondBlock = tac.getBasicBlockIndex()[1];
      auto thirdBlock = tac.getBasicBlockIndex()[2];

      EXPECT_EQ(firstBlock.getStart()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.getEnd()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.getStart()->getType(), Type::INT);
      
      EXPECT_EQ(secondBlock.getStart()->basicBlockId, 1);
      EXPECT_EQ(secondBlock.getEnd()->basicBlockId, 1);
      EXPECT_EQ(secondBlock.getStart()->getType(), Type::INT);
      
      EXPECT_EQ(thirdBlock.getStart()->basicBlockId, 2);
      EXPECT_EQ(thirdBlock.getEnd()->basicBlockId, 2);
      EXPECT_EQ(thirdBlock.getStart()->getType(), Type::INT);
      
    }
     
    TEST(BasicBlock, SixBlocks) {
       auto tree = parser::parse(R"(
        {
            int x=1;
            float y = 3.0;

            if(x > 0) {
		y = y * 1.5;
            } else {
		y = y + 2.0;
            }

            int a = 0;

            if( 1 <= 2) {
                a = 1;
            } else {
                a = 2;
            }
        })");


      Tac tac;
      tac.convertAst(tree);
      tac.createBasicBlockIndex();

      EXPECT_EQ(tac.basicBlockCount(), 6);
      
      auto firstBlock = tac.getBasicBlockIndex()[0];
      auto secondBlock = tac.getBasicBlockIndex()[1];
      auto thirdBlock = tac.getBasicBlockIndex()[2];
      auto fourthBlock = tac.getBasicBlockIndex()[3];
      auto fifthBlock = tac.getBasicBlockIndex()[4];
      auto sixthBlock = tac.getBasicBlockIndex()[5];

      EXPECT_EQ(firstBlock.getStart()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.getEnd()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.getStart()->getType(), Type::INT);
      
      EXPECT_EQ(secondBlock.getStart()->basicBlockId, 1);
      EXPECT_EQ(secondBlock.getEnd()->basicBlockId, 1);
      EXPECT_EQ(secondBlock.getStart()->getType(), Type::FLOAT);
      
      EXPECT_EQ(thirdBlock.getStart()->basicBlockId, 2);
      EXPECT_EQ(thirdBlock.getEnd()->basicBlockId, 2);
      EXPECT_EQ(thirdBlock.getStart()->getType(), Type::FLOAT);
      
      EXPECT_EQ(fourthBlock.getStart()->basicBlockId, 3);
      EXPECT_EQ(fourthBlock.getEnd()->basicBlockId, 3);
      EXPECT_EQ(fourthBlock.getStart()->getType(), Type::INT);
      
      EXPECT_EQ(fifthBlock.getStart()->basicBlockId, 4);
      EXPECT_EQ(fifthBlock.getEnd()->basicBlockId, 4);
      EXPECT_EQ(fifthBlock.getStart()->getType(), Type::INT);
      
      EXPECT_EQ(sixthBlock.getStart()->basicBlockId, 5);
      EXPECT_EQ(sixthBlock.getEnd()->basicBlockId, 5);
      EXPECT_EQ(sixthBlock.getStart()->getType(), Type::INT);
    }
  }
}



