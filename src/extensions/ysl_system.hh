#ifndef YSL_EXTENSION_SYSTEM_HH
#define YSL_EXTENSION_SYSTEM_HH

#include "../ysl.hh"
#include <stdlib.h>

namespace YSL {
	namespace Extensions {
		namespace System {
			std::vector <int> Command(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 1, "System.Command: Needs 1 argument");

				system(args[0].c_str());
				return {};
			}
			std::vector <int> GetEnv(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 1, "System.Getenv: Needs 1 argument");

				char* res = getenv(args[0].c_str());

				return res == nullptr?
					std::vector <int>{} : YSL::Util::StringToIntVector(args[0]);
			}
			std::vector <int> SetEnv(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 2, "System.SetEnv: Needs 2 arguments");

				setenv(args[0].c_str(), args[1].c_str(), 1);

				return {};
			}
			std::vector <int> UnSetEnv(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 1, "System.UnSetEnv: Needs 1 argument");

				unsetenv(args[0].c_str());

				return {};
			}

			YSL::Extension BuildExtension() {
				YSL::Extension ext;
				
				ext.name                  = "system";
				ext.functions["command"]  = Command;
				ext.functions["getenv"]   = GetEnv;
				ext.functions["setenv"]   = SetEnv;
				ext.functions["unsetenv"] = UnSetEnv;

				return ext;
			}
		}
	}
}

#endif
