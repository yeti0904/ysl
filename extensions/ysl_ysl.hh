#ifndef YSL_EXTENSION_YSL_HH
#define YSL_EXTENSION_YSL_HH

#include "../ysl.hh"

namespace YSL {
	namespace Extensions {
		std::vector <int> ReturnPush(
			const std::vector <std::string>& args, Environment& env
		) {
			env.Assert(args.size() == 1, "Ysl.ReturnPush: Requires 1 argument");
		
			auto& arr = env.variables[args[0]];

			env.Assert(!arr.empty(), "Ysl.ReturnPush: Array empty");

			env.returnValues.push_back(arr);
			return {};
		}
		YSL::Extension BuildExtension() {
			YSL::Extension ext;

			ext.name                     = "ysl";
			ext.functions["return_push"] = ReturnPush;

			return ext;
		}
	}
}

#endif
