#include <gtest/gtest.h>

#include "mcc/cfg/cfg.h"

#include "parser.h"
#include "boost/graph/graphviz.hpp"

namespace mcc {
  namespace cfg {

    TEST(Cfg, Cfg) {
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

      mcc::tac::Tac tac;
      tac.convertAst(tree);
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

      mcc::tac::Tac tac;
      tac.convertAst(tree);
      auto graph = std::make_shared<Cfg>(tac);

      auto index = tac.getBasicBlockIndex();

      EXPECT_EQ(graph->getIdom(index[0])->toString(), index[0]->toString());
      EXPECT_EQ(graph->getIdom(index[1])->toString(), index[0]->toString());
      EXPECT_EQ(graph->getIdom(index[2])->toString(), index[0]->toString());
      EXPECT_EQ(graph->getIdom(index[3])->toString(), index[0]->toString());
      EXPECT_EQ(graph->getIdom(index[4])->toString(), index[3]->toString());
      EXPECT_EQ(graph->getIdom(index[5])->toString(), index[3]->toString());
      EXPECT_EQ(graph->getIdom(index[6])->toString(), index[3]->toString());

      EXPECT_EQ(graph->getIdom(0), 0);
      EXPECT_EQ(graph->getIdom(1), 0);
      EXPECT_EQ(graph->getIdom(2), 0);
      EXPECT_EQ(graph->getIdom(3), 0);
      EXPECT_EQ(graph->getIdom(4), 3);
      EXPECT_EQ(graph->getIdom(5), 3);
      EXPECT_EQ(graph->getIdom(6), 3);
    }
  }
}

