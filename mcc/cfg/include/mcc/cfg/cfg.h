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
        typedef std::shared_ptr<SubExpression> SubExpressionPtr;
        typedef std::set<SubExpressionPtr> SubExpressionSet;

        Cfg(mcc::tac::Tac tac);
        std::string toDot() const;
        void storeDot(std::string fileName) const;

        void calculateDOM();
        VertexVertexMap& getDomTree();
        const VertexDescriptor getIdom(const VertexDescriptor vertex);
        const Vertex& getIdom(const Vertex& vertex);

        std::set<VertexDescriptor> getDomSet(const VertexDescriptor vertex);
        std::set<Vertex> getDomSet(const Vertex& vertex);

        std::set<VertexDescriptor> getSuccessor(const VertexDescriptor vertex);
        std::set<Vertex> getSuccessor(const Vertex& vertex);

        std::set<VertexDescriptor> getPredecessor(
            const VertexDescriptor vertex);
        std::set<Vertex> getPredecessor(const Vertex& vertex);

        unsigned variableSetSize() const;

        std::set<mcc::tac::VarTableValue> getNotKilled(
            const VertexDescriptor vertex) const;

        void computeLive(void);
        void computeWorkList();
        void computeAvailableExpressions();

        std::set<mcc::tac::VarTableValue> getLiveIn(VertexDescriptor v);
        std::set<mcc::tac::VarTableValue> getLiveOut(VertexDescriptor v);

        SubExpressionSet getNotKilledExpr(VertexDescriptor v);
        SubExpressionSet getAvail(VertexDescriptor v);

        std::set<mcc::tac::VarTableValue> variableSet;
      private:
        const VertexDescriptor getVertexDescriptor(const Vertex& vertex) const;
        const Vertex& getVertex(const VertexDescriptor vertex) const;
        std::set<Vertex> convertSet(std::set<VertexDescriptor> inSet) const;
        std::set<VertexDescriptor> convertSet(std::set<Vertex> inSet) const;

        bool updateLiveIn(VertexDescriptor v);
        bool updateLiveOut(VertexDescriptor v);

        const mcc::tac::bb_type basicBlockIndex;
        Graph graph;
        VertexVertexMap dominatorTree;

        std::map<VertexDescriptor, std::set<mcc::tac::VarTableValue>> notKilled;
        std::map<VertexDescriptor, std::set<mcc::tac::VarTableValue>> liveIn;
        std::map<VertexDescriptor, std::set<mcc::tac::VarTableValue>> liveOut;

        SubExpressionSet allSubExpressions;
        std::map<VertexDescriptor, SubExpressionSet> notKilledExpr;
        std::map<VertexDescriptor, SubExpressionSet> avail;
    };
  }
}

#endif /* CFG_H */

