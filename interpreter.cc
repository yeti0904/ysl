#include <iostream>
#include <fstream>
#include "ysl.hh"

int main(int argc, char** argv) {
	YSL::Environment env;

	if (argc > 1) {
		env.Interpret(std::string("load ") + argv[1]);
		env.Interpret("run");
	}
	else {
		while (true) {
			std::string input;
			fputs("> ", stdout);
			std::getline(std::cin, input, '\n');

			env.Interpret(input);
		}
	}

	return 0;
}
