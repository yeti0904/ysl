#include <iostream>
#include <fstream>
#include "environment.hh"
#include "extensions/ysl_file.hh"
#include "extensions/ysl_system.hh"
#include "extensions/ysl_regex.hh"
#include "extensions/ysl_so.hh"
#include "extensions/ysl_ysl.hh"
#ifndef YSL_NO_GRAPHICS
	#include "extensions/ysl_gfx.hh"
#endif
#ifndef YSL_NO_CURL
	#include "extensions/ysl_curl.hh"
#endif

#if defined(YSL_PLATFORM_WINDOWS) || defined(YSL_PLATFORM_APPLE)
#else
	#include "extensions/ysl_posix.hh"
#endif

#ifndef YSL_VERSION
	#define YSL_VERSION "Unknown"
#endif

#ifndef YSL_ISLIB
int main(int argc, char** argv) {
	Environment env;

	env.LoadExtension(YSL::Extensions::File::BuildExtension());
	env.LoadExtension(YSL::Extensions::System::BuildExtension());
	env.LoadExtension(YSL::Extensions::Regex::BuildExtension());
	env.LoadExtension(YSL::Extensions::So::BuildExtension());
	#ifndef YSL_NO_GRAPHICS
		env.LoadExtension(YSL::Extensions::Gfx::BuildExtension());
	#endif
	#ifndef YSL_NO_CURL
		env.LoadExtension(YSL::Extensions::Curl::BuildExtension());
	#endif

	#ifndef YSL_PLATFORM_WINDOWS
		env.LoadExtension(YSL::Extensions::Posix::BuildExtension());
	#endif
	env.LoadExtension(YSL::Extensions::Ysl::BuildExtension());

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
#endif
