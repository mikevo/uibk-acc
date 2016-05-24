/*
 * register_manager.h
 *
 *  Created on: May 23, 2016
 */

#ifndef INCLUDES_MCC_GAS_REGISTER_MANAGER_H_
#define INCLUDES_MCC_GAS_REGISTER_MANAGER_H_

#include <map>
#include <memory>

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

  typedef std::pair<mcc::tac::Tac::code_lines_iter, VertexDescriptor>
      iter_descr_pair_type;

  typedef std::map<Vertex, std::vector<iter_descr_pair_type>,
                   mcc::tac::Variable::less> vertex_range_map_type;

  typedef std::map<mcc::tac::Label::ptr_t, vertex_range_map_type,
                   mcc::tac::Label::less> function_descr_map_type;

 public:
  typedef std::shared_ptr<RegisterManager> ptr_t;

  RegisterManager(mcc::tac::Tac &tac);

  function_graph_map_type getFunctionGraphMap();
  VertexDescriptor lookupVertexDescr(mcc::tac::Label::ptr_t functionLabel,
                                     Vertex vertex,
                                     mcc::tac::Tac::code_lines_iter it) const;

  std::string toDot(std::string fucntionName) const;
  std::string toDot(mcc::tac::Label::ptr_t functionLabel) const;
  void storeDot(std::string fileName, std::string fucntionName) const;
  void storeDot(std::string fileName,
                mcc::tac::Label::ptr_t functionLabel) const;

 private:
  mcc::tac::Tac &tac;
  function_graph_map_type functionGraphMap;
  function_descr_map_type functionDescriptorMap;
};
}
}

#endif /* INCLUDES_MCC_GAS_REGISTER_MANAGER_H_ */
