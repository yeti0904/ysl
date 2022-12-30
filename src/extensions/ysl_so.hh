#ifndef YSL_EXTENSION_SO_HH
#define YSL_EXTENSION_SO_HH

#include "../environment.hh"
#include <dlfcn.h>

#ifdef YSL_PLATFORM_WINDOWS
	#error "Posix extension is being used on windows"
#endif

class LoadedLibrary {
	public:
		std::string name;
		void*       handle;

		LoadedLibrary():
			handle(nullptr)
		{}
		~LoadedLibrary() {
			if (handle) {
				//dlclose(handle);
				//handle = nullptr;
			}
		}
};

namespace YSL {
	namespace Extensions {
		namespace So {
			typedef void (*LoadedFunction)(
				const std::vector <std::string>& args, Environment* env
			);
			static std::vector <LoadedLibrary> libraries;
			std::vector <int> Load(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() == 1, "So.Load: Requires 1 argument");
			
				LoadedLibrary newLib;
				newLib.name = args[0];
				newLib.handle = dlopen(args[0].c_str(), RTLD_LAZY);
				
				if (!newLib.handle) {
					fprintf(stderr, "So.Load: %s\n", dlerror());
					env.ExitError();
				}
				dlerror();

				libraries.push_back(newLib);

				return {};
			}

			std::vector <int> Call(const std::vector <std::string>& args, Environment& env) {
				env.Assert(args.size() >= 2, "So.Call: Requires at least 2 arguments");
				for (auto& lib : libraries) {
					if (lib.name == args[0]) {
						LoadedFunction func =
							(LoadedFunction) dlsym(lib.handle, args[1].c_str());

						char* err = dlerror();
						if (err) {
							fprintf(stderr, "So.Call: %s\n", dlerror());
							env.ExitError();
						}
						if (!func) {
							fprintf(stderr, "So.Call: function is NULL\n");
							env.ExitError();
						}

						auto funcArgs = args;
						funcArgs.erase(funcArgs.begin(), funcArgs.begin() + 2);

						func(funcArgs, &env);
						return {};
					}
				}
				fprintf(stderr, "So.Call: %s not loaded\n", args[0].c_str());
				env.ExitError();
				return {};
			}
		
			Extension BuildExtension() {
				Extension ext;

				ext.name              = "so";
				ext.functions["load"] = Load;
				ext.functions["call"] = Call;

				return ext;
			}
		}
	}
}

#endif
