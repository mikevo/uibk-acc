#include "mcc/cfg/static_call_graph.h"

#include <cassert>
#include <iterator>
#include <memory>
#include <ostream>
#include <typeinfo>
#include <vector>

#include "mcc/cfg/set_helper.h"
#include "mcc/tac/operator.h"

namespace mcc {
namespace cfg {

Scg::Scg(mcc::tac::Tac &tac) {
  auto functionMap = tac.getFunctionMap();
  auto graphIndex = std::map<std::string, VertexDescriptor>();
  Label::ptr_t currentFunction;

  for (auto function : *functionMap) {
    auto vt = boost::add_vertex(function.second, graph);
    graphIndex.insert(std::make_pair(function.second->getName(), vt));
  }

  for (auto triple : tac.codeLines) {
    if (typeid(*triple) == typeid(Label)) {
      auto label = std::static_pointer_cast<Label>(triple);
      auto functionEntry = tac.lookupFunction(label->getName());
      if (functionEntry != nullptr) {
        currentFunction = functionEntry;
      }
    } else if (triple->getOperator().getType() == OperatorType::CALL) {
      if (typeid(*(triple->getArg1())) == typeid(Label)) {
        auto targetFunction =
            std::static_pointer_cast<Label>(triple->getArg1());
        boost::add_edge(graphIndex[currentFunction->getName()],
                        graphIndex[targetFunction->getName()], graph);
      }
    }
  }
}

std::string Scg::toDot() const {
  std::ostringstream out;
  auto name = boost::make_transform_value_property_map(
      Name{}, boost::get(boost::vertex_bundle, graph));
  boost::write_graphviz(out, graph, boost::make_label_writer(name));
  return out.str();
}

void Scg::storeDot(std::string fileName) const {
  std::ofstream outf(fileName);
  outf << toDot();
}
}
}
