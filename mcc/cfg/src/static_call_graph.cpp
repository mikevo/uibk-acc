#include "mcc/cfg/static_call_graph.h"

#include <cassert>
#include <iterator>
#include <memory>
#include <ostream>
#include <typeinfo>
#include <vector>

#include "mcc/cfg/set_helper.h"
#include "mcc/tac/operator.h"
#include "mcc/tac/helper/ast_converters.h"

namespace mcc {
namespace cfg {

Scg::Scg(mcc::tac::Tac &tac) {
  auto functionMap = tac.getFunctionMap();
  auto graphIndex = std::map<std::string, VertexDescriptor>();
  std::string currentFunctionName;

  for (auto function : *functionMap) {
    auto vt = boost::add_vertex(function.second, graph);
    graphIndex.insert(std::make_pair(function.second->getName(), vt));
  }

  for (auto triple : tac.codeLines) {
    if (mcc::tac::helper::isType<Label>(triple)) {
      auto label = std::static_pointer_cast<Label>(triple);
      if (label->isFunctionEntry()) {
        currentFunctionName = label->getName();
      }
    } else if (triple->getOperator().getType() == OperatorType::CALL) {
      if (mcc::tac::helper::isType<Label>(triple->getArg1())) {
        auto callTarget = std::static_pointer_cast<Label>(triple->getArg1());
        auto targetName = callTarget->getName();
        boost::add_edge(graphIndex[currentFunctionName], graphIndex[targetName],
                        graph);
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
