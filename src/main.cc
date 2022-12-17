#include <iostream>
#include <fstream>
#include "environment.hh"
#include "extensions/ysl_file.hh"
#include "extensions/ysl_system.hh"
#include "extensions/ysl_regex.hh"
#include "extensions/ysl_so.hh"

#ifndef YSL_PLATFORM_WINDOWS
	#include "extensions/ysl_posix.hh"
#endif

#ifndef YSL_VERSION
	#define YSL_VERSION "Unknown"
#endif

int main(int argc, char** argv) {
	Environment env;

	env.LoadExtension(YSL::Extensions::File::BuildExtension());
	env.LoadExtension(YSL::Extensions::System::BuildExtension());
	env.LoadExtension(YSL::Extensions::Regex::BuildExtension());
	env.LoadExtension(YSL::Extensions::So::BuildExtension());

	#ifndef YSL_PLATFORM_WINDOWS
		env.LoadExtension(YSL::Extensions::Posix::BuildExtension());
	#endif

	if (argc > 1) {
		if (std::string(argv[1]) == "--version") {
			printf("Version: %s\n", YSL_VERSION);
			return 0;
		}
		else if (std::string(argv[1]) == "--help") {
			puts(
				"YSL help\n"
				"--version : show what commit this version was built on\n"
				"--help    : show this menu"
			);
			return 0;
		}
		else {
			env.Interpret(std::string("load ") + argv[1]);
			env.Interpret("run");
		}
	}
	else {
		puts("YSL repl");
		while (true) {
			std::string input;
			fputs("> ", stdout);
			std::getline(std::cin, input, '\n');

			env.Interpret(input);
		}
	}

	return 0;
}
