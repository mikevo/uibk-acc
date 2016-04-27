/*
 * scope_manager.h
 *
 *  Created on: Apr 21, 2016
 */

#ifndef INCLUDES_MCC_TAC_SCOPE_MANAGER_H_
#define INCLUDES_MCC_TAC_SCOPE_MANAGER_H_

#include "mcc/tac/scope.h"

#include "boost/graph/adjacency_list.hpp"

namespace mcc {
namespace tac {
class ScopeManager {
 public:
  typedef std::shared_ptr<ScopeManager> ptr_t;
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                                Scope::ptr_t> Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;

  ScopeManager();
  Scope::ptr_t const getCurrentScope() const;

  bool goToParentScope();
  Scope::ptr_t const addNewChildScope();
  Scope::ptr_t const addNewSiblingScope();

  void setCheckPoint();
  void goToCheckPoint();

 private:
  Graph graph;
  Scope::ptr_t root;
  Scope::ptr_t currentScope;
  Scope::ptr_t checkPoint;
  bool checkPointValid;
  std::map<Scope::ptr_t, VertexDescriptor> vertexMap;
  std::map<VertexDescriptor, Scope::ptr_t> descriptorMap;

  Scope::ptr_t addVertex(Scope::ptr_t v);
};
}
}

#endif /* INCLUDES_MCC_TAC_SCOPE_MANAGER_H_ */
