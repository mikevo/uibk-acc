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

            boost::add_edge(line.get()->basicBlockId, label.get()->basicBlockId,
                graph);
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

            boost::add_edge(line.get()->basicBlockId, label.get()->basicBlockId,
                graph);
          } else {
            assert(false && "Unknown jump destination");
          }
        }
      }
    }

    std::string Cfg::toDot() const {
      std::ostringstream out;
      boost::write_graphviz(out, graph);
      return out.str();
    }

    void Cfg::storeDot(std::string fileName) const {
      std::ofstream outf(fileName);

      outf << toDot();
    }

  }
}

