#include "mcc/cfg/cfg.h"

#include <cassert>
#include <memory>
#include <ostream>
#include <typeinfo>
#include <vector>

#include "mcc/tac/operator.h"

namespace mcc {
  namespace cfg {
    Cfg::Cfg(mcc::tac::Tac tac) :
        basicBlockIndex(tac.getBasicBlockIndex()) {

      for (auto const& e : tac.getVarTable()) {
        for (auto const& var : e.second) {
          variableSet.insert(var);
        }
      }

      for (auto const& block : basicBlockIndex) {
        boost::add_vertex(block, graph);
      }

      unsigned prevBlockId = 0;
      bool prevMatched = false;

      for (auto line : tac.codeLines) {
        bool matched = false;

        auto op = line.get()->op;

        if (op.getName() == mcc::tac::OperatorName::JUMP) {
          if (typeid(*line.get()->arg1.get()) == typeid(mcc::tac::Label)) {
            auto label = std::static_pointer_cast<mcc::tac::Label>(
                line.get()->arg1);

            boost::add_edge(line.get()->basicBlockId, label.get()->basicBlockId,
                graph);
            matched = true;
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
            matched = true;
          } else {
            assert(false && "Unknown jump destination");
          }
        }

        if (prevBlockId < line->basicBlockId) {
          if (!prevMatched) {
            boost::add_edge(prevBlockId, line.get()->basicBlockId, graph);
          }

          prevBlockId = line->basicBlockId;
        }

        prevMatched = matched;
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

    void Cfg::calculateDOM() {

      boost::associative_property_map<VertexVertexMap> domTreePredMap(
          dominatorTree);

      boost::lengauer_tarjan_dominator_tree(graph, boost::vertex(0, graph),
          domTreePredMap);
    }

    VertexVertexMap& Cfg::getDomTree() {
      if (!dominatorTree.empty()) {
        return dominatorTree;
      }

      calculateDOM();

      return dominatorTree;
    }

    const VertexDescriptor Cfg::getIdom(const VertexDescriptor vertex) {
      if (dominatorTree.empty()) {
        calculateDOM();
      }

      return dominatorTree[vertex];
    }

    const Vertex& Cfg::getIdom(const Vertex& vertex) {
      auto idom = getIdom(getVertexDescriptor(vertex));
      return getVertex(idom);
    }

    std::set<VertexDescriptor> Cfg::getDomSet(VertexDescriptor vertex) {
      std::set<VertexDescriptor> domSet;

      VertexDescriptor idom = getIdom(vertex);
      while (idom > getIdom(idom)) {
        domSet.insert(idom);
        idom = getIdom(idom);
      }

      domSet.insert(idom);

      return domSet;
    }

    std::set<Vertex> Cfg::getDomSet(const Vertex& vertex) {
      std::set<Vertex> domSet;
      auto dSet = getDomSet(getVertexDescriptor(vertex));

      return convertSet(dSet);
    }

    const VertexDescriptor Cfg::getVertexDescriptor(
        const Vertex& vertex) const {
      return vertex->getBlockId();
    }

    const Vertex& Cfg::getVertex(const VertexDescriptor vertex) const {
      return basicBlockIndex[vertex];
    }

    std::set<Vertex> Cfg::convertSet(std::set<VertexDescriptor> inSet) const {
      std::set<Vertex> outSet;

      for (auto e : inSet) {
        outSet.insert(getVertex(e));
      }

      return outSet;
    }

    std::set<VertexDescriptor> Cfg::convertSet(std::set<Vertex> inSet) const {
      std::set<VertexDescriptor> outSet;

      for (auto e : inSet) {
        outSet.insert(getVertexDescriptor(e));
      }

      return outSet;
    }

    std::set<VertexDescriptor> Cfg::getSuccessor(
        const VertexDescriptor vertex) {
      auto outEdges = boost::out_edges(vertex, graph);

      std::set<VertexDescriptor> sSet;

      boost::graph_traits<Graph>::out_edge_iterator e, e_end;
      for (boost::tie(e, e_end) = outEdges; e != e_end; ++e) {
        sSet.insert(boost::target(*e, graph));
      }

      return sSet;
    }

    std::set<Vertex> Cfg::getSuccessor(const Vertex& vertex) {
      auto sSet = getSuccessor(getVertexDescriptor(vertex));

      return convertSet(sSet);
    }

    std::set<VertexDescriptor> Cfg::getPredecessor(
        const VertexDescriptor vertex) {
      auto inEdges = boost::in_edges(vertex, graph);

      std::set<VertexDescriptor> pSet;

      boost::graph_traits<Graph>::in_edge_iterator e, e_end;
      for (boost::tie(e, e_end) = inEdges; e != e_end; ++e) {
        pSet.insert(boost::source(*e, graph));
      }

      return pSet;
    }

    std::set<Vertex> Cfg::getPredecessor(const Vertex& vertex) {
      auto sSet = getPredecessor(getVertexDescriptor(vertex));

      return convertSet(sSet);
    }

    unsigned Cfg::variableSetSize() const {
      return variableSet.size();
    }

    std::set<mcc::tac::VarTableValue> Cfg::getNotKilled(
        const VertexDescriptor vertex) const {
      auto varSet(variableSet);

      return varSet;
    }
  }
}

