#ifndef YSL_EXTENSION_FILE_HH
#define YSL_EXTENSION_FILE_HH

#include <fstream>
#include "../environment.hh"

namespace YSL {
	namespace Extensions {
		namespace File {
			std::vector <int> Read(const std::vector <std::string>& args, Environment& env) {
				if (args.size() < 1) {
					fprintf(stderr, "File.Read: Needs at least 1 argument\n");
					env.ExitError();
				}
			
				std::vector <int> ret;
				std::ifstream fhnd(args[0]);

				if (!fhnd.good()) {
					fprintf(
						stderr, "File.Read: Failed to open file '%s'\n",
						args[0].c_str()
					);
					env.ExitError();
				}

				std::string line;

				while (getline(fhnd, line)) {
					for (auto& ch : line) {
						ret.push_back(ch);
					}
					ret.push_back(10); // new line
				}
				ret.erase(ret.begin() + (ret.size() - 1));

				return ret;
			}
			std::vector <int> Write(const std::vector <std::string>& args, Environment& env) {
				if (args.size() < 2) {
					fprintf(stderr, "File.Write: Needs at least 2 arguments\n");
					env.ExitError();
				}

				std::ofstream fhnd(args[0]);
				fhnd << args[1];

				return {};
			}
			std::vector <int> Flush(const std::vector <std::string>&, Environment&) {
				fflush(stdout);
				return {};
			}

			Extension BuildExtension() {
				Extension ext;

				ext.name               = "file";
				ext.functions["read"]  = Read;
				ext.functions["write"] = Write;
				ext.functions["flush"] = Flush;

				return ext;
			}
		}
	}
}

#endif
