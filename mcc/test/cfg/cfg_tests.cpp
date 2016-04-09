#include <gtest/gtest.h>

#include "mcc/cfg/cfg.h"

#include "parser.h"

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
3->4 ;
3->5 ;
4->6 ;
}
)";

      EXPECT_EQ(expected, graph->toDot());
    }
  }
}

