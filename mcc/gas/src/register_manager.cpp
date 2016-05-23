/*
 * register_manager.cpp
 *
 *  Created on: May 23, 2016
 */

#include "mcc/gas/register_manager.h"

#include "mcc/cfg/cfg.h"

RegisterManager::RegisterManager(mcc::tac::Tac &tac) {
  auto cfg = mcc::cfg::Cfg(tac);

  cfg.computeWorkList();

  for (auto range : tac.getFunctionRangeMap()) {
    Graph interference;
    std::map<Vertex, VertexDescriptor, mcc::tac::Variable::less> vertexMap;

    for (auto loc : range.second) {
      if (loc->containsTargetVar()) {
        auto defVar = loc->getTargetVariable();
        auto vt = boost::add_vertex(defVar, interference);
        vertexMap.insert(std::make_pair(defVar, vt));

        for (auto var : cfg.liveSetAt(loc, false)) {
          // targetVar interferes with var
          boost::add_edge(vt, vertexMap.at(var), interference);
        }
      }
    }

    this->functionGraphMap.insert(std::make_pair(range.first, interference));
  }
}
