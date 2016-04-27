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
  typedef std::shared_ptr<Scope> Vertex;
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                                Vertex> Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;

  ScopeManager();
  Vertex const getCurrentScope() const;

  bool goToParentScope();
  Vertex const addNewChildScope();
  Vertex const addNewSiblingScope();

  void setCheckPoint();
  void goToCheckPoint();

 private:
  Graph graph;
  Vertex root;
  Vertex currentScope;
  Vertex checkPoint;
  bool checkPointValid;
  std::map<Vertex, VertexDescriptor> vertexMap;
  std::map<VertexDescriptor, Vertex> descriptorMap;

  ScopeManager::Vertex addVertex(Vertex v);
};
}
}

#endif /* INCLUDES_MCC_TAC_SCOPE_MANAGER_H_ */
