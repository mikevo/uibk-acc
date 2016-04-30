#include <gtest/gtest.h>

#include "mcc/cfg/cfg.h"

#include "boost/graph/graphviz.hpp"
#include "mcc/cfg/set_helper.h"
#include "parser.h"
#include "test_utils.h"

#include <iostream>

namespace mcc {
namespace cfg {

namespace helper {
struct LiveSetGen {
  std::vector<mcc::tac::Variable::set_t> expected;
  mcc::tac::bb_type bbI;

  LiveSetGen(mcc::tac::Tac& tac) {
    this->bbI = tac.getBasicBlockIndex();
    this->expected = std::vector<mcc::tac::Variable::set_t>(this->bbI->size());
  };

  void addVariable(unsigned bbId, unsigned posInBb, unsigned destBb) {
    auto line = bbI->at(bbId)->getBlockMembers().at(posInBb);
    expected.at(destBb).insert(line->getTargetVariable());
  };
};
}

TEST(Cfg, Cfg) {
  auto tree = parser::parse(
      R"(
         {
            int x=1;
            float y = 3.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  std::string expected =
      R"(digraph G {
0;
1;
2;
3;
4;
5;
6;
7;
8;
9;
10;
11;
12;
13;
0->1 ;
0->4 ;
1->2 ;
1->3 ;
2->1 ;
3->10 ;
4->5 ;
5->6 ;
5->9 ;
6->7 ;
6->8 ;
7->6 ;
8->5 ;
9->10 ;
10->11 ;
11->12 ;
11->13 ;
12->11 ;
}
)";

  EXPECT_EQ(expected, graph->toDot());
}

TEST(Cfg, DomTree) {
  auto tree = parser::parse(
      R"(
         {
            int x=1;
            float y = 3.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto domTree = graph->getDomTree();

  for (auto domMap : domTree) {
    unsigned expected;
    switch (domMap.first) {
      case 1:
        expected = 0;
        break;
      case 2:
        expected = 1;
        break;
      case 3:
        expected = 1;
        break;
      case 4:
        expected = 0;
        break;
      case 5:
        expected = 4;
        break;
      case 6:
        expected = 5;
        break;
      case 7:
        expected = 6;
        break;
      case 8:
        expected = 6;
        break;
      case 9:
        expected = 5;
        break;
      case 10:
        expected = 0;
        break;
      case 11:
        expected = 10;
        break;
      case 12:
        expected = 11;
        break;
      case 13:
        expected = 11;
        break;
    }

    EXPECT_EQ(expected, domMap.second);
  }
}

TEST(Cfg, Idom) {
  auto tree = parser::parse(
      R"(
         {
            int x=1;
            float y = 3.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto index = tac.getBasicBlockIndex();

  for (auto bb : *index.get()) {
    auto idomV = graph->getIdom(bb);
    auto idomVD = graph->getIdom(bb->getBlockId());

    Vertex expectedV;
    VertexDescriptor expectedVD;
    switch (bb->getBlockId()) {
      case 0:
        expectedVD = 0;
        break;
      case 1:
        expectedVD = 0;
        break;
      case 2:
        expectedVD = 1;
        break;
      case 3:
        expectedVD = 1;
        break;
      case 4:
        expectedVD = 0;
        break;
      case 5:
        expectedVD = 4;
        break;
      case 6:
        expectedVD = 5;
        break;
      case 7:
        expectedVD = 6;
        break;
      case 8:
        expectedVD = 6;
        break;
      case 9:
        expectedVD = 5;
        break;
      case 10:
        expectedVD = 0;
        break;
      case 11:
        expectedVD = 10;
        break;
      case 12:
        expectedVD = 11;
        break;
      case 13:
        expectedVD = 11;
        break;
    }
    expectedV = index->at(expectedVD);

    // TODO find reason why shared pointers need << of *node
    EXPECT_EQ(expectedV.get(), idomV.get());
    EXPECT_EQ(expectedVD, idomVD);
  }
}

TEST(Cfg, DomSet) {
  auto tree = parser::parse(
      R"(
         {
            int x=1;
            float y = 3.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto index = tac.getBasicBlockIndex();

  for (auto bb : *index.get()) {
    auto domSetV = graph->getDomSet(bb);
    auto domSetVD = graph->getDomSet(bb->getBlockId());

    std::set<Vertex> expectedV;
    std::set<VertexDescriptor> expectedVD;
    switch (bb->getBlockId()) {
      case 0:
        expectedVD = {0};
        break;
      case 1:
        expectedVD = {0, 1};
        break;
      case 2:
        expectedVD = {0, 1, 2};
        break;
      case 3:
        expectedVD = {0, 1, 3};
        break;
      case 4:
        expectedVD = {0, 4};
        break;
      case 5:
        expectedVD = {0, 4, 5};
        break;
      case 6:
        expectedVD = {0, 4, 5, 6};
        break;
      case 7:
        expectedVD = {0, 4, 5, 6, 7};
        break;
      case 8:
        expectedVD = {0, 4, 5, 6, 8};
        break;
      case 9:
        expectedVD = {0, 4, 5, 9};
        break;
      case 10:
        expectedVD = {0, 10};
        break;
      case 11:
        expectedVD = {0, 10, 11};
        break;
      case 12:
        expectedVD = {0, 10, 11, 12};
        break;
      case 13:
        expectedVD = {0, 10, 11, 13};
        break;
    }

    for (auto vd : expectedVD) {
      expectedV.insert(index->at(vd));
    }

    EXPECT_EQ(expectedVD, domSetVD);
    EXPECT_EQ(expectedV.size(), domSetV.size());

    auto domSetVIt = domSetV.begin();
    auto expectedVIt = expectedV.begin();
    while (domSetVIt != domSetV.end() && expectedVIt != expectedV.end()) {
      // TODO find reason why shared pointers need << of *node
      EXPECT_EQ((*expectedVIt++).get(), (*domSetVIt++).get());
    }
  }
}

TEST(Cfg, SuccessorSet) {
  auto tree = parser::parse(
      R"(
         {
            int x=1;
            float y = 3.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto index = tac.getBasicBlockIndex();

  for (auto bb : *index.get()) {
    auto succSetV = graph->getSuccessor(bb);
    auto succSetVD = graph->getSuccessor(bb->getBlockId());

    std::set<Vertex> expectedV;
    std::set<VertexDescriptor> expectedVD;
    switch (bb->getBlockId()) {
      case 0:
        expectedVD = {1, 4};
        break;
      case 1:
        expectedVD = {2, 3};
        break;
      case 2:
        expectedVD = {1};
        break;
      case 3:
        expectedVD = {10};
        break;
      case 4:
        expectedVD = {5};
        break;
      case 5:
        expectedVD = {6, 9};
        break;
      case 6:
        expectedVD = {7, 8};
        break;
      case 7:
        expectedVD = {6};
        break;
      case 8:
        expectedVD = {5};
        break;
      case 9:
        expectedVD = {10};
        break;
      case 10:
        expectedVD = {11};
        break;
      case 11:
        expectedVD = {12, 13};
        break;
      case 12:
        expectedVD = {11};
        break;
      case 13:
        expectedVD = {};
        break;
    }

    for (auto vd : expectedVD) {
      expectedV.insert(index->at(vd));
    }

    EXPECT_EQ(expectedVD, succSetVD);
    EXPECT_EQ(expectedV.size(), succSetV.size());

    auto succSetVIt = succSetV.begin();
    auto expectedVIt = expectedV.begin();
    while (succSetVIt != succSetV.end() && expectedVIt != expectedV.end()) {
      // TODO find reason why shared pointers need << of *node
      EXPECT_EQ((*expectedVIt++).get(), (*succSetVIt++).get());
    }
  }
}

TEST(Cfg, PredecessorSet) {
  auto tree = parser::parse(
      R"(
         {
            int x=1;
            float y = 3.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto index = tac.getBasicBlockIndex();

  for (auto bb : *index.get()) {
    auto predSetV = graph->getPredecessor(bb);
    auto predSetVD = graph->getPredecessor(bb->getBlockId());

    std::set<Vertex> expectedV;
    std::set<VertexDescriptor> expectedVD;
    switch (bb->getBlockId()) {
      case 0:
        expectedVD = {};
        break;
      case 1:
        expectedVD = {0, 2};
        break;
      case 2:
        expectedVD = {1};
        break;
      case 3:
        expectedVD = {1};
        break;
      case 4:
        expectedVD = {0};
        break;
      case 5:
        expectedVD = {4, 8};
        break;
      case 6:
        expectedVD = {5, 7};
        break;
      case 7:
        expectedVD = {6};
        break;
      case 8:
        expectedVD = {6};
        break;
      case 9:
        expectedVD = {5};
        break;
      case 10:
        expectedVD = {3, 9};
        break;
      case 11:
        expectedVD = {10, 12};
        break;
      case 12:
        expectedVD = {11};
        break;
      case 13:
        expectedVD = {11};
        break;
    }

    for (auto vd : expectedVD) {
      expectedV.insert(index->at(vd));
    }

    EXPECT_EQ(expectedVD, predSetVD);
    EXPECT_EQ(expectedV.size(), predSetV.size());

    auto predSetVIt = predSetV.begin();
    auto expectedVIt = expectedV.begin();
    while (predSetVIt != predSetV.end() && expectedVIt != expectedV.end()) {
      // TODO find reason why shared pointers need << of *node
      EXPECT_EQ((*expectedVIt++).get(), (*predSetVIt++).get());
    }
  }
}

TEST(Cfg, VariableSetSize) {
  auto tree = parser::parse(
      R"(
         {
            int x=1;
            float y = 3.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  EXPECT_EQ(11, graph->variableSetSize());
}

TEST(Cfg, ComputeLiveInOut) {
  auto tree = parser::parse(
      R"(
         {
            int x=1;
            float y = 3.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  graph->computeLiveInOut();

  auto liveInSet = helper::LiveSetGen(tac);
  liveInSet.addVariable(0, 1, 1);   // y in BB1
  liveInSet.addVariable(0, 1, 2);   // y in BB2
  liveInSet.addVariable(0, 1, 5);   // y in BB5
  liveInSet.addVariable(0, 1, 6);   // y in BB6
  liveInSet.addVariable(0, 1, 7);   // y in BB7
  liveInSet.addVariable(0, 1, 11);  // y in BB11
  liveInSet.addVariable(0, 1, 12);  // y in BB12

  unsigned bbId = 0;
  for (auto const& set : liveInSet.expected) {
    EXPECT_EQ(set.size(), graph->getLiveIn(bbId).size());
    for (auto const var : set) {
      EXPECT_NE(graph->getLiveIn(bbId).end(), graph->getLiveIn(bbId).find(var));
    }
    ++bbId;
  }

  auto liveOutSet = helper::LiveSetGen(tac);
  liveOutSet.addVariable(0, 1, 0);   // y in BB0
  liveOutSet.addVariable(0, 1, 1);   // y in BB1
  liveOutSet.addVariable(0, 1, 2);   // y in BB2
  liveOutSet.addVariable(0, 1, 4);   // y in BB4
  liveOutSet.addVariable(0, 1, 5);   // y in BB5
  liveOutSet.addVariable(0, 1, 6);   // y in BB6
  liveOutSet.addVariable(0, 1, 7);   // y in BB7
  liveOutSet.addVariable(0, 1, 8);   // y in BB8
  liveOutSet.addVariable(0, 1, 10);  // y in BB10
  liveOutSet.addVariable(0, 1, 11);  // y in BB11
  liveOutSet.addVariable(0, 1, 12);  // y in BB12

  bbId = 0;
  for (auto const& set : liveOutSet.expected) {
    EXPECT_EQ(set.size(), graph->getLiveOut(bbId).size());
    for (auto const var : set) {
      EXPECT_NE(graph->getLiveOut(bbId).end(),
                graph->getLiveOut(bbId).find(var));
    }
    ++bbId;
  }
}

TEST(Cfg, ComputeWorkList) {
  auto tree = parser::parse(
      R"(
         {
            int x=1;
            float y = 3.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  graph->computeWorkList();

  auto liveInSet = helper::LiveSetGen(tac);
  liveInSet.addVariable(0, 1, 1);   // y in BB1
  liveInSet.addVariable(0, 1, 2);   // y in BB2
  liveInSet.addVariable(0, 1, 5);   // y in BB5
  liveInSet.addVariable(0, 1, 6);   // y in BB6
  liveInSet.addVariable(0, 1, 7);   // y in BB7
  liveInSet.addVariable(0, 1, 11);  // y in BB11
  liveInSet.addVariable(0, 1, 12);  // y in BB12

  unsigned bbId = 0;
  for (auto const& set : liveInSet.expected) {
    EXPECT_EQ(set.size(), graph->getLiveIn(bbId).size());
    for (auto const var : set) {
      EXPECT_NE(graph->getLiveIn(bbId).end(), graph->getLiveIn(bbId).find(var));
    }
    ++bbId;
  }

  auto liveOutSet = helper::LiveSetGen(tac);
  liveOutSet.addVariable(0, 1, 0);   // y in BB0
  liveOutSet.addVariable(0, 1, 1);   // y in BB1
  liveOutSet.addVariable(0, 1, 2);   // y in BB2
  liveOutSet.addVariable(0, 1, 4);   // y in BB4
  liveOutSet.addVariable(0, 1, 5);   // y in BB5
  liveOutSet.addVariable(0, 1, 6);   // y in BB6
  liveOutSet.addVariable(0, 1, 7);   // y in BB7
  liveOutSet.addVariable(0, 1, 8);   // y in BB8
  liveOutSet.addVariable(0, 1, 10);  // y in BB10
  liveOutSet.addVariable(0, 1, 11);  // y in BB11
  liveOutSet.addVariable(0, 1, 12);  // y in BB12

  bbId = 0;
  for (auto const& set : liveOutSet.expected) {
    EXPECT_EQ(set.size(), graph->getLiveOut(bbId).size());
    for (auto const var : set) {
      EXPECT_NE(graph->getLiveOut(bbId).end(),
                graph->getLiveOut(bbId).find(var));
    }
    ++bbId;
  }
}

TEST(Cfg, LiveSetAt) {
  auto tree = parser::parse(
      R"(
         {
            int x=1;
            float y = 3.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto expected = helper::LiveSetGen(tac);
  // BB1
  expected.addVariable(0, 1, 1);  // y
  // BB2
  expected.addVariable(0, 1, 2);  // y
  // BB4
  expected.addVariable(0, 1, 4);  // y
  // BB5
  expected.addVariable(0, 1, 5);  // y
  // BB6
  expected.addVariable(0, 1, 6);  // y
  // BB7
  expected.addVariable(0, 1, 7);  // y
  // BB8
  expected.addVariable(0, 1, 8);  // y
  // BB10
  expected.addVariable(0, 1, 10);  // y
  // BB11
  expected.addVariable(0, 1, 11);  // y
  // BB12
  expected.addVariable(0, 1, 12);  // y

  for (auto const bb : *tac.getBasicBlockIndex().get()) {
    auto bbId = bb->getBlockId();

    auto liveSet = graph->liveSetAt(bb->getBlockMembers().front());

    EXPECT_EQ(expected.expected.at(bbId).size(), liveSet.size());
    for (auto const var : expected.expected.at(bbId)) {
      EXPECT_NE(liveSet.end(), liveSet.find(var));
    }
  }
}

TEST(Cfg, LiveSetAfter) {
  auto tree = parser::parse(
      R"(
         {
            int x=1;
            float y = 3.0;

            if(x > 0) {
              while(y < 5.0) {
                  y = y * 1.5;
              }
            } else {
              while(y < 5.0) {
                  while(y < 10.0) {
                      y = y + 2.0;
                  }
              }
            }

            while(y < 10.0) {
              y = y + 1.0;
            }

            int a = 0;
         })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto expected = helper::LiveSetGen(tac);
  // BB0
  expected.addVariable(0, 0, 0);  // x
  // BB1
  expected.addVariable(0, 1, 1);  // y
  // BB2
  expected.addVariable(2, 0, 2);  // tempVar
  // BB4
  expected.addVariable(0, 1, 4);  // y
  // BB5
  expected.addVariable(0, 1, 5);  // y
  // BB6
  expected.addVariable(0, 1, 6);  // y
  // BB7
  expected.addVariable(7, 0, 7);  // tempVar
  // BB8
  expected.addVariable(0, 1, 8);  // y
  // BB10
  expected.addVariable(0, 1, 10);  // y
  // BB11
  expected.addVariable(0, 1, 11);  // y
  // BB12
  expected.addVariable(12, 0, 12);  // tempVar

  for (auto const bb : *tac.getBasicBlockIndex().get()) {
    auto bbId = bb->getBlockId();

    auto liveSet = graph->liveSetAfter(bb->getBlockMembers().front());

    EXPECT_EQ(expected.expected.at(bbId).size(), liveSet.size());
    for (auto const var : expected.expected.at(bbId)) {
      EXPECT_NE(liveSet.end(), liveSet.find(var));
    }
  }
}
}
}
