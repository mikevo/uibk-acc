/*
 * scope.cpp
 *
 *  Created on: Apr 21, 2016
 */

#include "mcc/tac/scope.h"

namespace mcc {
namespace tac {
Scope::Scope()
    : root(std::make_shared<ScopeNode>(0, 0)),
      currentScope(this->root),
      checkPoint(this->root),
      checkPointValid(false) {
  this->addVertex(this->currentScope);
}

Scope::Vertex const Scope::getCurrentScope() const { return currentScope; }

bool Scope::goToParent() {
  auto vd = this->vertexMap.at(this->currentScope);

  auto inEdges = boost::in_edges(vd, this->graph);

  if (inEdges.first != inEdges.second) {
    auto parent = boost::source(*inEdges.first, this->graph);

    this->currentScope = this->descriptorMap.at(parent);

    return true;
  } else {
    return false;
  }
}

Scope::Vertex const Scope::addNewChild() {
  auto childDepth = this->currentScope->getDepth() + 1;
  auto childIndex = this->currentScope->getNextIndex();
  auto child = std::make_shared<ScopeNode>(childDepth, childIndex);

  auto c = this->addVertex(child);

  boost::add_edge(this->vertexMap.at(this->currentScope), this->vertexMap.at(c),
                  this->graph);

  this->currentScope = c;

  return this->getCurrentScope();
}

Scope::Vertex const Scope::addNewSibling() {
  // TODO: maybe a restriction that causes trouble; if so it is necessary to
  // change the tree that it inserts a root that is never used as scope
  auto success = this->goToParent();
  assert(success && "you can not add a sibling to the root");

  return this->addNewChild();
}

void Scope::setCheckPoint() {
  this->checkPoint = this->currentScope;
  this->checkPointValid = true;
}

void Scope::goToCheckPoint() {
  if (this->checkPointValid) {
    this->currentScope = this->checkPoint;
    this->checkPointValid = false;
  }
}

Scope::Vertex Scope::addVertex(Scope::Vertex v) {
  auto d = boost::add_vertex(v, this->graph);

  vertexMap.insert(std::make_pair(v, d));
  descriptorMap.insert(std::make_pair(d, v));

  return descriptorMap.at(d);
}
}
}
