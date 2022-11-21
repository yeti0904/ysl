#ifndef YSL_EXTENSION_ARRAY_HH
#define YSL_EXTENSION_ARRAY_HH

#include "../ysl.hh"
#include <stdlib.h>

namespace YSL {
	namespace Extensions {
		namespace System {
			std::vector <int> Command(std::vector <std::string> args, Environment& env) {
				env.Assert(args.size() == 1, "System.Command: Needs 1 argument");

				system(args[0].c_str());
				return {};
			}

			YSL::Extension BuildExtension() {
				YSL::Extension ext;
				
				ext.name                 = "system";
				ext.functions["command"] = Command;

				return ext;
			}
		}
	}
}

#endif
