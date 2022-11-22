#ifndef YSL_EXTENSION_POSIX_HH
#define YSL_EXTENSION_POSIX_HH

#include "../ysl.hh"
#include <unistd.h>

#ifdef YSL_PLATFORM_WINDOWS
	#error "Posix extension is being used on windows"
#endif

namespace YSL {
	namespace Extensions {
		namespace Posix {
			std::vector <int> Execv(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 2, "Posix.Execv: Requires 2 arguments");

				auto argv = YSL::Util::StringVectorToCharpVector(
					YSL::Util::IntVectorToStringVector(env.variables[args[1]])
				);

				execv(
					args[0].c_str(), argv.data()
				);

				for (auto& ptr : argv) {
					free(ptr);
				}

				return {};
			}
			std::vector <int> Execvp(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 2, "Posix.Execvp: Requires 2 arguments");

				auto argv = YSL::Util::StringVectorToCharpVector(
					YSL::Util::IntVectorToStringVector(env.variables[args[1]])
				);

				execvp(
					args[0].c_str(), argv.data()
				);

				for (auto& ptr : argv) {
					free(ptr);
				}

				return {};
			}
			std::vector <int> Execvpe(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 3, "Posix.Execvpe: Requires 3 arguments");

				auto argv = YSL::Util::StringVectorToCharpVector(
					YSL::Util::IntVectorToStringVector(env.variables[args[1]])
				);
				auto envp = YSL::Util::StringVectorToCharpVector(
					YSL::Util::IntVectorToStringVector(env.variables[args[1]])
				);

				execvpe(
					args[0].c_str(), argv.data(), envp.data()
				);

				for (auto& ptr : argv) {
					free(ptr);
				}
				for (auto& ptr : envp) {
					free(ptr);
				}

				return {};
			}

			YSL::Extension BuildExtension() {
				YSL::Extension ext;

				ext.name                 = "posix";
				ext.functions["execv"]   = Execv;
				ext.functions["execvp"]  = Execvp;
				ext.functions["execvpe"] = Execvpe;

				return ext;
			}
		}
	}
}

#endif
