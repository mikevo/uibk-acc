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

RegisterManager::RegisterManager(Tac &tac) : tac(tac) {
  this->numOfRegForColoring = 5;

  this->functionArgSizeMap = std::make_shared<function_arg_size_type>();
  this->functionStackSpaceMap =
      std::make_shared<function_stack_space_map_type>();
  this->variableStackOffsetMap =
      std::make_shared<variable_stack_offset_map_type>();

  this->numColorsMap = std::make_shared<num_colors_map_type>();
  this->functionGraphColorsMap =
      std::make_shared<function_graph_color_map_type>();

  this->functionDescriptorMap = std::make_shared<function_descr_map_type>();

  this->generateInterferenceGraphs();
  this->graphColoring(this->numOfRegForColoring);
  this->analyzeStackUsages();
}

unsigned RegisterManager::getSize(Type t) {
  switch (t) {
    case Type::BOOL:
      return 0;  // Is not stored in assembly
      break;
    case Type::INT:
      return sizeof(int);
      break;
    case Type::FLOAT:
      return sizeof(float);
      break;
    default:
      return 0;
  }
}

unsigned RegisterManager::getSize(std::shared_ptr<mcc::tac::Operand> operand) {
  return getSize(operand->getType()) * operand->length();
}

unsigned RegisterManager::getSize(std::vector<Tac::type_size_type> argList) {
  unsigned size = 0;

  for (auto arg : argList) {
    size += (getSize(arg.first) * arg.second);
  }

  return size;
}

void RegisterManager::generateInterferenceGraphs() {
  auto cfg = mcc::cfg::Cfg(tac);
  cfg.computeWorkList();

  for (auto range : tac.getFunctionRangeMap()) {
    Graph interference;
    RegisterManager::vertex_range_map_type vertexMap;

    for (auto it = range.second.begin(); it < range.second.end(); ++it) {
      auto loc = *it;

      if (loc->containsTargetVar()) {
        auto defVar = loc->getTargetVariable();
        auto result = vertexMap.find(defVar);

        if (result == vertexMap.end()) {
          auto vt = boost::add_vertex(defVar, interference);
          auto pair = std::make_pair(defVar, vt);

          vertexMap.insert(pair);
        }

        for (auto var : cfg.liveSetAt(loc, false)) {
          // targetVar interferes with var
          boost::add_edge(vertexMap.at(defVar), vertexMap.at(var),
                          interference);
        }
      }
    }

    this->functionGraphMap.insert(std::make_pair(range.first, interference));
    this->functionDescriptorMap->insert(std::make_pair(range.first, vertexMap));
  }
}

void RegisterManager::analyzeStackUsages() {
  // TODO move functionVariableMap to helper
  // get argSize of all declared functions
  for (auto e : *tac.getFunctionPrototypeMap().get()) {
    (*this->functionArgSizeMap)[e.first] = getSize(e.second);

    std::vector<Variable::ptr_t> vec;
    this->functionVariableMap[tac.lookupFunction(e.first)] = vec;
  }

  Label::ptr_t currentFunctionLabel = nullptr;

  // Begin offset space for ebp and return address
  unsigned initStackSpace = 0;
  signed initLocalOffset = -4;
  unsigned initParamOffset = 8;

  unsigned stackSpace = initStackSpace;
  signed curLocalOffset = initLocalOffset;
  unsigned curParamOffset = initParamOffset;

  for (auto codeLine : tac.codeLines) {
    auto opName = codeLine->getOperator().getName();
    if (opName == OperatorName::LABEL) {
      auto label = std::static_pointer_cast<Label>(codeLine);
      if (label->isFunctionEntry()) {
        // if new function is entered
        if (currentFunctionLabel != nullptr) {
          this->setFunctionStackSpace(currentFunctionLabel, stackSpace);

          // reset variables
          stackSpace = initStackSpace;
          curLocalOffset = initLocalOffset;
          curParamOffset = initParamOffset;
        }

        currentFunctionLabel = label;
      }
    } else if (codeLine->containsTargetVar()) {
      auto targetVar = codeLine->getTargetVariable();
      auto funcVarPair = std::make_pair(currentFunctionLabel, targetVar);
        if (codeLine->getOperator().getName() == OperatorName::POP) {
          variableStackOffsetMap->insert(
              std::make_pair(funcVarPair, curParamOffset));
          curParamOffset += getSize(targetVar);

          this->functionVariableMap.at(currentFunctionLabel)
              .push_back(targetVar);
        } else {
          // if variable not parameter of function
          variableStackOffsetMap->insert(
              std::make_pair(funcVarPair, curLocalOffset));
          curLocalOffset -= getSize(targetVar);

          stackSpace += getSize(targetVar);
        }
    }
  }

  // add last function
  if (currentFunctionLabel) {
    this->setFunctionStackSpace(currentFunctionLabel, stackSpace);
  }
}

void RegisterManager::setFunctionStackSpace(Label::ptr_t functionLabel,
                                            unsigned stackSpace) {
  assert(functionLabel->isFunctionEntry() && "Not a function label!");
  (*functionStackSpaceMap)[functionLabel] = stackSpace;
}

unsigned RegisterManager::lookupFunctionStackSpace(Label::ptr_t functionLabel) {
  auto found = functionStackSpaceMap->find(functionLabel);

  if (found != functionStackSpaceMap->end()) {
    return found->second;
  } else {
    return 0;
  }
}

signed RegisterManager::lookupVariableStackOffset(Label::ptr_t functionLabel,
                                                  Variable::ptr_t var) {
  auto found = variableStackOffsetMap->find(std::make_pair(functionLabel, var));

  if (found != variableStackOffsetMap->end()) {
    return found->second;
  } else {
    return 0;
  }
}

unsigned RegisterManager::lookupFunctionArgSize(Label::ptr_t functionLabel) {
  auto found = functionArgSizeMap->find(functionLabel->getName());

  if (found != functionArgSizeMap->end()) {
    return found->second;
  } else {
    return 0;
  }
}

std::vector<Variable::ptr_t> RegisterManager::lookupFunctionVariables(
    Label::ptr_t functionLabel) {
  auto found = functionVariableMap.find(functionLabel);
  assert(found != functionVariableMap.end() &&
         "Function not found in variables map!");

  return found->second;
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
    Label::ptr_t functionLabel, Vertex vertex) const {
  auto vertexDescrMap = this->functionDescriptorMap->at(functionLabel);
  return vertexDescrMap.at(vertex);
}

std::string RegisterManager::toDot(std::string fucntionName) const {
  return this->toDot(this->tac.lookupFunction(fucntionName));
}

std::string RegisterManager::toDot(Label::ptr_t functionLabel) const {
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
                               Label::ptr_t functionLabel) const {
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

unsigned RegisterManager::graphColoring(Label::ptr_t functionLabel,
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
  auto order = smallestFirstOrdering(graph);

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

RegisterManager::order_map_type RegisterManager::smallestFirstOrdering(
    Graph g) {
  order_map_type order;

  std::vector<std::pair<VertexDescriptor, unsigned>> degrees;
  for (auto vp = vertices(g); vp.first != vp.second; ++vp.first) {
    degrees.push_back(
        std::make_pair(*vp.first, boost::out_degree(*vp.first, g)));
  }

  // sort ascending
  std::sort(degrees.begin(), degrees.end(),
            [](const std::pair<VertexDescriptor, unsigned> &a,
               const std::pair<VertexDescriptor, unsigned> &b) {
              return a.second < b.second;
            });

  for (unsigned i = 0; i < degrees.size(); ++i) {
    order[i] = degrees[i].first;
  }

  return order;
}

Operand::ptr_t RegisterManager::getLocationForVariable(
    Label::ptr_t functionLabel, Vertex vertex) {
  auto color = this->getColor(functionLabel, vertex);

  if (this->isColor(color)) {
    return this->getRegister(color);
  } else {
    signed varOffset = this->lookupVariableStackOffset(functionLabel, vertex);
    return std::make_shared<Operand>(varOffset);
  }
}

bool RegisterManager::isColor(unsigned color) {
  if (color < this->numOfRegForColoring)
    return true;
  else
    return false;
}

unsigned RegisterManager::getColor(Label::ptr_t functionLabel, Vertex vertex) {
  auto vd = this->lookupVertexDescr(functionLabel, vertex);
  auto colorMap = this->getFunctionGraphColorsMap()->at(functionLabel);

  return colorMap->at(vd);
}

RegisterManager::VertexDescriptor RegisterManager::getVertexDescriptor(
    Label::ptr_t functionLabel, Vertex vertex) {
  return this->lookupVertexDescr(functionLabel, vertex);
}

Operand::ptr_t RegisterManager::getRegister(unsigned color) {
  switch (color) {
    case 0:
      return std::make_shared<Operand>(Register::EBX);
    case 1:
      return std::make_shared<Operand>(Register::ECX);
    case 2:
      return std::make_shared<Operand>(Register::EDX);
    case 3:
      return std::make_shared<Operand>(Register::ESI);
    case 4:
      return std::make_shared<Operand>(Register::EDI);
    default:
      assert(false && "too few registers defined for reg alloc");
      return nullptr;
  }
}

Register RegisterManager::getTmpRegisterName() { return Register::EAX; }

Operand::ptr_t RegisterManager::getTmpRegister() {
  return std::make_shared<Operand>(true, this->getTmpRegisterName());
}
}
}
