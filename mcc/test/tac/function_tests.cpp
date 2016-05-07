#include <gtest/gtest.h>

#include "mcc/tac/tac.h"

#include <memory>
#include <string>

#include "ast.h"
#include "mcc/tac/type.h"
#include "parser.h"

namespace mcc {
    namespace tac {

        TEST(Function, DeclarationTAC) {
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
}
}
