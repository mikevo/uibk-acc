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

RegisterManager::RegisterManager(mcc::tac::Tac &tac, Gas *gas)
    : tac(tac), gas(gas) {
  this->numOfRegForColoring = 2;

  this->numColorsMap = std::make_shared<num_colors_map_type>();
  this->functionGraphColorsMap =
      std::make_shared<function_graph_color_map_type>();

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

  this->graphColoring(this->numOfRegForColoring);
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

unsigned RegisterManager::getNumOfRegForColoring() const {
  return this->numOfRegForColoring;
}

RegisterManager::VertexDescriptor RegisterManager::lookupVertexDescr(
    mcc::tac::Label::ptr_t functionLabel, Vertex vertex,
    mcc::tac::Tac::code_lines_iter it) const {
  auto vertexDescrMap = this->functionDescriptorMap.at(functionLabel);
  auto vec = vertexDescrMap.at(vertex);

  for (auto vIt = vec.rbegin(); vIt < vec.rend(); ++vIt) {
    if ((*(vIt.base() - 1)).first <= it) return (*vIt).second;
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

void RegisterManager::graphColoring(unsigned n) {
  // clear maps
  this->numColorsMap->clear();
  this->functionGraphColorsMap->clear();

  for (auto function : this->functionGraphMap) {
    this->graphColoring(function.first, n);
  }
}

unsigned RegisterManager::graphColoring(std::string functionName, unsigned n) {
  return this->graphColoring(this->tac.lookupFunction(functionName), n);
}

unsigned RegisterManager::graphColoring(mcc::tac::Label::ptr_t functionLabel,
                                        unsigned n) {
  auto graph = this->functionGraphMap.at(functionLabel);

  // compute coloring
  auto colors = std::make_shared<RegisterManager::graph_color_map_type>();
  unsigned numColors = graphColoring(graph, colors, n);

  // store colors and numColors in maps
  (*numColorsMap)[functionLabel] = numColors;
  (*functionGraphColorsMap)[functionLabel] = colors;

  return numColors;
}

unsigned RegisterManager::graphColoring(
    Graph graph, std::shared_ptr<graph_color_map_type> colors, unsigned n) {
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
    auto curCol = boostCol[i];
    (*colors)[i] = curCol > n ? n : curCol;
  }

  return numColors;
}

Operand::ptr_t RegisterManager::getRegisterForVariable(
    mcc::tac::Label::ptr_t functionLabel, Vertex vertex,
    mcc::tac::Tac::code_lines_iter it) {
  auto color = this->getColor(functionLabel, vertex, it);

  if (this->isColor(color)) {
    return this->getRegister(color);
  } else {
    return this->getSpilledVariable(vertex);
  }
}

void RegisterManager::storeRegisterInVariable(
    mcc::tac::Label::ptr_t functionLabel, Vertex vertex,
    mcc::tac::Tac::code_lines_iter it) {
  auto color = this->getColor(functionLabel, vertex, it);

  if (!this->isColor(color)) {
    this->storeSpilledVariable(vertex);
  }
}

bool RegisterManager::isColor(unsigned color) {
  if (color < this->numOfRegForColoring)
    return true;
  else
    return false;
}

unsigned RegisterManager::getColor(mcc::tac::Label::ptr_t functionLabel,
                                   Vertex vertex,
                                   mcc::tac::Tac::code_lines_iter it) {
  auto vd = this->lookupVertexDescr(functionLabel, vertex, it);
  auto colorMap = this->getFunctionGraphColorsMap()->at(functionLabel);

  return colorMap->at(vd);
}

RegisterManager::VertexDescriptor RegisterManager::getVertexDescriptor(
    mcc::tac::Label::ptr_t functionLabel, Vertex vertex,
    mcc::tac::Tac::code_lines_iter it) {
  return this->lookupVertexDescr(functionLabel, vertex, it);
}

Operand::ptr_t RegisterManager::getRegister(unsigned color) {
  switch (color) {
    case 0:
      return std::make_shared<Operand>(Register::ECX);
    case 1:
      return std::make_shared<Operand>(Register::EDX);
    default:
      assert(false && "to view registeres defined for reg alloc");
      return nullptr;
  }
}

Operand::ptr_t RegisterManager::getSpilledVariable(Vertex vertex) {
  Register regName;
  // toggle register to ensure that two possibly spilled operands are in
  // different regs
  if (this->spillReg)
    regName = Register::EAX;
  else
    regName = Register::EBX;

  auto reg = std::make_shared<Operand>(regName);
  this->spilledVarMap[vertex] = regName;
  this->spillReg = !this->spillReg;

  gas->loadSpilledVariable(vertex, reg);
  return reg;
}

void RegisterManager::storeSpilledVariable(Vertex vertex) {
  Register regName = this->spilledVarMap[vertex];
  auto reg = std::make_shared<Operand>(regName);

  gas->storeSpilledVariable(vertex, reg);
}
}
}
