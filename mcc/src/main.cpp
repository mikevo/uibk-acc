#include <fstream>
#include <iostream>
#include <sstream>

#include "mcc/cfg/cfg.h"
#include "mcc/gas/gas.h"
#include "mcc/lvn/lvn.h"
#include "mcc/main.h"
#include "mcc/tac/tac.h"

using namespace mcc::tac;
using namespace mcc::cfg;
using namespace mcc::lvn;
using namespace mcc::gas;

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "usage: mC [file]" << std::endl;
    return -1;
  }

  std::ifstream input{argv[1]};
  std::stringstream buffer;
  buffer << input.rdbuf();
  auto source = buffer.str();
  auto tree = parser::parse(source);

  if (!tree) {
    std::cout << "File: " << argv[1] << " is not a valid mC file." << std::endl;
    return -1;
  }

  formatted_ostream out(std::cout);
  out << "Parsed:\n" << tree << "\n";

  Tac tac = Tac(tree);
  std::cout << "Three-Adress Code:" << std::endl;
  std::cout << tac.toString() << std::endl;

  LVN::transform(tac);
  std::cout << std::endl << "Three-Adress Code after LVN:" << std::endl;
  std::cout << tac.toString() << std::endl;

  auto graph = std::make_shared<Cfg>(tac);
  std::cout << std::endl << "BB:" << std::endl;

  auto bbIndex = tac.getBasicBlockIndex();

  graph->computeLiveInOut();
  graph->computeWorkList();
  for (auto const b : *bbIndex.get()) {
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

  // GNU Assembly
  std::cout << "GAS:" << std::endl;
  Gas gas = Gas(tac);

  auto map1 = gas.getFunctionStackSpaceMap();
  for (auto it = map1->begin(); it != map1->end(); ++it) {
    std::cout << it->first.get() << ": Stack Space = " << it->second
              << std::endl;
  }

  auto map2 = gas.getVariableStackOffsetMap();
  for (auto it = map2->begin(); it != map2->end(); ++it) {
    std::cout << it->first.second->getValue()
              << ": Stack Offset = " << it->second << std::endl;
  }

  auto outFileName = std::string(argv[1]) + ".s";
  std::ofstream output;

  std::cout << "Assembly:" << std::endl << gas.toString();

  output.open(outFileName);

  output << gas;

  output.flush();
  output.close();

  auto libFileName = std::string(argv[0]);

  // remove mcc at end to get path to mcc file
  libFileName = libFileName.substr(0, libFileName.size() - 3);

  // relative location of lib.c to mcc executable
  libFileName.append("../lib.c");

  std::string gccCall = "gcc ";
  gccCall.append(outFileName);
  gccCall.append(" ");
  gccCall.append(libFileName);
  gccCall.append(" -m32");

  std::system(gccCall.c_str());

  std::ofstream dot;

  dot.open("/tmp/g.dot");
  dot << graph->toDot() << std::endl;
  dot.close();
}
