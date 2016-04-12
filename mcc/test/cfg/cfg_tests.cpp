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

      // TODO find reason why shared pointers as well as the results of get() are not equal
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

    /*TEST(Cfg, DomSetVertex) {
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

      std::vector<Vertex> set;
      set.push_back(index[0]);
      set.push_back(index[3]);

      auto dom0 = graph->getDomSet(index[0]).begin();
      auto dom1 = graph->getDomSet(index[1]).begin();
      auto dom2 = graph->getDomSet(index[2]).begin();
      auto dom3 = graph->getDomSet(index[3]).begin();
      auto dom4 = graph->getDomSet(index[4]).begin();
      auto dom5 = graph->getDomSet(index[5]).begin();
      auto dom6 = graph->getDomSet(index[6]).begin();

      // TODO find reason why shared pointers as well as the results of get() are not equal
      EXPECT_EQ(dom0->get()->toString(), index[0]->toString());
      EXPECT_EQ(dom1->get()->toString(), index[0]->toString());
      EXPECT_EQ(dom2->get()->toString(), index[0]->toString());
      EXPECT_EQ(dom3->get()->toString(), index[0]->toString());
      EXPECT_EQ(dom4->get()->toString(), index[0]->toString());
      EXPECT_EQ(dom5->get()->toString(), index[0]->toString());
      EXPECT_EQ(dom6->get()->toString(), index[0]->toString());

      EXPECT_EQ((++dom4)->get()->toString(), index[3]->toString());
      EXPECT_EQ((++dom5)->get()->toString(), index[3]->toString());
      EXPECT_EQ((++dom6)->get()->toString(), index[3]->toString());
    }*/

    TEST(Cfg, DomSet) {
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
  }
}

