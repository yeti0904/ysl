#ifndef YSL_EXTENSION_YSL_HH
#define YSL_EXTENSION_YSL_HH

#include "../environment.hh"
#include "../util.hh"

namespace YSL {
	namespace Extensions {
		namespace Ysl {
			std::vector <int> ReturnPush(
				const std::vector <std::string>& args, Environment& env
			) {
				env.Assert(args.size() == 1, "Ysl.ReturnPush: Requires 1 argument");
			
				auto& arr = env.variables[args[0]];

				env.Assert(!arr.empty(), "Ysl.ReturnPush: Array empty");

				env.returnValues.push_back(arr);
				return {};
			}

			std::vector <int> WriteLine(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 2, "Ysl.WriteLine: Requires 2 arguments");
				env.Assert(Util::IsInteger(args[0]), "YSL.WriteLine: Bad line number");

				env.program[stoi(args[0])] = args[1];

				return {};
			}
			
			Extension BuildExtension() {
				Extension ext;

				ext.name                     = "ysl";
				ext.functions["return_push"] = ReturnPush;
				ext.functions["write_line"]  = WriteLine;

				return ext;
			}
		}
	}
}

#endif
