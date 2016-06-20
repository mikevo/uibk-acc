#ifndef MCC_CFG_H
#define MCC_CFG_H

#include <memory>
#include <string>
#include <vector>

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/dominator_tree.hpp"
#include "boost/graph/graphviz.hpp"

#include "mcc/tac/basic_block.h"
#include "mcc/tac/tac.h"

namespace mcc {
namespace cfg {

class Cfg {
 public:
  typedef mcc::tac::BasicBlock::ptr_t Vertex;
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                                Vertex> Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
  typedef std::map<VertexDescriptor, VertexDescriptor> VertexVertexMap;

  typedef std::shared_ptr<Cfg> ptr_t;

  typedef std::map<mcc::tac::Variable::ptr_t, mcc::tac::Tac::code_lines_range,
                   mcc::tac::Variable::less> variable_live_range_map_type;
  typedef std::map<mcc::tac::Array::ptr_t, mcc::tac::Tac::code_lines_range,
                   mcc::tac::Array::less> array_live_range_map_type;

  Cfg(mcc::tac::Tac &tac);
  std::string toDot() const;
  void storeDot(std::string fileName) const;

  void calculateDOM();
  VertexVertexMap &getDomTree();
  const VertexDescriptor getIdom(const VertexDescriptor vertex);
  const Vertex &getIdom(const Vertex &vertex);

  std::set<VertexDescriptor> getDomSet(const VertexDescriptor vertex);
  std::set<Vertex> getDomSet(const Vertex &vertex);

  std::set<VertexDescriptor> getSuccessor(const VertexDescriptor vertex);
  std::set<Vertex> getSuccessor(const Vertex &vertex);

  std::set<VertexDescriptor> getPredecessor(const VertexDescriptor vertex);
  std::set<Vertex> getPredecessor(const Vertex &vertex);

  unsigned variableSetSize() const;

  mcc::tac::Variable::set_t getNotKilled(const VertexDescriptor vertex) const;

  void computeLiveInOut();
  void computeWorkList();

  // live set including the given line (triple)
  mcc::tac::Variable::set_t liveSetAt(mcc::tac::Triple::ptr_t const triple,
                                      bool recomputeWorkList = true);
  // live set excluding the given line (triple)
  mcc::tac::Variable::set_t liveSetAfter(mcc::tac::Triple::ptr_t const triple,
                                         bool recomputeWorkList = true);

  mcc::tac::Variable::set_t getLiveIn(VertexDescriptor v);
  mcc::tac::Variable::set_t getLiveOut(VertexDescriptor v);
  mcc::tac::Variable::set_t getLive(VertexDescriptor v);

  variable_live_range_map_type getVariableLiveRangeMap();
  array_live_range_map_type getArrayLiveRangeMap();

 private:
  mcc::tac::Variable::set_t variableSet;
  const VertexDescriptor getVertexDescriptor(const Vertex &vertex) const;
  const Vertex &getVertex(const VertexDescriptor vertex) const;
  std::set<Vertex> convertSet(std::set<VertexDescriptor> inSet) const;
  std::set<VertexDescriptor> convertSet(std::set<Vertex> inSet) const;

  bool updateLiveIn(VertexDescriptor v);
  bool updateLiveOut(VertexDescriptor v);

  const mcc::tac::bb_type basicBlockIndex;
  mcc::tac::VariableStore::ptr_t variableStore;
  //  std::vector<mcc::tac::Triple::ptr_t> & codeLines;
  mcc::tac::Tac &tac;
  Graph graph;
  VertexVertexMap dominatorTree;

  std::map<VertexDescriptor, mcc::tac::Variable::set_t> notKilled;
  std::map<VertexDescriptor, mcc::tac::Variable::set_t> liveIn;
  std::map<VertexDescriptor, mcc::tac::Variable::set_t> liveOut;
  std::map<VertexDescriptor, mcc::tac::Variable::set_t> live;
};
}
}

#endif /* CFG_H */
