#include <gtest/gtest.h>

#include "mcc/gas/register_manager.h"

#include "parser.h"
#include "mcc/tac/tac.h"

TEST(RegisterManager, DISABLED_InterfernceGraph) {
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
}
