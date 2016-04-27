/*
 * scope_manager.cpp
 *
 *  Created on: Apr 21, 2016
 */

#include "mcc/tac/scope_manager.h"

namespace mcc {
namespace tac {
ScopeManager::ScopeManager()
    : root(std::make_shared<ScopeNode>(0, 0)),
      currentScope(this->root),
      checkPoint(this->root),
      checkPointValid(false) {
  this->addVertex(this->currentScope);
}

ScopeManager::Vertex const ScopeManager::getCurrentScope() const {
  return currentScope;
}

bool ScopeManager::goToParentScope() {
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

ScopeManager::Vertex const ScopeManager::addNewChildScope() {
  auto childDepth = this->currentScope->getDepth() + 1;
  auto childIndex = this->currentScope->getNextIndex();
  auto child = std::make_shared<ScopeNode>(childDepth, childIndex);

  auto c = this->addVertex(child);

  boost::add_edge(this->vertexMap.at(this->currentScope), this->vertexMap.at(c),
                  this->graph);

  this->currentScope = c;

  return this->getCurrentScope();
}

ScopeManager::Vertex const ScopeManager::addNewSiblingScope() {
  // TODO: maybe a restriction that causes trouble; if so it is necessary to
  // change the tree that it inserts a root that is never used as scope
  auto success = this->goToParentScope();
  assert(success && "you can not add a sibling to the root");

  return this->addNewChildScope();
}

void ScopeManager::setCheckPoint() {
  this->checkPoint = this->currentScope;
  this->checkPointValid = true;
}

void ScopeManager::goToCheckPoint() {
  if (this->checkPointValid) {
    this->currentScope = this->checkPoint;
    this->checkPointValid = false;
  }
}

ScopeManager::Vertex ScopeManager::addVertex(ScopeManager::Vertex v) {
  auto d = boost::add_vertex(v, this->graph);

  vertexMap.insert(std::make_pair(v, d));
  descriptorMap.insert(std::make_pair(d, v));

  return descriptorMap.at(d);
}
}
}
