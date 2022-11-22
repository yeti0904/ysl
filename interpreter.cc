#include <iostream>
#include <fstream>
#include "ysl.hh"
#include "extensions/ysl_file.hh"
#include "extensions/ysl_system.hh"
#include "extensions/ysl_regex.hh"

#ifndef YSL_PLATFORM_WINDOWS
	#include "extensions/ysl_posix.hh"
#endif

int main(int argc, char** argv) {
	YSL::Environment env;

	env.LoadExtension(YSL::Extensions::File::BuildExtension());
	env.LoadExtension(YSL::Extensions::System::BuildExtension());
	env.LoadExtension(YSL::Extensions::Regex::BuildExtension());

	#ifndef YSL_PLATFORM_WINDOWS
		env.LoadExtension(YSL::Extensions::Posix::BuildExtension());
	#endif

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
