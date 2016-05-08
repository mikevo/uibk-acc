#include <gtest/gtest.h>

#include "mcc/tac/tac.h"
#include "mcc/cfg/StaticCallGraph.h"

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
          int foobar(int arg1, int arg2) {
            return arg1 + arg2;
          }
        )");
            
        Tac tac = Tac(tree);
        
        auto tempVarTriple = tac.codeLines.begin() + 3;
        auto tempVarID = (*tempVarTriple)->getValue();
            
        std::string expectedValue = "LABEL foobar\n";
        expectedValue.append("arg10:1:0 = POP\n");
        expectedValue.append("arg20:1:0 = POP\n");
        expectedValue.append(tempVarID + " = arg10:1:0 + arg20:1:0\n");
        expectedValue.append("RET " + tempVarID);
 
        EXPECT_EQ(5, tac.codeLines.size());
        EXPECT_EQ(expectedValue, tac.toString());
         
        }

        TEST(Function, FunctionCallTAC) {
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
        
        auto tempVarTriple = tac.codeLines.begin() + 3;
        auto tempVarID = (*tempVarTriple)->getValue();
        
        auto tempCallTriple = tac.codeLines.begin() + 8;
        auto tempCallID = (*tempCallTriple)->getValue();
            
        std::string expectedValue = "LABEL foobar\n";
        expectedValue.append("arg10:1:0 = POP\n");
        expectedValue.append("arg20:1:0 = POP\n");
        expectedValue.append(tempVarID + " = arg10:1:0 + arg20:1:0\n");
        expectedValue.append("RET " + tempVarID + "\n");
        expectedValue.append("LABEL main\n");
        expectedValue.append("PUSH 6\n");
        expectedValue.append("PUSH 3\n");
        expectedValue.append(tempCallID + " = CALL foobar\n");
        expectedValue.append("RET 0");
 
        EXPECT_EQ(10, tac.codeLines.size());
        EXPECT_EQ(expectedValue, tac.toString());
        
        }
        
          TEST(Function, FunctionRecursionTAC) {
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
        
        auto tempCondTriple = tac.codeLines.begin() + 2;
        auto tempCondID = (*tempCondTriple)->getValue();
        
        auto falseLabelTripel = tac.codeLines.begin() + 5;
        auto falseLabel = (*falseLabelTripel)->getValue();
        
        auto argTempTriple = tac.codeLines.begin() + 6;
        auto argTemp = (*argTempTriple)->getValue();
        
        auto tempCallTriple = tac.codeLines.begin() + 8;
        auto tempCallID = (*tempCallTriple)->getValue();
        
         auto tempCallMainTriple = tac.codeLines.begin() + 11;
        auto tempCallMainID = (*tempCallMainTriple)->getValue();
            
        std::string expectedValue = "LABEL foobar\n";
        expectedValue.append("arg10:1:0 = POP\n");
        expectedValue.append(tempCondID + " = arg10:1:0 == 0\n");
        expectedValue.append("JUMPFALSE " + tempCondID + " " + falseLabel + "\n");
        expectedValue.append("RET \n");
        expectedValue.append("LABEL " + falseLabel + "\n");
        expectedValue.append(argTemp + " = arg10:1:0 - 1\n");
        expectedValue.append("PUSH " + argTemp + "\n");
        expectedValue.append(tempCallID + " = CALL foobar\n");
        expectedValue.append("LABEL main\n");
        expectedValue.append("PUSH 5\n");
        expectedValue.append(tempCallMainID + " = CALL foobar\n");
        expectedValue.append("RET 0");
 
        EXPECT_EQ(13, tac.codeLines.size());
        EXPECT_EQ(expectedValue, tac.toString());
  
        }
          
    }
}
 
namespace mcc {
    namespace cfg {
         TEST(Function, StaticCallGraph) {
            auto tree = parser::parse(
                    R"(
          int foobar(int arg1, int arg2) {
            foobar(2,3);
            return arg1 + arg2;
          }
            
            int main() {
                foobar(3, 6);
                return 0;
            }
        )");

        Tac tac = Tac(tree);
        Scg scg = Scg(tac);
        std::cout << scg.toDot();
    }
         
}
}
