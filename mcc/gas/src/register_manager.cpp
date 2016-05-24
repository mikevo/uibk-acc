/*
 * register_manager.cpp
 *
 *  Created on: May 23, 2016
 */

#include "mcc/gas/register_manager.h"

#include "mcc/cfg/cfg.h"

namespace mcc {
namespace gas {

RegisterManager::RegisterManager(mcc::tac::Tac &tac) : tac(tac) {
  auto cfg = mcc::cfg::Cfg(tac);

  cfg.computeWorkList();

  for (auto range : tac.getFunctionRangeMap()) {
    Graph interference;
    std::map<Vertex, VertexDescriptor, mcc::tac::Variable::less> vertexMap;

    for (auto loc : range.second) {
      if (loc->containsTargetVar()) {
        auto defVar = loc->getTargetVariable();

        auto result = vertexMap.find(defVar);

        RegisterManager::VertexDescriptor vt;

        if (result != vertexMap.end()) {
          vt = result->second;
        } else {
          vt = boost::add_vertex(defVar, interference);
        }
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

RegisterManager::function_graph_map_type
RegisterManager::getFunctionGraphMap() {
  return this->functionGraphMap;
}

std::string RegisterManager::toDot(std::string fucntionName) const {
  return this->toDot(this->tac.lookupFunction(fucntionName));
}

std::string RegisterManager::toDot(mcc::tac::Label::ptr_t functionLabel) const {
  std::ostringstream out;
  auto graph = this->functionGraphMap.at(functionLabel);
  boost::write_graphviz(out, graph);
  return out.str();
}

void RegisterManager::storeDot(std::string fileName,
                               std::string fucntionName) const {
  return this->storeDot(fileName, this->tac.lookupFunction(fucntionName));
}

void RegisterManager::storeDot(std::string fileName,
                               mcc::tac::Label::ptr_t functionLabel) const {
  std::ofstream outf(fileName);
  outf << this->toDot(functionLabel);
  outf.close();
}
}
}
