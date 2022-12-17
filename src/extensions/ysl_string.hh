#ifndef YSL_EXTENSION_ARRAY_HH
#define YSL_EXTENSION_ARRAY_HH

#include "../ysl.hh"

namespace YSL {
	namespace Extensions {
		namespace String {
			std::vector <int> Cmp(std::vector <std::string> args, Environment& env) {
				if (args.size() < 2) {
					fprintf(stderr, "String.Cmp: Needs at least 2 arguments\n");
					env.ExitError();
				}

				return args[0] == args[1]?
					std::vector <int> {1} : std::vector <int> {0};
			}
		}
	}
}

#endif
