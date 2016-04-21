#include <iostream>
#include <fstream>
#include <sstream>

#include "mcc/main.h"
#include "mcc/tac/tac.h"
#include "mcc/lvn/lvn.h"

using namespace mcc::tac;

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
    
    mcc::lvn::LVN::transform(tac);
    
    std::cout << std::endl << "Three-Adress Code after LVN:" << std::endl;
    std::cout << tac.toString() << std::endl;
}
