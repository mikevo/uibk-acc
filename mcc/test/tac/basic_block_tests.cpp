#include <gtest/gtest.h>

#include "mcc/tac/tac.h"

#include <memory>
#include <string>

#include "ast.h"
#include "mcc/tac/type.h"
#include "parser.h"

namespace mcc {
namespace tac {

TEST(BasicBlock, OneBlock) {
  auto tree = parser::parse(R"(1 + 2;)");

  Tac tac = Tac(tree);

  auto firstBlock = tac.getBasicBlockIndex()->at(0);

  EXPECT_EQ(tac.basicBlockCount(), 1);

  EXPECT_EQ(firstBlock.get()->getStart()->getBasicBlockId(), 0);
  EXPECT_EQ(firstBlock.get()->getEnd()->getBasicBlockId(), 0);
  EXPECT_EQ(firstBlock.get()->getStart()->getType(), Type::INT);
  EXPECT_EQ(firstBlock.get()->getStart()->getId(),
            firstBlock.get()->getEnd()->getId());
}

TEST(BasicBlock, FourBlocks) {
  auto tree = parser::parse(
      R"(
        {
          int a = 0;

          if(1 <= 2) {
            a = 1;
          } else {
            a = 2;
          }
        })");

  Tac tac = Tac(tree);

  auto firstBlock = tac.getBasicBlockIndex()->at(0);
  auto secondBlock = tac.getBasicBlockIndex()->at(1);
  auto thirdBlock = tac.getBasicBlockIndex()->at(2);
  auto fourthBlock = tac.getBasicBlockIndex()->at(3);

  EXPECT_EQ(tac.basicBlockCount(), 4);

  EXPECT_EQ(firstBlock.get()->getStart()->getBasicBlockId(), 0);
  EXPECT_EQ(firstBlock.get()->getEnd()->getBasicBlockId(), 0);
  EXPECT_EQ(firstBlock.get()->getStart()->getType(), Type::INT);

  EXPECT_EQ(secondBlock.get()->getStart()->getBasicBlockId(), 1);
  EXPECT_EQ(secondBlock.get()->getEnd()->getBasicBlockId(), 1);
  EXPECT_EQ(secondBlock.get()->getStart()->getType(), Type::INT);

  EXPECT_EQ(thirdBlock.get()->getStart()->getBasicBlockId(), 2);
  EXPECT_EQ(thirdBlock.get()->getEnd()->getBasicBlockId(), 2);

  auto members = thirdBlock->getBlockMembers();
  EXPECT_EQ(members.at(1)->getType(), Type::INT);

  EXPECT_EQ(fourthBlock.get()->getStart()->getBasicBlockId(), 3);
  EXPECT_EQ(fourthBlock.get()->getStart().get(),
            fourthBlock.get()->getEnd().get());
  EXPECT_EQ(fourthBlock.get()->getStart()->getType(), Type::NONE);

  EXPECT_EQ(2, firstBlock->getDefVar().size());
  EXPECT_EQ(0, firstBlock->getUeVar().size());

  members = firstBlock->getBlockMembers();

  EXPECT_EQ(true, members.at(0)->containsTargetVar());
  EXPECT_EQ(true, members.at(1)->containsTargetVar());

  EXPECT_NE(firstBlock->getDefVar().end(),
            firstBlock->getDefVar().find(members.at(0)->getTargetVariable()));
  EXPECT_NE(firstBlock->getDefVar().end(),
            firstBlock->getDefVar().find(members.at(1)->getTargetVariable()));

  EXPECT_EQ(1, secondBlock->getDefVar().size());
  EXPECT_EQ(0, secondBlock->getUeVar().size());

  members = secondBlock->getBlockMembers();

  EXPECT_EQ(true, members.at(0)->containsTargetVar());

  EXPECT_NE(secondBlock->getDefVar().end(),
            secondBlock->getDefVar().find(members.at(0)->getTargetVariable()));
}

TEST(BasicBlock, SevenBlocks) {
  auto tree = parser::parse(
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

  Tac tac = Tac(tree);

  auto firstBlock = tac.getBasicBlockIndex()->at(0);
  auto secondBlock = tac.getBasicBlockIndex()->at(1);
  auto thirdBlock = tac.getBasicBlockIndex()->at(2);
  auto fourthBlock = tac.getBasicBlockIndex()->at(3);
  auto fifthBlock = tac.getBasicBlockIndex()->at(4);
  auto sixthBlock = tac.getBasicBlockIndex()->at(5);
  auto seventhBlock = tac.getBasicBlockIndex()->at(6);

  EXPECT_EQ(tac.basicBlockCount(), 7);

  EXPECT_EQ(firstBlock.get()->getStart()->getBasicBlockId(), 0);
  EXPECT_EQ(firstBlock.get()->getEnd()->getBasicBlockId(), 0);
  EXPECT_EQ(firstBlock.get()->getStart()->getType(), Type::INT);

  EXPECT_EQ(secondBlock.get()->getStart()->getBasicBlockId(), 1);
  EXPECT_EQ(secondBlock.get()->getEnd()->getBasicBlockId(), 1);
  EXPECT_EQ(secondBlock.get()->getStart()->getType(), Type::FLOAT);

  EXPECT_EQ(thirdBlock.get()->getStart()->getBasicBlockId(), 2);
  EXPECT_EQ(thirdBlock.get()->getEnd()->getBasicBlockId(), 2);

  auto members = thirdBlock->getBlockMembers();
  EXPECT_EQ(members.at(1)->getType(), Type::FLOAT);

  EXPECT_EQ(fourthBlock.get()->getStart()->getBasicBlockId(), 3);
  EXPECT_EQ(fourthBlock.get()->getEnd()->getBasicBlockId(), 3);

  members = fourthBlock->getBlockMembers();
  EXPECT_EQ(members.at(1)->getType(), Type::INT);

  EXPECT_EQ(fifthBlock.get()->getStart()->getBasicBlockId(), 4);
  EXPECT_EQ(fifthBlock.get()->getEnd()->getBasicBlockId(), 4);
  EXPECT_EQ(fifthBlock.get()->getStart()->getType(), Type::INT);

  EXPECT_EQ(sixthBlock.get()->getStart()->getBasicBlockId(), 5);
  EXPECT_EQ(sixthBlock.get()->getEnd()->getBasicBlockId(), 5);

  members = sixthBlock->getBlockMembers();
  EXPECT_EQ(members.at(1)->getType(), Type::INT);

  EXPECT_EQ(seventhBlock.get()->getStart()->getBasicBlockId(), 6);
  EXPECT_EQ(seventhBlock.get()->getStart().get(),
            seventhBlock.get()->getEnd().get());
  EXPECT_EQ(seventhBlock.get()->getStart()->getType(), Type::NONE);
}

TEST(BasicBlock, CheckNumOfLines) {
  auto tree = parser::parse(
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

  Tac tac = Tac(tree);

  auto numOfLines = tac.codeLines.size();
  unsigned linesInBlocks = 0;

  for (auto const block : *tac.getBasicBlockIndex().get()) {
    linesInBlocks += block->size();
  }

  EXPECT_EQ(numOfLines, linesInBlocks);
}

TEST(BasicBlock, WhileBlockCreation) {
  auto tree = parser::parse(
      R"(
        {
          int a = 0;
          while(a < 50) {
            a = a - 3;
            a = a + 2;
            a = a + 2;
            
          }
            float b = 10.0;
            b = 2.0;
        })");

  Tac tac = Tac(tree);
  auto blockIndex = tac.getBasicBlockIndex();

  EXPECT_EQ(tac.basicBlockCount(), 4);
  EXPECT_EQ(blockIndex->at(0)->size(), 1);  // Declaration
  EXPECT_EQ(blockIndex->at(1)->size(),
            3);  // Label, While Condition and JumpFalse
  EXPECT_EQ(blockIndex->at(2)->size(), 7);  // temp var, assignments and jump
  EXPECT_EQ(blockIndex->at(3)->size(), 3);  // Label, assignments
}

TEST(BasicBlock, NestedWhileBlockCreation) {
  auto tree = parser::parse(
      R"(
        {
          int a = 0;
          int b = 150;
          while(a < 50) {
            b = b - 1;
            while(b > 100)
            a = a + 1;
          }
            int c = 10;
            c = b * 2;
        })");

  Tac tac = Tac(tree);
  auto blockIndex = tac.getBasicBlockIndex();

  EXPECT_EQ(tac.basicBlockCount(), 7);
  EXPECT_EQ(blockIndex->at(0)->size(), 2);
  EXPECT_EQ(blockIndex->at(1)->size(), 3);
  EXPECT_EQ(blockIndex->at(2)->size(), 2);
  EXPECT_EQ(blockIndex->at(3)->size(), 3);
  EXPECT_EQ(blockIndex->at(4)->size(), 3);
  EXPECT_EQ(blockIndex->at(5)->size(), 2);
  EXPECT_EQ(blockIndex->at(6)->size(), 4);
}

TEST(BasicBlock, UEVarDefVar) {
  auto tree = parser::parse(
      R"(
        {
          int a;
          int b;
            
          a = 0;
          if(3 < 5) {
            b = 5;
          }
          else {
            int x = a + b;
          }

        })");

  Tac tac = Tac(tree);
  auto blockIndex = tac.getBasicBlockIndex();

  EXPECT_EQ(tac.basicBlockCount(), 4);
  auto firstBlock = blockIndex->at(0);
  auto secondBlock = blockIndex->at(1);
  auto thirdBlock = blockIndex->at(2);
  auto fourthBlock = blockIndex->at(3);

  EXPECT_EQ(firstBlock->getDefVar().size(), 2);
  EXPECT_EQ(firstBlock->getUeVar().size(), 0);
  EXPECT_EQ(secondBlock->getDefVar().size(), 1);
  EXPECT_EQ(secondBlock->getUeVar().size(), 0);
  EXPECT_EQ(thirdBlock->getDefVar().size(), 1);
  EXPECT_EQ(thirdBlock->getUeVar().size(), 2);
  EXPECT_EQ(fourthBlock->getDefVar().size(), 0);
  EXPECT_EQ(fourthBlock->getUeVar().size(), 0);
}
}
}
