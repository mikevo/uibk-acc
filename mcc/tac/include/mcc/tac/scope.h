/*
 * scope.h
 *
 *  Created on: Apr 21, 2016
 */

#ifndef INCLUDES_MCC_TAC_SCOPE_H_
#define INCLUDES_MCC_TAC_SCOPE_H_

#include "mcc/tac/scope_node.h"

#include "boost/graph/adjacency_list.hpp"

namespace mcc {
namespace tac {
class Scope {
 public:
  typedef std::shared_ptr<ScopeNode> Vertex;
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                                Vertex> Graph;
  typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;

  Scope();
  Vertex const getCurrentScope() const;

  bool goToParent();
  Vertex const addNewChild();
  Vertex const addNewSibling();

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

  Scope::Vertex addVertex(Vertex v);
};
}
}

#endif /* INCLUDES_MCC_TAC_SCOPE_H_ */
