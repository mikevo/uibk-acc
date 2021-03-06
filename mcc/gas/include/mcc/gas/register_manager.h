/*
 * register_manager.h
 *
 *  Created on: May 23, 2016
 */

#ifndef INCLUDES_MCC_GAS_REGISTER_MANAGER_H_
#define INCLUDES_MCC_GAS_REGISTER_MANAGER_H_

#include <map>
#include <memory>

#include "mcc/gas/operand.h"
#include "mcc/tac/label.h"
#include "mcc/tac/tac.h"
#include "mcc/tac/variable.h"

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/graphviz.hpp"

using namespace mcc::tac;

namespace mcc {
namespace gas {
typedef std::map<Label::ptr_t, unsigned, Label::less>
    function_stack_space_map_type;
typedef std::pair<Label::ptr_t, Variable::ptr_t>
    variable_stack_offset_map_key_type;
typedef std::map<variable_stack_offset_map_key_type, signed>
    variable_stack_offset_map_type;

typedef std::pair<Label::ptr_t, Array::ptr_t> array_stack_offset_map_key_type;
struct labelArrayPairLess {
  bool operator()(const array_stack_offset_map_key_type &lhs,
                  const array_stack_offset_map_key_type &rhs) const {
    return Label::less()(lhs.first, rhs.first) ||
           Array::less()(lhs.second, rhs.second);
  }
};

typedef std::map<array_stack_offset_map_key_type, signed, labelArrayPairLess>
    array_stack_offset_map_type;

typedef std::map<std::string, unsigned> function_arg_size_type;
typedef std::map<Label::ptr_t, std::vector<Variable::ptr_t>, Label::less>
    func_var_map_type;

class RegisterManager {
  typedef Variable::ptr_t Vertex;
  typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS,
                                Vertex> Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;

  typedef std::map<Label::ptr_t, Graph, Label::less> function_graph_map_type;

  typedef boost::property_map<Graph, boost::vertex_index_t>::const_type
      vertex_index_map;
  typedef boost::graph_traits<Graph>::vertices_size_type vertices_size_type;
  // key: vertex descriptor, value: color index
  typedef std::map<RegisterManager::VertexDescriptor, unsigned>
      graph_color_map_type;
  // key: function label, value: color map (see above)
  typedef std::map<Label::ptr_t, std::shared_ptr<graph_color_map_type>>
      function_graph_color_map_type;
  // key: function label, value: number of colors
  typedef std::map<Label::ptr_t, unsigned> num_colors_map_type;
  typedef boost::vector_property_map<RegisterManager::VertexDescriptor>
      order_map_type;

  typedef std::pair<Tac::code_lines_iter, VertexDescriptor>
      iter_descr_pair_type;

  typedef std::map<Vertex, VertexDescriptor, Variable::less>
      vertex_range_map_type;

  typedef std::map<Label::ptr_t, vertex_range_map_type, Label::less>
      function_descr_map_type;

 public:
  typedef std::shared_ptr<RegisterManager> ptr_t;

  RegisterManager(Tac &tac);

  function_graph_map_type getFunctionGraphMap();
  unsigned getNumOfRegForColoring() const;

  std::shared_ptr<num_colors_map_type> getNumColorsMap();
  std::shared_ptr<function_graph_color_map_type> getFunctionGraphColorsMap();

  std::string toDot(std::string fucntionName) const;
  std::string toDot(Label::ptr_t functionLabel) const;
  void storeDot(std::string fileName, std::string fucntionName) const;
  void storeDot(std::string fileName, Label::ptr_t functionLabel) const;

  unsigned getSize(Type t);
  unsigned getSize(std::shared_ptr<mcc::tac::Operand> operand);
  unsigned getSize(std::vector<Tac::type_size_type> argList);

  unsigned lookupFunctionStackSpace(Label::ptr_t functionLabel);
  signed lookupVariableStackOffset(Label::ptr_t functionLabel,
                                   Variable::ptr_t var);
  signed lookupArrayStackOffset(Label::ptr_t functionLabel, Array::ptr_t arr);
  unsigned lookupFunctionArgSize(Label::ptr_t functionLabel);
  std::vector<Variable::ptr_t> lookupFunctionVariables(
      Label::ptr_t functionLabel);

  Operand::ptr_t getLocationForVariable(Label::ptr_t functionLabel,
                                        Vertex vertex);
  Operand::ptr_t getLocationForArray(Label::ptr_t functionLabel,
                                     Array::ptr_t arr);

  Register getTmpRegisterName();
  Operand::ptr_t getTmpRegister();

 private:
  std::shared_ptr<function_stack_space_map_type> functionStackSpaceMap;
  std::shared_ptr<variable_stack_offset_map_type> variableStackOffsetMap;
  std::shared_ptr<array_stack_offset_map_type> arrayStackOffsetMap;
  std::shared_ptr<function_arg_size_type> functionArgSizeMap;
  func_var_map_type functionVariableMap;

  void generateInterferenceGraphs();

  void analyzeStackUsages();
  void setFunctionStackSpace(Label::ptr_t functionLabel, unsigned stackSpace);

  void graphColoring(unsigned n);
  unsigned graphColoring(std::string fucntionName, unsigned n);
  unsigned graphColoring(Label::ptr_t functionLabel, unsigned n);
  unsigned graphColoring(Graph graph,
                         std::shared_ptr<graph_color_map_type> colors,
                         unsigned n);
  order_map_type smallestFirstOrdering(Graph g);

  VertexDescriptor lookupVertexDescr(Label::ptr_t functionLabel,
                                     Vertex vertex) const;

  bool isColor(unsigned color);
  unsigned getColor(Label::ptr_t functionLabel, Vertex vertex);
  VertexDescriptor getVertexDescriptor(Label::ptr_t functionLabel,
                                       Vertex vertex);
  Operand::ptr_t getRegister(unsigned color);

  Tac &tac;
  function_graph_map_type functionGraphMap;
  // key: function label, value: color map (see above)
  std::shared_ptr<function_graph_color_map_type> functionGraphColorsMap;
  // key: function label, value: number of colors
  std::shared_ptr<num_colors_map_type> numColorsMap;
  std::shared_ptr<function_descr_map_type> functionDescriptorMap;
  unsigned numOfRegForColoring;
  bool tmpToggle = false;
};
}
}

#endif /* INCLUDES_MCC_GAS_REGISTER_MANAGER_H_ */
