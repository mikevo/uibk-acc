#ifndef MCC_CFG_H
#define MCC_CFG_H

#include <string>
#include <memory>
#include <vector>

#include "mcc/tac/tac.h"

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/dominator_tree.hpp"
#include "boost/graph/graphviz.hpp"

#include "mcc/tac/basic_block.h"

namespace mcc {
  namespace cfg {

    typedef std::shared_ptr<mcc::tac::BasicBlock> Vertex;
    typedef boost::adjacency_list<boost::vecS, boost::vecS,
        boost::bidirectionalS, Vertex> Graph;
    typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
    typedef std::map<VertexDescriptor, VertexDescriptor> VertexVertexMap;

    class Cfg {
      public:
        Cfg(mcc::tac::Tac tac);
        std::string toDot() const;
        void storeDot(std::string fileName) const;

        void calculateDOM();
        VertexVertexMap& getDomTree();
        VertexDescriptor getIdom(VertexDescriptor vertex);
        const Vertex& getIdom(Vertex& vertex);

        std::set<VertexDescriptor> getDomSet(VertexDescriptor vertex);
        std::set<Vertex> getDomSet(Vertex vertex);

      private:
        const std::vector<Vertex> basicBlockIndex;
        Graph graph;
        VertexVertexMap dominatorTree;
    };
  }
}

#endif /* CFG_H */

