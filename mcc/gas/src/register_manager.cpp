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
    RegisterManager::vertex_range_map_type vertexMap;

    for (auto it = range.second.begin(); it < range.second.end(); ++it) {
      auto loc = *it;

      if (loc->containsTargetVar()) {
        auto defVar = loc->getTargetVariable();
        auto vt = boost::add_vertex(defVar, interference);
        auto pair = std::make_pair(it, vt);

        auto result = vertexMap.find(defVar);

        if (result != vertexMap.end()) {
          result->second.push_back(pair);
        } else {
          std::vector<RegisterManager::iter_descr_pair_type> vec;
          vec.push_back(pair);
          vertexMap.insert(std::make_pair(defVar, vec));
        }

        for (auto var : cfg.liveSetAt(loc, false)) {
          // targetVar interferes with var
          boost::add_edge(vt, vertexMap.at(var).back().second, interference);
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

RegisterManager::VertexDescriptor RegisterManager::lookupVertexDescr(
    mcc::tac::Label::ptr_t functionLabel, Vertex vertex,
    mcc::tac::Tac::code_lines_iter it) const {
  auto vertexDescrMap = this->functionDescriptorMap.at(functionLabel);
  auto vec = vertexDescrMap.at(vertex);

  for (auto vIt = vec.begin(); vIt < vec.end(); ++vIt) {
    if ((*vIt).first == it) return (*vIt).second;
  }

  assert(false && "Vertex descriptor not found");
  return 0;
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
