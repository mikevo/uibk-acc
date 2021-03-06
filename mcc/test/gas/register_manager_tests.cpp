#include <gtest/gtest.h>

#include "mcc/gas/register_manager.h"

#include "parser.h"
#include "mcc/tac/tac.h"

namespace mcc {
namespace gas {

void expectColorMapsEQ(std::map<long unsigned, unsigned> expected,
                       std::map<long unsigned, unsigned> actual) {
  auto expIt = expected.begin();
  auto actIt = actual.begin();
  while (expIt != expected.end()) {
    EXPECT_EQ(expIt->first, actIt->first);
    EXPECT_EQ(expIt->second, actIt->second);

    ++expIt;
    ++actIt;
  }
}

TEST(RegisterManager, InterfernceGraph) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

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

  auto l1 = std::make_shared<mcc::tac::Label>("foo", scope);
  auto l2 = std::make_shared<mcc::tac::Label>("bar", scope);
  auto l3 = std::make_shared<mcc::tac::Label>("main", scope);

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

TEST(RegisterManager, GraphColoringNumColors) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  // TODO re-implement
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

  std::string foo = "foo";
  std::string bar = "bar";
  std::string main = "main";

  auto fooLabel = tac.lookupFunction(foo);
  auto barLabel = tac.lookupFunction(bar);
  auto mainLabel = tac.lookupFunction(main);

  unsigned maxNumOfColors = regMan.getNumOfRegForColoring();

  auto numColorMap = regMan.getNumColorsMap();

  EXPECT_EQ(3, numColorMap->size());
  // expect less or equal number of given colors needed to color graph
  EXPECT_GE(maxNumOfColors, numColorMap->at(fooLabel));
  EXPECT_GE(maxNumOfColors, numColorMap->at(barLabel));
  EXPECT_GE(maxNumOfColors, numColorMap->at(mainLabel));
}

TEST(RegisterManager, GraphColoringColors) {
  Scope::ptr_t scope = std::make_shared<Scope>(0, 0);

  // TODO re-implement
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

  std::string foo = "foo";
  std::string bar = "bar";
  std::string main = "main";

  auto fooLabel = tac.lookupFunction(foo);
  auto barLabel = tac.lookupFunction(bar);
  auto mainLabel = tac.lookupFunction(main);

  // test for function using strings
  auto functionGraphColorsMap = regMan.getFunctionGraphColorsMap();

  EXPECT_EQ(3, functionGraphColorsMap->size());

  // foo
  auto fooColors = functionGraphColorsMap->at(fooLabel);
  EXPECT_EQ(5, fooColors->size());

  std::map<long unsigned, unsigned> fooColExp = {
      {0, 1}, {1, 0}, {2, 0}, {3, 0}, {4, 0},
  };

  expectColorMapsEQ(fooColExp, *fooColors);

  // bar
  auto barColors = functionGraphColorsMap->at(barLabel);
  EXPECT_EQ(6, barColors->size());

  std::map<long unsigned, unsigned> barColExp = {
      {0, 1}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0},
  };

  expectColorMapsEQ(barColExp, *barColors);

  // main
  auto mainColors = functionGraphColorsMap->at(mainLabel);
  EXPECT_EQ(1, mainColors->size());

  std::map<long unsigned, unsigned> mainColExp = {
      {0, 0},
  };

  expectColorMapsEQ(mainColExp, *mainColors);
}
}
}
