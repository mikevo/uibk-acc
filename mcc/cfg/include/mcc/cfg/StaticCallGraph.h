#ifndef MCC_SCG_H
#define MCC_SCG_H

#include <memory>
#include <string>
#include <vector>

#include "mcc/tac/tac.h"
#include "mcc/tac/label.h"

#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/graphviz.hpp"

using namespace mcc::tac;

struct Name { std::string operator()(Label::ptr_t const& sp) const { return sp->getName(); } };

namespace mcc {
namespace cfg {
    
typedef mcc::tac::Label::ptr_t Vertex;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, Vertex> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;

class Scg {
 public:
  typedef std::shared_ptr<Scg> ptr_t;
  Scg(mcc::tac::Tac &tac);
  
  std::string toDot() const;
  void storeDot(std::string fileName) const;
  
private:
    Graph graph;
  
};
}
}

#endif /* SCG_H */
