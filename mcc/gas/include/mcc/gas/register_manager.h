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

class RegisterManager {
  typedef mcc::tac::Variable::ptr_t Vertex;
  typedef boost::adjacency_list<boost::setS, boost::setS, boost::undirectedS,
                                Vertex> Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;

  typedef std::map<mcc::tac::Label::ptr_t, Graph, mcc::tac::Label::less>
      function_graph_map_type;

 public:
  typedef std::shared_ptr<RegisterManager> ptr_t;

  RegisterManager(mcc::tac::Tac &tac);

 private:
  function_graph_map_type functionGraphMap;
};

#endif /* INCLUDES_MCC_GAS_REGISTER_MANAGER_H_ */
