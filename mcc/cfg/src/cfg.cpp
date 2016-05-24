#include "mcc/cfg/cfg.h"

#include <cassert>
#include <iterator>
#include <memory>
#include <ostream>
#include <typeinfo>
#include <vector>

#include "mcc/cfg/set_helper.h"
#include "mcc/tac/helper/ast_converters.h"
#include "mcc/tac/operator.h"

typedef std::map<mcc::tac::Variable::ptr_t, mcc::tac::Tac::code_lines_iter,
                 mcc::tac::Variable::less> var_iter_map;

namespace mcc {
namespace cfg {

Cfg::Cfg(mcc::tac::Tac &tac)
    : basicBlockIndex(tac.getBasicBlockIndex()),
      variableStore(tac.getVariableStore()),
      tac(tac) {
  for (auto const var : *tac.getVariableStore().get()) {
    variableSet.insert(var);
  }

  for (auto const block : *basicBlockIndex.get()) {
    auto descriptor = boost::add_vertex(block, graph);

    assert((descriptor == block->getBlockId()) &&
           "Descriptor does not match blockId");

    mcc::tac::Variable::set_t set(variableSet);

    for (auto const var : block->getDefVar()) {
      set.erase(var);
    }

    notKilled.insert(std::make_pair(block->getBlockId(), set));
    liveIn.insert(
        std::make_pair(block->getBlockId(), mcc::tac::Variable::set_t()));

    liveOut.insert(
        std::make_pair(block->getBlockId(), mcc::tac::Variable::set_t()));
  }

  unsigned prevBlockId = 0;
  std::set<unsigned> returnBbIdSet;
  bool alreadyHandled = false;

  for (auto line : tac.codeLines) {
    // if entered a new block
    if (prevBlockId != line->getBasicBlockId()) {
      // if candidate edge not already handled
      if (!alreadyHandled) {
        bool isFunctionEntryLabel = false;
        if (mcc::tac::helper::isType<mcc::tac::Label>(line)) {
          auto label = std::static_pointer_cast<mcc::tac::Label>(line);

          isFunctionEntryLabel = label->isFunctionEntry();
        }

        // do not connect subsequent BBs of different functions in their
        // definitions
        if (!isFunctionEntryLabel) {
          boost::add_edge(prevBlockId, line->getBasicBlockId(), graph);
        }
      } else {
        // add edges from function returns to Basic Block after function call
        for (auto const returnBbId : returnBbIdSet) {
          if (returnBbId != line->getBasicBlockId()) {
            boost::add_edge(returnBbId, line->getBasicBlockId(), graph);
          }
        }
      }
    }

    // reset variables belonging to previous iteration
    returnBbIdSet.clear();
    alreadyHandled = false;

    auto op = line->getOperator();

    if (op.getName() == mcc::tac::OperatorName::JUMP) {
      if (mcc::tac::helper::isType<mcc::tac::Label>(line->getArg1())) {
        auto label = std::static_pointer_cast<mcc::tac::Label>(line->getArg1());

        boost::add_edge(line->getBasicBlockId(), label->getBasicBlockId(),
                        graph);

        alreadyHandled = true;
      } else {
        assert(false && "Unknown jump or call destination");
      }
    }

    if (op.getName() == mcc::tac::OperatorName::JUMPFALSE) {
      if (mcc::tac::helper::isType<mcc::tac::Label>(line->getArg2())) {
        auto label = std::static_pointer_cast<mcc::tac::Label>(line->getArg2());

        boost::add_edge(line->getBasicBlockId(), line->getBasicBlockId() + 1,
                        graph);

        boost::add_edge(line->getBasicBlockId(), label->getBasicBlockId(),
                        graph);
        alreadyHandled = true;
      } else {
        assert(false && "Unknown jump destination");
      }
    }

    // do not link BB with return in it with the subsequent BB
    if (op.getName() == mcc::tac::OperatorName::RET) {
      alreadyHandled = true;
    }

    prevBlockId = line->getBasicBlockId();
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

Cfg::VertexVertexMap &Cfg::getDomTree() {
  if (!dominatorTree.empty()) {
    return dominatorTree;
  }

  calculateDOM();

  return dominatorTree;
}

const Cfg::VertexDescriptor Cfg::getIdom(const VertexDescriptor vertex) {
  if (dominatorTree.empty()) {
    calculateDOM();
  }

  return dominatorTree[vertex];
}

const Cfg::Vertex &Cfg::getIdom(const Vertex &vertex) {
  auto idom = getIdom(getVertexDescriptor(vertex));
  return getVertex(idom);
}

std::set<Cfg::VertexDescriptor> Cfg::getDomSet(VertexDescriptor vertex) {
  std::set<VertexDescriptor> domSet;

  domSet.insert(vertex);

  VertexDescriptor idom = getIdom(vertex);
  while (idom > getIdom(idom)) {
    domSet.insert(idom);
    idom = getIdom(idom);
  }

  domSet.insert(idom);

  return domSet;
}

std::set<Cfg::Vertex> Cfg::getDomSet(const Vertex &vertex) {
  std::set<Vertex> domSet;
  auto dSet = getDomSet(getVertexDescriptor(vertex));

  return convertSet(dSet);
}

const Cfg::VertexDescriptor Cfg::getVertexDescriptor(
    const Vertex &vertex) const {
  return vertex->getBlockId();
}

const Cfg::Vertex &Cfg::getVertex(const VertexDescriptor vertex) const {
  return basicBlockIndex->at(vertex);
}

std::set<Cfg::Vertex> Cfg::convertSet(std::set<VertexDescriptor> inSet) const {
  std::set<Vertex> outSet;

  for (auto e : inSet) {
    outSet.insert(getVertex(e));
  }

  return outSet;
}

std::set<Cfg::VertexDescriptor> Cfg::convertSet(std::set<Vertex> inSet) const {
  std::set<VertexDescriptor> outSet;

  for (auto e : inSet) {
    outSet.insert(getVertexDescriptor(e));
  }

  return outSet;
}

std::set<Cfg::VertexDescriptor> Cfg::getSuccessor(
    const VertexDescriptor vertex) {
  auto outEdges = boost::out_edges(vertex, graph);

  std::set<VertexDescriptor> sSet;

  boost::graph_traits<Graph>::out_edge_iterator e, e_end;
  for (boost::tie(e, e_end) = outEdges; e != e_end; ++e) {
    sSet.insert(boost::target(*e, graph));
  }

  return sSet;
}

std::set<Cfg::Vertex> Cfg::getSuccessor(const Vertex &vertex) {
  auto sSet = getSuccessor(getVertexDescriptor(vertex));

  return convertSet(sSet);
}

std::set<Cfg::VertexDescriptor> Cfg::getPredecessor(
    const VertexDescriptor vertex) {
  auto inEdges = boost::in_edges(vertex, graph);

  std::set<VertexDescriptor> pSet;

  boost::graph_traits<Graph>::in_edge_iterator e, e_end;
  for (boost::tie(e, e_end) = inEdges; e != e_end; ++e) {
    pSet.insert(boost::source(*e, graph));
  }

  return pSet;
}

std::set<Cfg::Vertex> Cfg::getPredecessor(const Vertex &vertex) {
  auto sSet = getPredecessor(getVertexDescriptor(vertex));

  return convertSet(sSet);
}

unsigned Cfg::variableSetSize() const { return variableSet.size(); }

mcc::tac::Variable::set_t Cfg::getNotKilled(
    const VertexDescriptor vertex) const {
  return this->notKilled.at(vertex);
}

bool Cfg::updateLiveIn(VertexDescriptor v) {
  auto const &ueVar = this->getVertex(v)->getUeVar();
  auto const &notKilled = this->getNotKilled(v);

  auto const &liveOutSet = this->getLiveOut(v);

  auto tmp = set_intersect(liveOutSet, notKilled);
  tmp = set_union(ueVar, tmp);

  auto changed = (this->getLiveIn(v) != tmp);
  this->liveIn.at(v).swap(tmp);
  return changed;
}

bool Cfg::updateLiveOut(VertexDescriptor v) {
  mcc::tac::Variable::set_t tmp;

  for (auto s : this->getSuccessor(v)) {
    tmp = set_union(tmp, this->getLiveIn(s));
  }

  auto changed = (this->getLiveOut(v) != tmp);
  this->liveOut.at(v).swap(tmp);
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

mcc::tac::Variable::set_t Cfg::liveSetAt(mcc::tac::Triple::ptr_t const triple,
                                         bool recomputeWorkList) {
  if (recomputeWorkList) this->computeWorkList();

  auto bb = basicBlockIndex->at(triple->getBasicBlockId());

  auto bLive = mcc::tac::Variable::set_t(
      this->getLiveOut(this->getVertexDescriptor(bb)));

  auto members = bb->getBlockMembers();
  for (auto it = members.rbegin(); it != members.rend(); ++it) {
    auto line = *it;

    auto se = SubExpression(line);

    for (auto const var : se.getVariables()) {
      bLive.insert(var);
    }

    if (line->containsTargetVar()) {
      bLive.erase(line->getTargetVariable());
    }

    if (line == triple) {
      return bLive;
    }
  }

  std::cout << bb->getBlockId() << " " << triple->toString() << std::endl;
  assert(false && "triple not found in block");
  return {};
}

mcc::tac::Variable::set_t Cfg::liveSetAfter(
    mcc::tac::Triple::ptr_t const triple, bool recomputeWorkList) {
  auto bb = basicBlockIndex->at(triple->getBasicBlockId());

  if (bb->getBlockMembers().back() == triple) {
    if (recomputeWorkList) this->computeWorkList();
    return getLiveOut(this->getVertexDescriptor(bb));
  } else {
    auto members = bb->getBlockMembers();

    // find line inside BB
    for (auto it = members.begin(); it != members.end(); ++it) {
      auto line = *it;

      if (line == triple) {
        ++it;
        line = *it;  // set line to following line
        // liveSetAfter result is the liveSet of the next line
        return this->liveSetAt(line, recomputeWorkList);
      }
    }
  }

  std::cout << bb->getBlockId() << " " << triple->toString() << std::endl;
  assert(false && "triple not found in block");
  return {};
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

Cfg::variable_live_range_map_type Cfg::getVariableLiveRangeMap() {
  var_iter_map startMap;
  var_iter_map endMap;

  for (auto var : *this->variableStore.get()) {
    auto start = std::make_pair(var, this->tac.codeLines.end());
    startMap.insert(start);

    auto end = std::make_pair(var, this->tac.codeLines.begin());
    endMap.insert(end);
  }

  this->computeWorkList();

  for (auto it = this->tac.codeLines.begin(); it < this->tac.codeLines.end();
       ++it) {
    for (auto var : this->liveSetAt(*it, false)) {
      if (startMap.at(var) > it) {
        startMap[var] = it;
      }

      if (endMap.at(var) < it + 1) {
        endMap[var] = it + 1;
      }
    }
  }

  Cfg::variable_live_range_map_type map;

  for (auto var : *this->variableStore.get()) {
    if (startMap.at(var) != this->tac.codeLines.end()) {
      auto range = boost::make_iterator_range(startMap.at(var), endMap.at(var));
      auto pair = std::make_pair(var, range);

      map.insert(pair);
    }
  }

  return map;
}
}
}
