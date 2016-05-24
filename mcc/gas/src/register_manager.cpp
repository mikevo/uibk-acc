/*
 * register_manager.cpp
 *
 *  Created on: May 23, 2016
 */

#include "mcc/gas/register_manager.h"

#include "mcc/cfg/cfg.h"
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/graph/smallest_last_ordering.hpp>

namespace mcc {
namespace gas {

RegisterManager::RegisterManager(mcc::tac::Tac &tac) : tac(tac) {
  this->numColorsMap = std::make_shared<num_colors_map_type>();
  this->functionGraphColorsMap =
      std::make_shared<function_graph_color_map_type>();

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

std::shared_ptr<RegisterManager::num_colors_map_type>
RegisterManager::getNumColorsMap() {
  return this->numColorsMap;
}

std::shared_ptr<RegisterManager::function_graph_color_map_type>
RegisterManager::getFunctionGraphColorsMap() {
  return this->functionGraphColorsMap;
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

void RegisterManager::graphColoring() {
  // clear maps
  this->numColorsMap->clear();
  this->functionGraphColorsMap->clear();

  for (auto function : this->functionGraphMap) {
    this->graphColoring(function.first);
  }
}

unsigned RegisterManager::graphColoring(std::string functionName) {
  return this->graphColoring(this->tac.lookupFunction(functionName));
}

unsigned RegisterManager::graphColoring(mcc::tac::Label::ptr_t functionLabel) {
  auto graph = this->functionGraphMap.at(functionLabel);

  // compute coloring
  auto colors = std::make_shared<RegisterManager::graph_color_map_type>();
  unsigned numColors = graphColoring(graph, colors);

  // store colors and numColors in maps
  (*numColorsMap)[functionLabel] = numColors;
  (*functionGraphColorsMap)[functionLabel] = colors;

  return numColors;
}

unsigned RegisterManager::graphColoring(
    Graph graph, std::shared_ptr<graph_color_map_type> colors) {
  // compute smallest last ordering
  boost::vector_property_map<RegisterManager::VertexDescriptor> order;
  smallest_last_vertex_ordering(graph, order);

  // compute coloring
  auto colorVec = std::vector<vertices_size_type>(boost::num_vertices(graph));
  auto boostCol =
      boost::iterator_property_map<vertices_size_type *, vertex_index_map>(
          &colorVec.front(), boost::get(boost::vertex_index, graph));

  unsigned numColors = sequential_vertex_coloring(graph, order, boostCol);

  colors->clear();
  for (unsigned i = 0; i < num_vertices(graph); ++i) {
    (*colors)[i] = boostCol[i];
  }

  return numColors;
}
}
}
