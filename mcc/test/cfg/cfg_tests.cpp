#include <gtest/gtest.h>

#include "mcc/cfg/cfg.h"

#include "boost/graph/graphviz.hpp"
#include "parser.h"

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
0->1 ;
0->2 ;
1->3 ;
2->3 ;
3->4 ;
3->5 ;
4->6 ;
5->6 ;
}
)";

  EXPECT_EQ(expected, graph->toDot());
}

TEST(Cfg, Idom) {
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

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto index = tac.getBasicBlockIndex();

  // TODO find reason why shared pointers need << of *node
  EXPECT_EQ(graph->getIdom(index->at(0)).get(), index->at(0).get());
  EXPECT_EQ(graph->getIdom(index->at(1)).get(), index->at(0).get());
  EXPECT_EQ(graph->getIdom(index->at(2)).get(), index->at(0).get());
  EXPECT_EQ(graph->getIdom(index->at(3)).get(), index->at(0).get());
  EXPECT_EQ(graph->getIdom(index->at(4)).get(), index->at(3).get());
  EXPECT_EQ(graph->getIdom(index->at(5)).get(), index->at(3).get());
  EXPECT_EQ(graph->getIdom(index->at(6)).get(), index->at(3).get());

  EXPECT_EQ(graph->getIdom(0), 0);
  EXPECT_EQ(graph->getIdom(1), 0);
  EXPECT_EQ(graph->getIdom(2), 0);
  EXPECT_EQ(graph->getIdom(3), 0);
  EXPECT_EQ(graph->getIdom(4), 3);
  EXPECT_EQ(graph->getIdom(5), 3);
  EXPECT_EQ(graph->getIdom(6), 3);
}

TEST(Cfg, DomSetVertex) {
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

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto index = tac.getBasicBlockIndex();

  auto dom0 = graph->getDomSet(index->at(0));
  auto dom1 = graph->getDomSet(index->at(1));
  auto dom2 = graph->getDomSet(index->at(2));
  auto dom3 = graph->getDomSet(index->at(3));
  auto dom4 = graph->getDomSet(index->at(4));
  auto dom5 = graph->getDomSet(index->at(5));
  auto dom6 = graph->getDomSet(index->at(6));

  // TODO find reason why shared pointers need << of *node
  EXPECT_EQ(dom0.begin()->get(), index->at(0).get());
  EXPECT_EQ(dom1.begin()->get(), index->at(0).get());
  EXPECT_EQ(dom2.begin()->get(), index->at(0).get());
  EXPECT_EQ(dom3.begin()->get(), index->at(0).get());

  std::set<mcc::tac::BasicBlock::ptr_t> bbSet;

  for (auto const block : dom4) {
    bbSet.insert(block);
  }

  EXPECT_NE(bbSet.find(index->at(0)), bbSet.end());
  EXPECT_NE(bbSet.find(index->at(3)), bbSet.end());

  bbSet.clear();

  for (auto& block : dom5) {
    bbSet.insert(block);
  }

  EXPECT_NE(bbSet.find(index->at(0)), bbSet.end());
  EXPECT_NE(bbSet.find(index->at(3)), bbSet.end());

  bbSet.clear();

  for (auto& block : dom6) {
    bbSet.insert(block);
  }

  EXPECT_NE(bbSet.find(index->at(0)), bbSet.end());
  EXPECT_NE(bbSet.find(index->at(3)), bbSet.end());
}

TEST(Cfg, DomSet) {
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

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  std::vector<VertexDescriptor> set;
  set.push_back(0);
  set.push_back(3);

  auto dom0 = graph->getDomSet(0);
  auto dom1 = graph->getDomSet(1);
  auto dom2 = graph->getDomSet(2);
  auto dom3 = graph->getDomSet(3);
  auto dom4 = graph->getDomSet(4);
  auto dom5 = graph->getDomSet(5);
  auto dom6 = graph->getDomSet(6);

  EXPECT_NE(dom0.find(0), dom0.end());
  EXPECT_NE(dom1.find(0), dom1.end());
  EXPECT_NE(dom2.find(0), dom2.end());
  EXPECT_NE(dom3.find(0), dom3.end());

  for (auto e : set) {
    EXPECT_NE(dom4.find(e), dom4.end());
    EXPECT_NE(dom5.find(e), dom5.end());
    EXPECT_NE(dom6.find(e), dom6.end());
  }
}

TEST(Cfg, SuccessorSet) {
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

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto succ0 = graph->getSuccessor(0);
  auto succ1 = graph->getSuccessor(1);
  auto succ2 = graph->getSuccessor(2);
  auto succ3 = graph->getSuccessor(3);
  auto succ4 = graph->getSuccessor(4);
  auto succ5 = graph->getSuccessor(5);
  auto succ6 = graph->getSuccessor(6);

  EXPECT_EQ(succ0.size(), 2);
  EXPECT_NE(succ0.find(1), succ0.end());
  EXPECT_NE(succ0.find(2), succ0.end());

  EXPECT_EQ(succ1.size(), 1);
  EXPECT_NE(succ1.find(3), succ1.end());

  EXPECT_EQ(succ2.size(), 1);
  EXPECT_NE(succ2.find(3), succ2.end());

  EXPECT_EQ(succ3.size(), 2);
  EXPECT_NE(succ3.find(4), succ3.end());
  EXPECT_NE(succ3.find(5), succ3.end());

  EXPECT_EQ(succ4.size(), 1);
  EXPECT_NE(succ4.find(6), succ3.end());

  EXPECT_EQ(succ5.size(), 1);
  EXPECT_NE(succ5.find(6), succ3.end());

  EXPECT_EQ(succ6.size(), 0);
}

TEST(Cfg, PredecessorSet) {
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

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto pred0 = graph->getPredecessor(0);
  auto pred1 = graph->getPredecessor(1);
  auto pred2 = graph->getPredecessor(2);
  auto pred3 = graph->getPredecessor(3);
  auto pred4 = graph->getPredecessor(4);
  auto pred5 = graph->getPredecessor(5);
  auto pred6 = graph->getPredecessor(6);

  EXPECT_EQ(pred0.size(), 0);

  EXPECT_EQ(pred1.size(), 1);
  EXPECT_NE(pred1.find(0), pred1.end());

  EXPECT_EQ(pred2.size(), 1);
  EXPECT_NE(pred2.find(0), pred2.end());

  EXPECT_EQ(pred3.size(), 2);
  EXPECT_NE(pred3.find(2), pred3.end());
  EXPECT_NE(pred3.find(1), pred3.end());

  EXPECT_EQ(pred4.size(), 1);
  EXPECT_NE(pred4.find(3), pred3.end());

  EXPECT_EQ(pred5.size(), 1);
  EXPECT_NE(pred5.find(3), pred3.end());

  EXPECT_EQ(pred6.size(), 2);
  EXPECT_NE(pred6.find(4), pred0.end());
  EXPECT_NE(pred6.find(5), pred0.end());
}

TEST(Cfg, VariableSet) {
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

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  EXPECT_EQ(7, graph->variableSetSize());
}

TEST(Cfg, ComputeLiveInOut) {
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
                                            x = a;
                                            a = 1;
                                        } else {
                                            a = 2;
                                        }
                                      })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  graph->computeLiveInOut();

  auto liveInSet = helper::LiveSetGen(tac);
  liveInSet.addVariable(1, 1, 1);
  liveInSet.addVariable(2, 2, 2);
  liveInSet.addVariable(4, 1, 4);

  unsigned bbId = 0;
  for (auto const& set : liveInSet.expected) {
    EXPECT_EQ(set.size(), graph->getLiveIn(bbId).size());
    for (auto const var : set) {
      EXPECT_NE(graph->getLiveIn(bbId).end(), graph->getLiveIn(bbId).find(var));
    }
    ++bbId;
  }

  auto liveOutSet = helper::LiveSetGen(tac);
  liveOutSet.addVariable(0, 1, 0);
  liveOutSet.addVariable(3, 1, 3);

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
                                               y = y * 1.5;
                                            } else {
                                               y = y + 2.0;
                                            }

                                            int a = 0;

                                            if( 1 <= 2) {
                                                x = a;
                                                a = 1;
                                            } else {
                                                a = 2;
                                            }
                                          })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  graph->computeWorkList();

  auto liveInSet = helper::LiveSetGen(tac);
  liveInSet.addVariable(1, 1, 1);
  liveInSet.addVariable(2, 2, 2);
  liveInSet.addVariable(4, 1, 4);

  unsigned bbId = 0;
  for (auto const& set : liveInSet.expected) {
    EXPECT_EQ(set.size(), graph->getLiveIn(bbId).size());
    for (auto const var : set) {
      EXPECT_NE(graph->getLiveIn(bbId).end(), graph->getLiveIn(bbId).find(var));
    }
    ++bbId;
  }

  auto liveOutSet = helper::LiveSetGen(tac);
  liveOutSet.addVariable(0, 1, 0);
  liveOutSet.addVariable(3, 1, 3);

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

TEST(Cfg, IsLiveAt) {
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
                                                x = a;
                                                a = 1;
                                            } else {
                                                a = 2;
                                            }
                                          })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  for (auto const bb : *tac.getBasicBlockIndex().get()) {
    auto bbId = bb->getBlockId();

    auto liveSet = graph->isLiveAt(bb, bb->getBlockMembers().front());

    EXPECT_EQ(graph->getLiveIn(bbId).size(), liveSet.size());
    for (auto const var : graph->getLiveIn(bbId)) {
      EXPECT_NE(liveSet.end(), liveSet.find(var));
    }
  }
}

TEST(Cfg, IsLiveAfter) {
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
                                                x = a;
                                                a = 1;
                                            } else {
                                                a = 2;
                                            }
                                          })");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  auto graph = std::make_shared<Cfg>(tac);

  auto liveSet = helper::LiveSetGen(tac);
  liveSet.addVariable(0, 0, 0);  // x0:1:0
  liveSet.addVariable(1, 0, 1);  // $t157
  liveSet.addVariable(2, 2, 2);  // y0:1:0

  for (auto const bb : *tac.getBasicBlockIndex().get()) {
    auto bbId = bb->getBlockId();

    auto live = graph->isLiveAfter(bb, bb->getBlockMembers().front());

    EXPECT_EQ(liveSet.expected.at(bbId).size(), live.size());
    for (auto const var : liveSet.expected.at(bbId)) {
      EXPECT_NE(live.end(), live.find(var));
    }
  }
}
}
}
