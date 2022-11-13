#include <iostream>
#include <fstream>
#include "ysl.hh"

int main(int argc, char** argv) {
	YSL::Environment          env;

	if (argc > 1) {
		std::ifstream             fhnd(argv[1]);
		std::string               line;

		while (getline(fhnd, line)) {
			env.Interpret(line);
		}
		fhnd.close();
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
