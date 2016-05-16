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

  Tac tac = Tac(tree);
  LVN::transform(tac);

  auto graph = std::make_shared<Cfg>(tac);

  Gas gas = Gas(tac);

  auto outFileName = std::string(argv[1]) + ".s";

  std::ofstream output{outFileName};

  output << gas;

  output.flush();
  output.close();
}
