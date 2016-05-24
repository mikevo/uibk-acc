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

 public:
  typedef std::shared_ptr<RegisterManager> ptr_t;

  RegisterManager(mcc::tac::Tac &tac);

  function_graph_map_type getFunctionGraphMap();

  std::string toDot(std::string fucntionName) const;
  std::string toDot(mcc::tac::Label::ptr_t functionLabel) const;
  void storeDot(std::string fileName, std::string fucntionName) const;
  void storeDot(std::string fileName,
                mcc::tac::Label::ptr_t functionLabel) const;

 private:
  mcc::tac::Tac &tac;
  function_graph_map_type functionGraphMap;
};
}
}

#endif /* INCLUDES_MCC_GAS_REGISTER_MANAGER_H_ */
