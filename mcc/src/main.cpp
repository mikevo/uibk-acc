#include <iostream>
#include <fstream>
#include <sstream>

#include "mcc/main.h"
#include "mcc/tac/tac.h"
#include "mcc/lvn/lvn.h"
#include "mcc/cfg/cfg.h"

using namespace mcc::tac;
using namespace mcc::cfg;

int main(int argc, char** argv) {

	if(argc!=2) {
		std::cout << "usage: mC [file]" << std::endl;
		return -1;
	}

	std::ifstream input{argv[1]};
	std::stringstream buffer;
	buffer << input.rdbuf();
	auto source = buffer.str();

	auto tree = parser::parse(source);

	formatted_ostream out(std::cout);
	out << "Parsed:\n" << tree << "\n";
    
    // three adress code
    Tac tac = Tac(tree);
	std::cout << "Three-Adress Code:" << std::endl;
    std::cout << tac.toString() << std::endl;
    
//    mcc::lvn::LVN::transform(tac);
//    
//    std::cout << std::endl << "Three-Adress Code after LVN:" << std::endl;
//    std::cout << tac.toString() << std::endl;
    
    auto graph = std::make_shared<Cfg>(tac);
    std::cout << std::endl << "BB:" << std::endl;

    auto bbIndex = tac.getBasicBlockIndex();

    graph->computeLive();
    graph->computeWorkList();
    graph->computeAvailableExpressions();
    for (auto const b : *bbIndex.get()) {
      std::cout << "Avail: { ";
      for (auto const avail : graph->getAvail(b->getBlockId())) {
          std::cout << avail->toString() << ", ";
      }
      std::cout << "}" << std::endl;

      std::cout << "NotKilledExpr: { ";
      for (auto const notKilled : graph->getNotKilledExpr(b->getBlockId())) {
          std::cout << notKilled->toString() << ", ";
      }
      std::cout << "}" << std::endl;

      std::cout << "DeExpr: { ";
      for (auto const deExpr : b->getDeExpr()) {
          std::cout << deExpr->toString() << ", ";
      }
      std::cout << "}" << std::endl;

      std::cout << "LiveIn: { ";
      for (auto const in : graph->getLiveIn(b->getBlockId())) {
          std::cout << in->getName() << ", ";
      }
      std::cout << "}" << std::endl;

      std::cout << b->toString();

      std::cout << "LiveOut: { ";
      for (auto const out : graph->getLiveOut(b->getBlockId())) {
          std::cout << out->getName() << ", ";
      }
      std::cout << "}" << std::endl << std::endl;
    }
}
