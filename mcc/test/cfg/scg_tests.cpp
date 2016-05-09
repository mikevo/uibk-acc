#include <gtest/gtest.h>

#include "mcc/cfg/static_call_graph.h"

#include "boost/graph/graphviz.hpp"
#include "parser.h"
#include "test_utils.h"

#include <iostream>

namespace mcc {
namespace cfg {
TEST(SCG, FunctionCall) {
  auto tree = parser::parse(
      R"(
          int foobar(int arg1, int arg2) {
            return arg1 + arg2;
          }
            
            int main() {
                foobar(3, 6);
                return 0;
            }
        )");

  Tac tac = Tac(tree);
  auto callgraph = std::make_shared<Scg>(tac);
 
  std::string expected =
      R"(digraph G {
0[label=foobar];
1[label=main];
1->0 ;
}
)";

 EXPECT_EQ(expected, callgraph->toDot());
}

TEST(SCG, FunctionRecursion) {
  auto tree = parser::parse(
      R"(
         void foobar(int arg1) {
            if(arg1 == 0) return;
            foobar(arg1 - 1);
         }   
            
         int main() {
            foobar(5);
            return 0;
         }   
        )");

  Tac tac = Tac(tree);
  auto callgraph = std::make_shared<Scg>(tac);
  std::cout << callgraph->toDot();
  std::string expected =
      R"(digraph G {
0[label=foobar];
1[label=main];
0->0 ;
1->0 ;
}
)";

 EXPECT_EQ(expected, callgraph->toDot());

}

TEST(SCG, MutualRecursion) {
  auto tree = parser::parse(
      R"(
         void bar(int arg1);
  
         void foo(int arg1) {
            if(arg1 < 0) return;
            bar(arg1 - 1);
         }
         
         void bar(int arg1) {
            if(arg1 < 0) return;
            foo(arg1 - 1);
         }   
            
          int main() {
             foo(10);
             return 0;
          }
        )");

 Tac tac = Tac(tree);
  auto callgraph = std::make_shared<Scg>(tac);
  std::cout << callgraph->toDot();
  std::string expected =
      R"(digraph G {
0[label=bar];
1[label=foo];
2[label=main];
1->0 ;
0->1 ;
2->1 ;
}
)";

 EXPECT_EQ(expected, callgraph->toDot());
}

}
}
