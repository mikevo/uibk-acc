#include <gtest/gtest.h>

#include "mcc/tac/tac.h"

#include <memory>
#include <string>

#include "ast.h"
#include "mcc/tac/type.h"
#include "parser.h"

namespace mcc {
    namespace tac {

        TEST(Function, FunctionDefTAC) {
            auto tree = parser::parse(
                    R"(
          int foobar(int arg1, float arg2) {
            arg1 * 5;
            arg2 = 4.0;
            return arg1;
          }
        )");

            Tac tac = Tac(tree);
            std::cout << tac.toString() << std::endl;
        }

        TEST(Function, FunctionCallTAC) {
            auto tree = parser::parse(
                    R"(
          int foobar(int arg1, int arg2) {
            return arg1 + arg2;
          }
            
            int main() {
                foobar(3, 6);
            }
        )");

            Tac tac = Tac(tree);
            std::cout << tac.toString() << std::endl;
        }
        
          TEST(Function, FunctionRecursion) {
            auto tree = parser::parse(
                    R"(
         void foobar(int arg1) {
            if(arg1 == 0) return;
            foobar(arg1 - 1);
         }   
            
         int main() {
            foobar(5);
         }   
        )");

            Tac tac = Tac(tree);
            std::cout << tac.toString() << std::endl;
        }
    }
}
