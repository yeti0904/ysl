#ifndef YSL_EXTENSION_POSIX_HH
#define YSL_EXTENSION_POSIX_HH

#include "../ysl.hh"
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

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
			std::vector <int> Fork(const std::vector <std::string>&, Environment&) {
				return {fork()};
			}
			std::vector <int> Wait(const std::vector <std::string>&, Environment&) {
				return {wait(nullptr)};
			}
			std::vector <int> Open(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 2, "Posix.Open: Requires 2 arguments");
				env.Assert(
					YSL::Util::IsInteger(args[1]),
					"Posix.Open: 2nd argument must be integer"
				);

				return {open(args[0].c_str(), stoi(args[1]))};
			}
			std::vector <int> Close(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 1, "Posix.Close: Requires 1 argument");
				env.Assert(YSL::Util::IsInteger(args[0]), "Posix.Close: Invalid FD");

				return {close(stoi(args[0]))};
			}
			std::vector <int> Read(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 3, "Posix.Read: Requires 3 arguments");
				env.Assert(YSL::Util::IsInteger(args[0]), "Posix.Read: Invalid FD");
				env.Assert(
					YSL::Util::IsInteger(args[2]), "Posix.Read: Count must be integer"
				);

				char*   buf = (char*) malloc(stoi(args[2]));
				ssize_t readRet = read(stoi(args[0]), &buf, stoi(args[2]));

				auto ret = YSL::Util::StringToIntVector(buf);
				free(buf);
				env.variables[args[1]] = ret;

				return {(int) readRet};
			}
			std::vector <int> Write(const std::vector <std::string>& args, Environment& env) {
				// TODO
				return {};
			}

			YSL::Extension BuildExtension() {
				YSL::Extension ext;

				ext.name                 = "posix";
				ext.functions["execv"]   = Execv;
				ext.functions["execvp"]  = Execvp;
				ext.functions["execvpe"] = Execvpe;
				ext.functions["fork"]    = Fork;
				ext.functions["wait"]    = Wait;
				
				return ext;
			}
		}
	}
}

#endif
