#ifndef YSL_YSL_HH
#define YSL_YSL_HH

#include <math.h>

#include <map>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>
#include <algorithm>

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

		bool StringStartsWith(std::string str, std::string prefix) {
			if (prefix.length() > str.length()) {
				return false;
			}
			return str.substr(0, prefix.length()) == prefix;
		}

		std::string StringReplace(std::string str, char from, char to) {
			std::string ret;
			for (auto& ch : str) {
				if (ch == from) {
					ret += to;
				}
				else {
					ret += ch;
				}
			}

			return ret;
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
		    	if ((str[i] == '"') && ((maxSplit < 0) || (splits < maxSplit - 1))) {
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

	#define YSL_FUNCTION(F) \
		std::vector <int> F(std::vector <std::string>, Environment&)

	class Extension {
		public:
			std::string                      name;
			std::map <std::string, Function> functions;
	};
	
	namespace STD {
		YSL_FUNCTION(Print);
		YSL_FUNCTION(PrintLn);
		YSL_FUNCTION(Exit);
		YSL_FUNCTION(Run);
		YSL_FUNCTION(Goto);
		YSL_FUNCTION(GotoIf);
		YSL_FUNCTION(Wait);
		YSL_FUNCTION(Cmp);
		YSL_FUNCTION(Var);
		YSL_FUNCTION(Load);
		YSL_FUNCTION(Size);
		YSL_FUNCTION(Getch);
		YSL_FUNCTION(Input);
		YSL_FUNCTION(Putch);
		YSL_FUNCTION(SetSize);
		YSL_FUNCTION(GoSub);
		YSL_FUNCTION(GoSubIf);
		YSL_FUNCTION(Return);
		YSL_FUNCTION(Debug);
		YSL_FUNCTION(Swap);
		YSL_FUNCTION(Gt);
		YSL_FUNCTION(Lt);
		YSL_FUNCTION(Pow);
	}

	class Environment {
		public:
			std::map <size_t, std::string>            program;
			std::map <std::string, Function>          builtins;
			std::map <size_t, std::string>::iterator  lineAt;
			bool                                      increment;
			std::map <std::string, std::vector <int>> variables;
			std::vector <std::vector <int>>           returnValues;
			std::vector <std::vector <int>>           passes;
			bool                                      yslDebug;
			bool                                      fromProgram; // running from program map

			std::vector <std::map <size_t, std::string>::iterator> calls;

			Environment():
				increment(true),
				yslDebug(false),
				fromProgram(false)
			{
				builtins["print"]    = STD::Print;
				builtins["println"]  = STD::PrintLn;
				builtins["exit"]     = STD::Exit;
				builtins["run"]      = STD::Run;
				builtins["goto"]     = STD::Goto;
				builtins["goto_if"]  = STD::GotoIf;
				builtins["wait"]     = STD::Wait;
				builtins["cmp"]      = STD::Cmp;
				builtins["var"]      = STD::Var;
				builtins["load"]     = STD::Load;
				builtins["size"]     = STD::Size;
				builtins["getch"]    = STD::Getch;
				builtins["input"]    = STD::Input;
				builtins["putch"]    = STD::Putch;
				builtins["set_size"] = STD::SetSize;
				builtins["gosub"]    = STD::GoSub;
				builtins["gosub_if"] = STD::GoSubIf;
				builtins["return"]   = STD::Return;
				builtins["debug"]    = STD::Debug;
				builtins["swap"]     = STD::Swap;
				builtins["gt"]       = STD::Gt;
				builtins["lt"]       = STD::Lt;
				builtins["pow"]      = STD::Pow;
			}

			void ExitError() {
				if (fromProgram) {
					fprintf(stderr, "Exited at line %i\n", (int) lineAt->first);
				}
				exit(1);
			}

			void Assert(bool b, std::string error) {
				if (!b) {
					fprintf(stderr, "%s\n", error.c_str());
					ExitError();
				}
			}

			void LoadExtension(const Extension& ext) {
				for (auto it = ext.functions.begin(); it != ext.functions.end(); ++it) {
					std::string functionName = ext.name + "." + it->first;
					builtins[functionName] = it->second;
				}
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
					else if (arg[0] == '!') {
						std::string value;
						for (auto& ch : variables[arg.substr(1)]) {
							value += ch;
						}
						ret.push_back(value);
					}
					else if (arg[0] == '&') {
						ret.push_back(std::to_string((int) arg[1]));
					}
					else if (arg[0] == '*') {
						bool found = false;
						for (auto it = program.begin(); it != program.end(); ++it) {
							if (Util::StringStartsWith(it->second, arg.substr(1) + ':')) {
								ret.push_back(std::to_string(it->first));
								found = true;
								break;
							}
						}

						if (!found) {
							fprintf(
								stderr, "Tried to access non-existant label: %s\n",
								arg.substr(1).c_str()
							);
							ExitError();
						}
					}
					else {
						ret.push_back(arg);
					}
				}

				return ret;
			}

			void Interpret(std::string code) {
				code = Util::StringReplace(code, '\t', ' ');
			
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
					if (code.back() == ':') {
						return;
					}
					
					if (builtins[parts[0]] == nullptr) {
						fprintf(stderr, "No such function: %s\n", parts[0].c_str());
						return;
					}
					auto args = AddVariables(
						std::vector <std::string> (parts.begin() + 1, parts.end())
					);

					std::vector <int> ret;
					if (yslDebug) {
						ret = builtins[parts[0]](args, *this);
					}
					else {
						try {
							ret = builtins[parts[0]](args, *this);
						}
						catch (const std::exception& e) {
							fprintf(
								stderr, "Crashed at line %i\n", (int) lineAt->first
							);
							fprintf(stderr, "Error: %s\n", e.what());
							exit(1);
						}
					}
					if (!ret.empty()) {
						returnValues.push_back(ret);
					}
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
		std::vector <int> PrintLn(std::vector <std::string> args, Environment& env) {
			Print(args, env);
			puts("");
			return {};
		}
		std::vector <int> Exit(std::vector <std::string>, Environment&) {
			exit(0);
			return {};
		}
		std::vector <int> Run(std::vector <std::string>, Environment& env) {
			env.fromProgram = true;
			for (env.lineAt = env.program.begin(); env.lineAt != env.program.end();) {
				env.Interpret(env.lineAt->second);

				if (env.increment) {
					++ env.lineAt;
				}
				env.increment = true;
			}
			env.fromProgram = false;
			return {};
		}
		std::vector <int> Goto(std::vector <std::string> args, Environment& env) {
			if (args.size() == 0) {
				fprintf(stderr, "Goto: need at least 1 argument\n");
				env.ExitError();
			}
			if (!Util::IsInteger(args[0])) {
				fprintf(stderr, "Goto: invalid argument %s\n", args[0].c_str());
				env.ExitError();
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
				env.ExitError();
			}

			if (!env.returnValues.empty() && (env.returnValues.back()[0] != 0)) {
				return Goto(args, env);
			}
			return {};
		}
		std::vector <int> GoSub(std::vector <std::string> args, Environment& env) {
			env.calls.push_back(env.lineAt);

			for (size_t i = 1; i < args.size(); ++i) {
				if (Util::IsInteger(args[i])) {
					env.passes.push_back({stoi(args[i])});
				}
				else {
					std::vector <int> str;
					for (auto& ch : args[i]) {
						str.push_back(ch);
					}
					env.passes.push_back(str);
				}
			}
			
			return Goto(args, env);
		}
		std::vector <int> GoSubIf(std::vector <std::string> args, Environment& env) {
			env.calls.push_back(env.lineAt);

			for (size_t i = 1; i < args.size(); ++i) {
				if (Util::IsInteger(args[i])) {
					env.passes.push_back({stoi(args[i])});
				}
				else {
					std::vector <int> str;
					for (auto& ch : args[i]) {
						str.push_back(ch);
					}
					env.passes.push_back({stoi(args[i])});
				}
			}
			
			return GotoIf(args, env);
		}
		std::vector <int> Wait(std::vector <std::string> args, Environment& env) {
			if (args.size() == 0) {
				fprintf(stderr, "Wait: needs at least 1 argument\n");
				env.ExitError();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(stol(args[0])));
			return {};
		}
		std::vector <int> Cmp(std::vector <std::string> args, Environment& env) {
			if (args.size() < 2) {
				fprintf(stderr, "Cmp: needs at least 2 arguments\n");
				env.ExitError();
			}

			return {args[0] == args[1]? 1 : 0};
		}
		std::vector <int> Var(std::vector <std::string> args, Environment& env) {
			if ((args.size() < 3) && !((args.size() > 1) && (args[1] == "p"))) {
				fprintf(stderr, "Var: needs at least 3 arguments\n");
				env.ExitError();
			}

			if (
				env.variables[args[0]].empty() &&
				(args[1] != "=") && (args[1] != "f") && (args[1] != "c") &&
				(args[1] != "p")
			) {
				fprintf(stderr, "Var: no such variable %s\n", args[0].c_str());
				env.ExitError();
			}

			switch (args[1][0]) {
				case '=': {
					if (Util::IsInteger(args[2])) {
						std::vector <int> value;
						for (size_t i = 2; i < args.size(); ++i) {
							value.push_back(stoi(args[i]));
						}
						env.variables[args[0]] = value;
					}
					else {
						std::vector <int> value;
						for (auto& ch : args[2]) {
							value.push_back(ch);
						}
						env.variables[args[0]] = value;
					}
					break;
				}
				case '+': {
					env.variables[args[0]][0] += stoi(args[2]);
					break;
				}
				case '-': {
					env.variables[args[0]][0] -= stoi(args[2]);
					break;
				}
				case '*': {
					env.variables[args[0]][0] *= stoi(args[2]);
					break;
				}
				case '/': {
					env.variables[args[0]][0] /= stoi(args[2]);
					break;
				}
				case '%': {
					env.variables[args[0]][0] %= stoi(args[2]);
					break;
				}
				case 'f': {
					if (args.size() < 3) {
						fprintf(stderr, "Var: from operator needs 3 arguments\n");
						env.ExitError();
					}

					if (env.variables[args[2]].empty() && (args[2] != "return")) {
						fprintf(stderr, "Var: no such variable %s\n", args[2].c_str());
						env.ExitError();
					}
					size_t index = args.size() == 4? stol(args[3]) : 0;
					auto   arr = args[2] == "return"?
						std::vector <int> {env.returnValues.back()} :
						env.variables[args[2]];

					if (index >= arr.size()) {
						fprintf(
							stderr,
							"Var: out of range for index %i of array of size %i\n",
							(int) index, (int) arr.size()
						);
						env.ExitError();
					}

					env.variables[args[0]] = {arr[index]};
					break;
				}
				case 'c': {
					auto arr = args[2] == "return"?
						env.returnValues.back() : env.variables[args[2]];
					env.variables[args[0]] = arr;
					break;
				}
				case 'p': {
					if (env.passes.empty()) {
						fprintf(stderr, "Var: p: nothing to pop from pass stack\n");
						env.ExitError();
					}

					env.variables[args[0]] = env.passes.back();
					env.passes.pop_back();
					break;
				}
				case 'a': {
					env.variables[args[0]].push_back(stoi(args[2]));
					break;
				}
				case 'r': {
					if (args.size() < 4) {
						fprintf(stderr, "remove operator needs 4 arguments\n");
						env.ExitError();
					}
				
					auto& arr = env.variables[args[0]];
					arr.erase(
						arr.begin() + stoi(args[2]),
						arr.begin() + stoi(args[2]) + stoi(args[3])
					);
					break;
				}
				case 's': {
					if (args.size() < 4) {
						fprintf(stderr, "set operator needs 4 arguments\n");
						env.ExitError();
					}

					auto& arr = env.variables[args[0]];
					arr[stol(args[2])] = stoi(args[3]);
					break;
				}
				default: {
					fprintf(stderr, "Var: unknown operation %c\n", args[1][0]);
					env.ExitError();
				}
			}

			return {};
		}
		std::vector <int> Load(std::vector <std::string> args, Environment& env) {
			if (args.empty()) {
				fprintf(stderr, "Load: needs at least 1 argument\n");
				env.ExitError();
			}

			env.program.clear();
			std::ifstream             fhnd(args[0]);
			std::string               line;
			size_t                    lineNum = 10;

			while (getline(fhnd, line)) {
				env.program[lineNum] =  line;
				lineNum              += 10;
			}
			fhnd.close();
			return {};
		}
		std::vector <int> Size(std::vector <std::string> args, Environment& env) {
			if (args.empty()) {
				fprintf(stderr, "Size: needs at least 1 argument\n");
				env.ExitError();
			}

			return {(int) env.variables[args[0]].size()};
		}
		std::vector <int> Getch(std::vector <std::string>, Environment&) {
			return {getchar()};
		}
		std::vector <int> Input(std::vector <std::string>, Environment&) {
			std::string input;
			std::getline(std::cin, input, '\n');

			std::vector <int> ret;
			for (auto& ch : input) {
				ret.push_back(ch);
			}

			return ret;
		}
		std::vector <int> Putch(std::vector <std::string> args, Environment& env) {
			if (args.empty()) {
				fprintf(stderr, "Putch: needs at least 1 argument\n");
				env.ExitError();
			}

			putchar(stoi(args[0]));
			return {};
		}
		std::vector <int> SetSize(std::vector <std::string> args, Environment& env) {
			if (args.size() < 2) {
				fprintf(stderr, "SetSize: needs at least 2 arguments\n");
				env.ExitError();
			}

			auto& arr = env.variables[args[0]];
			arr.resize(stoi(args[1]));
			return {};
		}
		std::vector <int> Return(std::vector <std::string> args, Environment& env) {
			if (args.size() < 1) {
				
			}
			else if (Util::IsInteger(args[0])) {
				env.returnValues.push_back({stoi(args[0])});
			}
			else {
				env.returnValues.push_back(env.variables[args[0]]); // from variable
			}

			env.lineAt    = env.calls.back();
			env.calls.pop_back();
			return {};
		}
		std::vector <int> Debug(std::vector <std::string>, Environment& env) {
			env.yslDebug = !env.yslDebug;
			puts(std::to_string(env.yslDebug).c_str());
			return {};
		}
		std::vector <int> Swap(std::vector <std::string> args, Environment& env) {
			if (args.size() < 2) {
				fprintf(stderr, "Swap: needs at least 2 arguments\n");
				env.ExitError();
			}
			std::swap(env.variables[args[0]], env.variables[args[1]]);
			return {};
		}
		std::vector <int> Gt(std::vector <std::string> args, Environment& env) {
			if (args.size() < 2) {
				fprintf(stderr, "Gt: needs at least 2 arguments\n");
				env.ExitError();
			}

			return {
				(stoi(args[0]) > stoi(args[1]))? 1 : 0
			};
		}
		std::vector <int> Lt(std::vector <std::string> args, Environment& env) {
			if (args.size() < 2) {
				fprintf(stderr, "Gt: needs at least 2 arguments\n");
				env.ExitError();
			}

			env.Assert(
				!Util::IsInteger(args[0]) || !Util::IsInteger(args[1]),
				"Pow: needs integer arguments"
			);

			return {
				(stoi(args[0]) < stoi(args[1]))? 1 : 0
			};
		}
		std::vector <int> Pow(std::vector <std::string> args, Environment& env) {
			env.Assert(args.size() == 2, "Pow: needs 2 arguments");
			env.Assert(
				Util::IsInteger(args[0]) && Util::IsInteger(args[1]),
				"Pow: needs integer arguments"
			);

			return {(int) pow((double) stoi(args[0]), (double) stoi(args[1]))};
		}
	}
}

#endif
