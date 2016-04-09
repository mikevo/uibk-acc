#include "mcc/cfg/cfg.h"

#include <cassert>
#include <memory>
#include <ostream>
#include <typeinfo>

#include "mcc/tac/operator.h"

namespace mcc {
  namespace cfg {
    Cfg::Cfg() {
    }

    Cfg::Cfg(mcc::tac::Tac tac) {
      tac.createBasicBlockIndex();

      auto index = tac.getBasicBlockIndex();

      for (auto block : index) {
        boost::add_vertex(block, graph);
      }

      for (auto line : tac.codeLines) {
        auto op = line.get()->op;

        if (op.getName() == mcc::tac::OperatorName::JUMP) {
          if (typeid(*line.get()->arg1.get()) == typeid(mcc::tac::Label)) {
            auto label = std::static_pointer_cast<mcc::tac::Label>(
                line.get()->arg1);

            boost::add_edge(line.get()->basicBlockId,
                label.get()->basicBlockId + 1, graph);
          } else {
            assert(false && "Unknown jump destination");
          }
        }

        if (op.getName() == mcc::tac::OperatorName::JUMPFALSE) {
          if (typeid(*line.get()->arg2.get()) == typeid(mcc::tac::Label)) {
            auto label = std::static_pointer_cast<mcc::tac::Label>(
                line.get()->arg2);

            boost::add_edge(line.get()->basicBlockId,
                line.get()->basicBlockId + 1, graph);

            boost::add_edge(line.get()->basicBlockId,
                label.get()->basicBlockId + 1, graph);
          } else {
            assert(false && "Unknown jump destination");
          }
        }
      }

      // TODO: find a better solution
      std::ofstream outf("/tmp/min.gv");
      boost::write_graphviz(outf, graph);
    }

  }
}

