/*
 * register_manager.h
 *
 *  Created on: May 23, 2016
 */

#ifndef INCLUDES_MCC_GAS_REGISTER_MANAGER_H_
#define INCLUDES_MCC_GAS_REGISTER_MANAGER_H_

#include <map>
#include <memory>

// resolve circular dependency
namespace mcc {
namespace gas {
class Gas;
}
}

#include "mcc/gas/gas.h"
#include "mcc/gas/operand.h"
#include "mcc/tac/label.h"
#include "mcc/tac/tac.h"
#include "mcc/tac/variable.h"

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/graphviz.hpp"

namespace mcc {
namespace gas {

class RegisterManager {
  typedef mcc::tac::Variable::ptr_t Vertex;
  typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS,
                                Vertex> Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;

  typedef std::map<mcc::tac::Label::ptr_t, Graph, mcc::tac::Label::less>
      function_graph_map_type;

  typedef boost::property_map<Graph, boost::vertex_index_t>::const_type
      vertex_index_map;
  typedef boost::graph_traits<Graph>::vertices_size_type vertices_size_type;
  // key: vertex descriptor, value: color index
  typedef std::map<RegisterManager::VertexDescriptor, unsigned>
      graph_color_map_type;
  // key: function label, value: color map (see above)
  typedef std::map<mcc::tac::Label::ptr_t,
                   std::shared_ptr<graph_color_map_type>>
      function_graph_color_map_type;
  // key: function label, value: number of colors
  typedef std::map<mcc::tac::Label::ptr_t, unsigned> num_colors_map_type;
  typedef boost::vector_property_map<RegisterManager::VertexDescriptor>
      order_map_type;

  typedef std::pair<mcc::tac::Tac::code_lines_iter, VertexDescriptor>
      iter_descr_pair_type;

  typedef std::map<Vertex, VertexDescriptor, mcc::tac::Variable::less>
      vertex_range_map_type;

  typedef std::map<mcc::tac::Label::ptr_t, vertex_range_map_type,
                   mcc::tac::Label::less> function_descr_map_type;

 public:
  typedef std::shared_ptr<RegisterManager> ptr_t;

  RegisterManager(mcc::tac::Tac &tac, Gas *gas = nullptr);

  function_graph_map_type getFunctionGraphMap();
  unsigned getNumOfRegForColoring() const;

  std::shared_ptr<num_colors_map_type> getNumColorsMap();
  std::shared_ptr<function_graph_color_map_type> getFunctionGraphColorsMap();

  std::string toDot(std::string fucntionName) const;
  std::string toDot(mcc::tac::Label::ptr_t functionLabel) const;
  void storeDot(std::string fileName, std::string fucntionName) const;
  void storeDot(std::string fileName,
                mcc::tac::Label::ptr_t functionLabel) const;

  Operand::ptr_t getLocationForVariable(mcc::tac::Label::ptr_t functionLabel,
                                        Vertex vertex);

  Register getTmpRegisterName();
  Operand::ptr_t getTmpRegister();

 private:
  void graphColoring(unsigned n);
  unsigned graphColoring(std::string fucntionName, unsigned n);
  unsigned graphColoring(mcc::tac::Label::ptr_t functionLabel, unsigned n);
  unsigned graphColoring(Graph graph,
                         std::shared_ptr<graph_color_map_type> colors,
                         unsigned n);
  order_map_type smallestFirstOrdering(Graph g);

  VertexDescriptor lookupVertexDescr(mcc::tac::Label::ptr_t functionLabel,
                                     Vertex vertex) const;

  bool isColor(unsigned color);
  unsigned getColor(mcc::tac::Label::ptr_t functionLabel, Vertex vertex);
  VertexDescriptor getVertexDescriptor(mcc::tac::Label::ptr_t functionLabel,
                                       Vertex vertex);
  Operand::ptr_t getRegister(unsigned color);
  //  void storeSpilledVariable(Vertex vertex);

  mcc::tac::Tac &tac;
  Gas *gas;
  function_graph_map_type functionGraphMap;
  // key: function label, value: color map (see above)
  std::shared_ptr<function_graph_color_map_type> functionGraphColorsMap;
  // key: function label, value: number of colors
  std::shared_ptr<num_colors_map_type> numColorsMap;
  std::shared_ptr<function_descr_map_type> functionDescriptorMap;
  unsigned numOfRegForColoring;
};
}
}

#endif /* INCLUDES_MCC_GAS_REGISTER_MANAGER_H_ */
