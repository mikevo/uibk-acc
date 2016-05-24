#include <gtest/gtest.h>

#include "mcc/gas/register_manager.h"

#include "parser.h"
#include "mcc/tac/tac.h"

namespace mcc {
namespace gas {

TEST(RegisterManager, InterfernceGraph) {
  auto tree = parser::parse(
      R"(
           void bar(int arg1);
    
           void foo(int arg1, int arg2) {
              if(arg1 < 0) return;
              bar(arg1 - 1);
           }
           
           void bar(int arg1) {
              if(arg1 < 0) return;
              foo(arg1 - 1, arg1);
              bar(arg1 - 1);
           }   
              
            int main() {
               foo(10, 10);
               return 0;
            }
          )");

  mcc::tac::Tac tac = mcc::tac::Tac(tree);
  RegisterManager regMan = RegisterManager(tac);

  std::map<mcc::tac::Label::ptr_t, std::string, mcc::tac::Label::less> expected;

  auto l1 = std::make_shared<mcc::tac::Label>("foo");
  auto l2 = std::make_shared<mcc::tac::Label>("bar");
  auto l3 = std::make_shared<mcc::tac::Label>("main");

  expected[l1] = R"(graph G {
0;
1;
2;
3;
4;
1--0 ;
2--0 ;
3--0 ;
}
)";

  expected[l2] = R"(graph G {
0;
1;
2;
3;
4;
5;
1--0 ;
2--0 ;
3--0 ;
4--0 ;
}
)";

  expected[l3] = R"(graph G {
0;
}
)";

  EXPECT_EQ(expected.size(), tac.getFunctionMap()->size());

  for (auto const e : expected) {
    EXPECT_EQ(e.second, regMan.toDot(e.first));
  }

  for (auto f : *tac.getFunctionMap().get()) {
    regMan.storeDot("/tmp/" + f.first + ".dot", f.second);
  }
}
}
}
