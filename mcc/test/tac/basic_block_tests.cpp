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

      auto firstBlock = tac.getBasicBlockIndex()[0];

      EXPECT_EQ(tac.basicBlockCount(), 1);

      EXPECT_EQ(firstBlock.get()->getStart()->getBasicBlockId(), 0);
      EXPECT_EQ(firstBlock.get()->getEnd()->getBasicBlockId(), 0);
      EXPECT_EQ(firstBlock.get()->getStart()->getType(), Type::INT);
      EXPECT_EQ(firstBlock.get()->getStart()->getId(),
          firstBlock.get()->getEnd()->getId());

    }

    TEST(BasicBlock, FourBlocks) {
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

      auto firstBlock = tac.getBasicBlockIndex()[0];
      auto secondBlock = tac.getBasicBlockIndex()[1];
      auto thirdBlock = tac.getBasicBlockIndex()[2];
      auto fourthBlock = tac.getBasicBlockIndex()[3];

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
      EXPECT_EQ(members[1]->getType(), Type::INT);

      EXPECT_EQ(fourthBlock.get()->getStart()->getBasicBlockId(), 3);
      EXPECT_EQ(fourthBlock.get()->getStart().get(),
          fourthBlock.get()->getEnd().get());
      EXPECT_EQ(fourthBlock.get()->getStart()->getType(), Type::NONE);

      EXPECT_EQ(2, firstBlock->getDefVar().size());
      EXPECT_EQ(0, firstBlock->getUeVar().size());

      members = firstBlock->getBlockMembers();

      EXPECT_EQ(true, members[0]->containsTargetVar());
      EXPECT_EQ(true, members[1]->containsTargetVar());

      EXPECT_NE(firstBlock->getDefVar().end(),
          firstBlock->getDefVar().find(members[0]->getTargetVariable()));
      EXPECT_NE(firstBlock->getDefVar().end(),
          firstBlock->getDefVar().find(members[1]->getTargetVariable()));

      EXPECT_EQ(1, secondBlock->getDefVar().size());
      EXPECT_EQ(0, secondBlock->getUeVar().size());

      members = secondBlock->getBlockMembers();

      EXPECT_EQ(true, members[0]->containsTargetVar());

      EXPECT_NE(secondBlock->getDefVar().end(),
                secondBlock->getDefVar().find(members[0]->getTargetVariable()));

      for (auto const& v : thirdBlock->getDefVar()) {
        std::cout << v->getFullName() << std::endl;
      }

      std::cout << thirdBlock->toString() << std::endl;

      for (auto const& v : thirdBlock->getUeVar()) {
        std::cout << v->getFullName() << std::endl;
      }
    }

    TEST(BasicBlock, SevenBlocks) {
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

      auto firstBlock = tac.getBasicBlockIndex()[0];
      auto secondBlock = tac.getBasicBlockIndex()[1];
      auto thirdBlock = tac.getBasicBlockIndex()[2];
      auto fourthBlock = tac.getBasicBlockIndex()[3];
      auto fifthBlock = tac.getBasicBlockIndex()[4];
      auto sixthBlock = tac.getBasicBlockIndex()[5];
      auto seventhBlock = tac.getBasicBlockIndex()[6];

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
      EXPECT_EQ(members[1]->getType(), Type::FLOAT);

      EXPECT_EQ(fourthBlock.get()->getStart()->getBasicBlockId(), 3);
      EXPECT_EQ(fourthBlock.get()->getEnd()->getBasicBlockId(), 3);

      members = fourthBlock->getBlockMembers();
      EXPECT_EQ(members[1]->getType(), Type::INT);

      EXPECT_EQ(fifthBlock.get()->getStart()->getBasicBlockId(), 4);
      EXPECT_EQ(fifthBlock.get()->getEnd()->getBasicBlockId(), 4);
      EXPECT_EQ(fifthBlock.get()->getStart()->getType(), Type::INT);

      EXPECT_EQ(sixthBlock.get()->getStart()->getBasicBlockId(), 5);
      EXPECT_EQ(sixthBlock.get()->getEnd()->getBasicBlockId(), 5);

      members = sixthBlock->getBlockMembers();
      EXPECT_EQ(members[1]->getType(), Type::INT);

      EXPECT_EQ(seventhBlock.get()->getStart()->getBasicBlockId(), 6);
      EXPECT_EQ(seventhBlock.get()->getStart().get(),
          seventhBlock.get()->getEnd().get());
      EXPECT_EQ(seventhBlock.get()->getStart()->getType(), Type::NONE);
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

      auto numOfLines = tac.codeLines.size();
      unsigned linesInBlocks = 0;

      for (auto block : tac.getBasicBlockIndex()) {
        linesInBlocks += block.get()->size();
      }

      EXPECT_EQ(numOfLines, linesInBlocks);

    }
  }
}

