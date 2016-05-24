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

TEST(RegisterManager, GraphColoringNumColors) {
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
  auto fooNumCol = regMan.graphColoring(foo);
  auto barNumCol = regMan.graphColoring(bar);
  auto mainNumCol = regMan.graphColoring(main);

  auto numColorMap = regMan.getNumColorsMap();

  EXPECT_EQ(2, fooNumCol);
  EXPECT_EQ(2, barNumCol);
  EXPECT_EQ(1, mainNumCol);

  EXPECT_EQ(3, numColorMap->size());
  EXPECT_EQ(2, numColorMap->at(fooLabel));
  EXPECT_EQ(2, numColorMap->at(barLabel));
  EXPECT_EQ(1, numColorMap->at(mainLabel));

  // test for function using labels
  numColorMap->clear();

  fooNumCol = regMan.graphColoring(fooLabel);
  barNumCol = regMan.graphColoring(barLabel);
  mainNumCol = regMan.graphColoring(mainLabel);

  EXPECT_EQ(2, fooNumCol);
  EXPECT_EQ(2, barNumCol);
  EXPECT_EQ(1, mainNumCol);

  EXPECT_EQ(3, numColorMap->size());
  EXPECT_EQ(2, numColorMap->at(fooLabel));
  EXPECT_EQ(2, numColorMap->at(barLabel));
  EXPECT_EQ(1, numColorMap->at(mainLabel));

  // test for function calling subsequent functions for each function label
  numColorMap->clear();

  regMan.graphColoring();

  EXPECT_EQ(3, numColorMap->size());
  EXPECT_EQ(2, numColorMap->at(fooLabel));
  EXPECT_EQ(2, numColorMap->at(barLabel));
  EXPECT_EQ(1, numColorMap->at(mainLabel));
}

TEST(RegisterManager, GraphColoringColors) {
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

  regMan.graphColoring(foo);
  regMan.graphColoring(bar);
  regMan.graphColoring(main);

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

  // test for function using labels
  functionGraphColorsMap->clear();

  regMan.graphColoring(fooLabel);
  regMan.graphColoring(barLabel);
  regMan.graphColoring(mainLabel);

  EXPECT_EQ(3, functionGraphColorsMap->size());

  // foo
  fooColors = functionGraphColorsMap->at(fooLabel);
  EXPECT_EQ(5, fooColors->size());
  fooColExp = {
      {0, 1}, {1, 0}, {2, 0}, {3, 0}, {4, 0},
  };

  expectColorMapsEQ(fooColExp, *fooColors);

  // bar
  barColors = functionGraphColorsMap->at(barLabel);
  EXPECT_EQ(6, barColors->size());
  barColExp = {
      {0, 1}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0},
  };

  expectColorMapsEQ(barColExp, *barColors);

  // main
  mainColors = functionGraphColorsMap->at(mainLabel);
  EXPECT_EQ(1, mainColors->size());
  mainColExp = {
      {0, 0},
  };

  expectColorMapsEQ(mainColExp, *mainColors);

  // test for function calling subsequent functions for each function label
  functionGraphColorsMap->clear();

  regMan.graphColoring();

  EXPECT_EQ(3, functionGraphColorsMap->size());

  // foo
  fooColors = functionGraphColorsMap->at(fooLabel);
  EXPECT_EQ(5, fooColors->size());
  fooColExp = {
      {0, 1}, {1, 0}, {2, 0}, {3, 0}, {4, 0},
  };

  expectColorMapsEQ(fooColExp, *fooColors);

  // bar
  barColors = functionGraphColorsMap->at(barLabel);
  EXPECT_EQ(6, barColors->size());
  barColExp = {
      {0, 1}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0},
  };

  expectColorMapsEQ(barColExp, *barColors);

  // main
  mainColors = functionGraphColorsMap->at(mainLabel);
  EXPECT_EQ(1, mainColors->size());
  mainColExp = {
      {0, 0},
  };

  expectColorMapsEQ(mainColExp, *mainColors);
}
}
}
