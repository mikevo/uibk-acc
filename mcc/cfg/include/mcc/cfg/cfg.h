#ifndef MCC_CFG_H
#define MCC_CFG_H

#include <string>

#include "mcc/tac/tac.h"

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/graphviz.hpp"

namespace mcc {
  namespace cfg {

    class Cfg {
      public:
        Cfg();
        Cfg(mcc::tac::Tac tac);
        std::string toDot() const;
        void storeDot(std::string fileName) const;

      private:
        boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
            std::shared_ptr<mcc::tac::BasicBlock> > graph;

    };
  }
}

#endif /* CFG_H */

