#include "mcc/cfg/cfg.h"

#include <cassert>
#include <iterator>
#include <memory>
#include <ostream>
#include <typeinfo>
#include <vector>

#include "mcc/cfg/set_helper.h"
#include "mcc/tac/operator.h"

namespace mcc {
namespace cfg {

Cfg::Cfg(mcc::tac::Tac tac) : basicBlockIndex(tac.getBasicBlockIndex()) {
  for (auto const var : *tac.getVariableStore().get()) {
    variableSet.insert(var);
  }

  for (auto const block : *basicBlockIndex.get()) {
    auto descriptor = boost::add_vertex(block, graph);

    assert((descriptor == block->getBlockId()) &&
           "Descriptor does not match blockId");

    mcc::tac::Variable::set_t set(variableSet);

    for (auto &var : block->getDefVar()) {
      set.erase(var);
    }

    notKilled.insert(std::make_pair(block->getBlockId(), set));
    liveIn.insert(
        std::make_pair(block->getBlockId(), mcc::tac::Variable::set_t()));

    liveOut.insert(
        std::make_pair(block->getBlockId(), mcc::tac::Variable::set_t()));
  }

  unsigned prevBlockId = 0;
  bool prevMatched = false;

  for (auto line : tac.codeLines) {
    bool matched = false;

    auto op = line->getOperator();

    if (op.getName() == mcc::tac::OperatorName::JUMP) {
      if (typeid(*line->getArg1().get()) == typeid(mcc::tac::Label)) {
        auto label = std::static_pointer_cast<mcc::tac::Label>(line->getArg1());

        boost::add_edge(line->getBasicBlockId(), label->getBasicBlockId(),
                        graph);

        matched = true;
      } else {
        assert(false && "Unknown jump destination");
      }
    }

    if (op.getName() == mcc::tac::OperatorName::JUMPFALSE) {
      if (typeid(*line->getArg2().get()) == typeid(mcc::tac::Label)) {
        auto label = std::static_pointer_cast<mcc::tac::Label>(line->getArg2());

        boost::add_edge(line->getBasicBlockId(), line->getBasicBlockId() + 1,
                        graph);

        boost::add_edge(line->getBasicBlockId(), label->getBasicBlockId(),
                        graph);
        matched = true;
      } else {
        assert(false && "Unknown jump destination");
      }
    }

    if (prevBlockId < line->getBasicBlockId()) {
      if (!prevMatched) {
        boost::add_edge(prevBlockId, line->getBasicBlockId(), graph);
      }

      prevBlockId = line->getBasicBlockId();
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

VertexVertexMap &Cfg::getDomTree() {
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

const Vertex &Cfg::getIdom(const Vertex &vertex) {
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

std::set<Vertex> Cfg::getDomSet(const Vertex &vertex) {
  std::set<Vertex> domSet;
  auto dSet = getDomSet(getVertexDescriptor(vertex));

  return convertSet(dSet);
}

const VertexDescriptor Cfg::getVertexDescriptor(const Vertex &vertex) const {
  return vertex->getBlockId();
}

const Vertex &Cfg::getVertex(const VertexDescriptor vertex) const {
  return basicBlockIndex->at(vertex);
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

std::set<VertexDescriptor> Cfg::getSuccessor(const VertexDescriptor vertex) {
  auto outEdges = boost::out_edges(vertex, graph);

  std::set<VertexDescriptor> sSet;

  boost::graph_traits<Graph>::out_edge_iterator e, e_end;
  for (boost::tie(e, e_end) = outEdges; e != e_end; ++e) {
    sSet.insert(boost::target(*e, graph));
  }

  return sSet;
}

std::set<Vertex> Cfg::getSuccessor(const Vertex &vertex) {
  auto sSet = getSuccessor(getVertexDescriptor(vertex));

  return convertSet(sSet);
}

std::set<VertexDescriptor> Cfg::getPredecessor(const VertexDescriptor vertex) {
  auto inEdges = boost::in_edges(vertex, graph);

  std::set<VertexDescriptor> pSet;

  boost::graph_traits<Graph>::in_edge_iterator e, e_end;
  for (boost::tie(e, e_end) = inEdges; e != e_end; ++e) {
    pSet.insert(boost::source(*e, graph));
  }

  return pSet;
}

std::set<Vertex> Cfg::getPredecessor(const Vertex &vertex) {
  auto sSet = getPredecessor(getVertexDescriptor(vertex));

  return convertSet(sSet);
}

unsigned Cfg::variableSetSize() const { return variableSet.size(); }

mcc::tac::Variable::set_t Cfg::getNotKilled(
    const VertexDescriptor vertex) const {
  auto varSet(variableSet);

  return varSet;
}

bool Cfg::updateLiveIn(VertexDescriptor v) {
  auto const &ueVar = this->getVertex(v)->getUeVar();
  auto const &notKilled = this->notKilled.at(v);

  auto const &liveOutSet = this->liveOut.at(v);

  auto temp1 = set_intersect(liveOutSet, notKilled);
  auto temp2 = set_union(ueVar, temp1);

  auto changed = (this->liveIn.at(v) != temp2);
  this->liveIn.at(v).swap(temp2);
  return changed;
}

bool Cfg::updateLiveOut(VertexDescriptor v) {
  mcc::tac::Variable::set_t tmp;

  for (auto s : this->getSuccessor(v)) {
    tmp = set_union(tmp, this->liveIn.at(s));
  }

  auto changed = (this->liveOut.at(v) != tmp);
  liveOut.at(v).swap(tmp);
  return (changed);
}

void Cfg::computeLiveInOut() {
  bool changed;

  do {
    changed = false;
    for (auto const b : *basicBlockIndex.get()) {
      this->updateLiveIn(b->getBlockId());
    }

    for (auto const b : *basicBlockIndex.get()) {
      if (this->updateLiveOut(b->getBlockId())) {
        changed = true;
      }
    }

  } while (changed);
}

void Cfg::computeWorkList() {
  std::set<VertexDescriptor> worklist;

  for (auto const b : *basicBlockIndex.get()) {
    worklist.insert(b->getBlockId());
  }

  while (worklist.size() > 0) {
    // let b in W
    // W = W \ {b}
    auto b = *worklist.begin();
    worklist.erase(worklist.begin());

    // recompute LIVEOUT(b)
    this->updateLiveOut(b);

    // t = LIVEIN(b)
    // recompute LIVEIN(b)
    auto changed = this->updateLiveIn(b);

    // if LIVEIN(b) != t then
    if (changed) {
      // W = W union pred(b)
      worklist = set_union(worklist, this->getPredecessor(b));
    }
  }
}

void Cfg::computeLive() {
  this->computeWorkList();

  for (auto const b : *basicBlockIndex.get()) {
    auto bLive = this->computeLive(b, b->getBlockMembers().front());
    this->live.insert(std::make_pair(this->getVertexDescriptor(b), bLive));
  }
}

mcc::tac::Variable::set_t Cfg::computeLive(
    mcc::tac::BasicBlock::ptr_t const bb,
    mcc::tac::Triple::ptr_t const triple) {
  assert((triple->getBasicBlockId() == bb->getBlockId()) &&
         "triple not in block");

  auto bLive = mcc::tac::Variable::set_t(
      this->getLiveOut(this->getVertexDescriptor(bb)));

  auto members = bb->getBlockMembers();
  for (auto it = members.rbegin(); it != members.rend(); ++it) {
    auto line = *it;

    auto se = SubExpression(line);

    for (auto const var : se.getVariables()) {
      bLive.insert(var);
    }

    if (line == triple) break;
  }

  return bLive;
}

mcc::tac::Variable::set_t Cfg::getLiveIn(VertexDescriptor v) {
  return this->liveIn.at(v);
}

mcc::tac::Variable::set_t Cfg::getLiveOut(VertexDescriptor v) {
  return this->liveOut.at(v);
}

mcc::tac::Variable::set_t Cfg::getLive(VertexDescriptor v) {
  return this->live.at(v);
}
}
}
