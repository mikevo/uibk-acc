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

      EXPECT_EQ(firstBlock.get()->getStart()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.get()->getEnd()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.get()->getStart()->getType(), Type::INT);
      EXPECT_EQ(firstBlock.get()->getStart()->getId(),
          firstBlock.get()->getEnd()->getId());

    }

    TEST(BasicBlock, ThreeBlocks) {
      auto tree =
          parser::parse(
              R"(
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

      EXPECT_EQ(firstBlock.get()->getStart()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.get()->getEnd()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.get()->getStart()->getType(), Type::INT);

      EXPECT_EQ(secondBlock.get()->getStart()->basicBlockId, 1);
      EXPECT_EQ(secondBlock.get()->getEnd()->basicBlockId, 1);
      EXPECT_EQ(secondBlock.get()->getStart()->getType(), Type::INT);

      EXPECT_EQ(thirdBlock.get()->getStart()->basicBlockId, 2);
      EXPECT_EQ(thirdBlock.get()->getEnd()->basicBlockId, 2);
      EXPECT_EQ(thirdBlock.get()->getStart()->getType(), Type::INT);

    }

    TEST(BasicBlock, SixBlocks) {
      auto tree =
          parser::parse(
              R"(
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

      EXPECT_EQ(firstBlock.get()->getStart()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.get()->getEnd()->basicBlockId, 0);
      EXPECT_EQ(firstBlock.get()->getStart()->getType(), Type::INT);

      EXPECT_EQ(secondBlock.get()->getStart()->basicBlockId, 1);
      EXPECT_EQ(secondBlock.get()->getEnd()->basicBlockId, 1);
      EXPECT_EQ(secondBlock.get()->getStart()->getType(), Type::FLOAT);

      EXPECT_EQ(thirdBlock.get()->getStart()->basicBlockId, 2);
      EXPECT_EQ(thirdBlock.get()->getEnd()->basicBlockId, 2);
      EXPECT_EQ(thirdBlock.get()->getStart()->getType(), Type::FLOAT);

      EXPECT_EQ(fourthBlock.get()->getStart()->basicBlockId, 3);
      EXPECT_EQ(fourthBlock.get()->getEnd()->basicBlockId, 3);
      EXPECT_EQ(fourthBlock.get()->getStart()->getType(), Type::INT);

      EXPECT_EQ(fifthBlock.get()->getStart()->basicBlockId, 4);
      EXPECT_EQ(fifthBlock.get()->getEnd()->basicBlockId, 4);
      EXPECT_EQ(fifthBlock.get()->getStart()->getType(), Type::INT);

      EXPECT_EQ(sixthBlock.get()->getStart()->basicBlockId, 5);
      EXPECT_EQ(sixthBlock.get()->getEnd()->basicBlockId, 5);
      EXPECT_EQ(sixthBlock.get()->getStart()->getType(), Type::INT);
    }

    TEST(BasicBlock, CheckNumOfLines) {
      auto tree =
          parser::parse(
              R"(
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

      auto numOfLines = tac.codeLines.size();
      unsigned linesInBlocks = 0;

      for(auto block : tac.getBasicBlockIndex()) {
        linesInBlocks += block.get()->size();
      }

      EXPECT_EQ(numOfLines, linesInBlocks);

    }
  }
}

