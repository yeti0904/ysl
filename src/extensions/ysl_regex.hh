#ifndef YSL_EXTENSION_REGEX_HH
#define YSL_EXTENSION_REGEX_HH

#include "../environment.hh"
#include <regex>

namespace YSL {
	namespace Extensions {
		namespace Regex {
			std::vector <int> Valid(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 1, "Regex.Valid: Needs 1 argument");

				try {
					std::regex re(args[0]);
				}
				catch (...) {
					return {0};
				}

				return {1};
			}
			std::vector <int> Match(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 2, "Regex.Match: Needs 2 arguments");

				std::regex re;
				try {
					re = std::regex(args[0]);
				}
				catch (...) {
					env.Assert(false, "Regex.Match: Invalid regex: " + args[0]);
				}

				std::smatch m;

				return std::regex_match(args[1], m, re)?
					std::vector <int>{1} : std::vector <int>{0};
			}

			Extension BuildExtension() {
				Extension ext;
				
				ext.name               = "regex";
				ext.functions["valid"] = Valid;
				ext.functions["match"] = Match;

				return ext;
			}
		}
	}
}

#endif
