#ifndef YSL_YSL_HH
#define YSL_YSL_HH

#include <map>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

namespace YSL {
	namespace Util {
		bool IsInteger(std::string str) {
		    for (auto& ch : str) {
		        if (!isdigit(ch)) {
		            return false;
		        }
		    }
		    return true;
		}

		std::vector <std::string> SplitString(
		    std::string str, char splitter, ssize_t maxSplit
		) {
		    ssize_t                   splits = 0;
		    std::string               reading;
		    std::vector <std::string> ret;
		    bool                      inString = false;
		    
		    for (size_t i = 0; i <= str.length(); ++i) {
		    	if (inString && str[i] == '\\') {
		    		++ i;
		    		switch (str[i]) {
		    			case 'n': {
		    				reading += '\n';
		    				break;
		    			}
		    			case 'r': {
		    				reading += '\r';
		    				break;
		    			}
		    			case 'e': {
		    				reading += '\x1b';
		    				break;
		    			}
		    			case '"': {
		    				reading += '\"';
		    				break;
		    			}
		    			case '\\': {
		    				reading += '\\';
		    			}
		    			default: {
		    				reading += std::string("\\") + str[i];
		    				break;
		    			}
		    		}
		    		continue;
		    	}
		    	if (str[i] == '"') {
		    		inString = !inString;
		    		continue;
		    	}
		    	
		        if (
		        	(!inString && (str[i] == splitter)) ||
		        	(str[i] == '\0')
		        ) {
		            if ((maxSplit < 0) || (splits < maxSplit - 1) || (str[i] == '\0')) {
		                if (reading == "") {
		                    continue;
		                }
		                ret.push_back(reading);
		                reading = "";
		                ++ splits;
		                continue;
		            }
		            reading += splitter;
		            continue;
		        }
		        reading += str[i];
		    }

		    return ret;
		}
	}

	class Environment;
	typedef std::vector <int> (*Function)(std::vector <std::string>, Environment&);

	namespace STD {
		std::vector <int> Print(std::vector <std::string>,  Environment&);
		std::vector <int> Exit(std::vector <std::string>,   Environment&);
		std::vector <int> Run(std::vector <std::string>,    Environment&);
		std::vector <int> Goto(std::vector <std::string>,   Environment&);
		std::vector <int> GotoIf(std::vector <std::string>, Environment&);
		std::vector <int> Wait(std::vector <std::string>,   Environment&);
		std::vector <int> Cmp(std::vector <std::string>,    Environment&);
		std::vector <int> Var(std::vector <std::string>,    Environment&);
	}

	class Environment {
		public:
			std::map <size_t, std::string>            program;
			std::map <std::string, Function>          builtins;
			std::map <size_t, std::string>::iterator  lineAt;
			bool                                      increment;
			std::map <std::string, std::vector <int>> variables;
			std::vector <std::vector <int>>           returnValues;


			Environment():
				increment(true)
			{
				builtins["print"]   = STD::Print;
				builtins["exit"]    = STD::Exit;
				builtins["run"]     = STD::Run;
				builtins["goto"]    = STD::Goto;
				builtins["goto_if"] = STD::GotoIf;
				builtins["wait"]    = STD::Wait;
				builtins["cmp"]     = STD::Cmp;
				builtins["var"]     = STD::Var;
			}

			std::vector <std::string> AddVariables(std::vector <std::string> args) {
				std::vector <std::string> ret;
				for (auto& arg : args) {
					if (arg[0] == '$') {
						if (variables[arg.substr(1)].empty()) {
							fprintf(stderr, "No such variable: %s\n", arg.substr(1).c_str());
							exit(1);
						}

						ret.push_back(std::to_string(variables[arg.substr(1)][0]));
					}
					else {
						ret.push_back(arg);
					}
				}

				return ret;
			}

			void Interpret(std::string code) {
				if (code[0] == '#') {
					return; // comment
				}
			
				auto parts = Util::SplitString(code, ' ', -1);
				if (parts.empty()) {
					return;
				}
				if (Util::IsInteger(parts[0])) {
					if (parts.size() == 1) {
						program.erase(stoi(parts[0]));
					}
					else {
						program[stol(parts[0])] = Util::SplitString(code, ' ', 2)[1];
					}
				}
				else {
					if (builtins[parts[0]] == nullptr) {
						fprintf(stderr, "No such function: %s\n", parts[0].c_str());
						return;
					}
					auto args = AddVariables(
						std::vector <std::string> (parts.begin() + 1, parts.end())
					);
					
					returnValues.push_back(builtins[parts[0]](args, *this));
				}
			}
	};
	
	namespace STD {
		std::vector <int> Print(std::vector <std::string> args, Environment&) {
			for (size_t i = 0; i < args.size(); ++i) {
				printf("%s%s", args[i].c_str(), i == args.size() - 1? "" : " ");
			}
			return {};
		}
		std::vector <int> Exit(std::vector <std::string>, Environment&) {
			exit(0);
			return {};
		}
		std::vector <int> Run(std::vector <std::string>, Environment& env) {
			for (env.lineAt = env.program.begin(); env.lineAt != env.program.end();) {
				env.Interpret(env.lineAt->second);

				if (env.increment) {
					++ env.lineAt;
				}
				env.increment = true;
			}
			return {};
		}
		std::vector <int> Goto(std::vector <std::string> args, Environment& env) {
			if (args.size() == 0) {
				fprintf(stderr, "Goto: need at least 1 argument\n");
				exit(1);
			}

			size_t to = stol(args[0]);
			for (auto it = env.program.begin(); it != env.program.end(); ++it) {
				if (it->first == to) {
					env.lineAt    = it;
					env.increment = false;
					return {};
				}
			}
			fprintf(stderr, "Goto: can't jump to %s\n", args[0].c_str());
			exit(1);
			return {};
		}
		std::vector <int> GotoIf(std::vector <std::string> args, Environment& env) {
			if (args.size() == 0) {
				fprintf(stderr, "Goto_if: needs at least 1 argument\n");
				exit(1);
			}

			if (!env.returnValues.empty() && (env.returnValues.back()[0] != 0)) {
				return Goto(args, env);
			}
			return {};
		}
		std::vector <int> Wait(std::vector <std::string> args, Environment&) {
			if (args.size() == 0) {
				fprintf(stderr, "Wait: needs at least 1 argument\n");
				exit(1);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(stol(args[0])));
			return {};
		}
		std::vector <int> Cmp(std::vector <std::string> args, Environment&) {
			if (args.size() < 2) {
				fprintf(stderr, "Cmp: needs at least 2 arguments\n");
				exit(1);
			}

			return {args[0] == args[1]? 1 : 0};
		}
		std::vector <int> Var(std::vector <std::string> args, Environment& env) {
			if (args.size() < 3) {
				fprintf(stderr, "Var: needs at least 3 arguments\n");
				exit(1);
			}

			switch (args[1][0]) {
				case '=': {
					std::vector <int> value;
					for (size_t i = 2; i < args.size(); ++i) {
						value.push_back(stoi(args[i]));
					}
					env.variables[args[0]] = value;
					break;
				}
				default: {
					fprintf(stderr, "Var: unknown operation %c\n", args[1][0]);
					exit(1);
				}
			}

			return {};
		}
	}
}

#endif
