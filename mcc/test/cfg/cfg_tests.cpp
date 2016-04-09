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
      auto graph = new Cfg(tac);

      // TODO: find a better solution
      std::ofstream outf("/tmp/min.tac");
      tac.createBasicBlockIndex();

      for (auto block : tac.getBasicBlockIndex()) {
        outf << block.get()->toString();
      }
    }
  }
}

