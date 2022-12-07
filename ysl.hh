#ifndef YSL_YSL_HH
#define YSL_YSL_HH

#include <string.h>
#include <math.h>

#include <map>
#include <regex>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>
#include <algorithm>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#define YSL_PLATFORM_WINDOWS
#elif defined(__APPLE__)
	#define YSL_PLATFORM_APPLE
#elif defined(__linux__)
	#define YSL_PLATFORM_LINUX
#elif defined(__unix__)
	#define YSL_PLATFORM_UNIX
#else
	#define YSL_PLATFORM_UNKNOWN
#endif

namespace YSL {
	namespace Util {
		bool IsInteger(std::string str) {
		    try {
		    	stoi(str);
		    }
		    catch (...) {
		    	return false;
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

		std::string TrimString(std::string str) {
			// an improved version of this https://stackoverflow.com/a/58773060
			return std::regex_replace(str, std::regex("^ +| +$"),"");
		}

		std::vector <int> StringToIntVector(std::string str) {
			std::vector <int> ret;

			for (auto& ch : str) {
				ret.push_back(ch);
			}

			return ret;
		}
		std::vector <int> StringVectorToIntVector(std::vector <std::string> vec) {
			std::vector <int> ret;
		
			for (auto& str : vec) {
				for (auto& ch : str) {
					ret.push_back(ch);
				}
				ret.push_back(0);
			}

			return ret;
		}
		std::vector <std::string> IntVectorToStringVector(std::vector <int> vec) {
			size_t                    index = 0;
			std::string               reading;
			std::vector <std::string> ret;

			for (auto& ch : vec) {
				switch (ch) {
					case '\0': {
						++ index;
						ret.push_back(reading);
						reading = "";
						break;
					}
					default: {
						reading += ch;
						break;
					}
				}
			}

			return ret;
		}
		std::vector <char*> StringVectorToCharpVector(std::vector <std::string> vec) {
			std::vector <char*> ret;

			for (auto& str : vec) {
				ret.push_back(strdup(str.c_str()));
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
		    bool                      lastWasString = false;
		    
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
		    		if (!inString) {
		    			lastWasString = true;
		    		}
		    		continue;
		    	}
		    	
		        if (
		        	(!inString && (str[i] == splitter)) ||
		        	(str[i] == '\0')
		        ) {
		            if ((maxSplit < 0) || (splits < maxSplit - 1) || (str[i] == '\0')) {
		                if ((reading == "") && !lastWasString) {
		                    continue;
		                }
		                ret.push_back(reading);
		                lastWasString = false;
		                reading       = "";
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
	typedef std::vector <int> (*Function)(const std::vector <std::string>&, Environment&);

	#define YSL_FUNCTION(F) \
		std::vector <int> F(const std::vector <std::string>&, Environment&)

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
		YSL_FUNCTION(StringArray);
		YSL_FUNCTION(Not);
		YSL_FUNCTION(And);
		YSL_FUNCTION(Split);
		YSL_FUNCTION(Or);
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
				builtins["print"]        = STD::Print;
				builtins["println"]      = STD::PrintLn;
				builtins["exit"]         = STD::Exit;
				builtins["run"]          = STD::Run;
				builtins["goto"]         = STD::Goto;
				builtins["goto_if"]      = STD::GotoIf;
				builtins["wait"]         = STD::Wait;
				builtins["cmp"]          = STD::Cmp;
				builtins["var"]          = STD::Var;
				builtins["load"]         = STD::Load;
				builtins["size"]         = STD::Size;
				builtins["getch"]        = STD::Getch;
				builtins["input"]        = STD::Input;
				builtins["putch"]        = STD::Putch;
				builtins["set_size"]     = STD::SetSize;
				builtins["gosub"]        = STD::GoSub;
				builtins["gosub_if"]     = STD::GoSubIf;
				builtins["return"]       = STD::Return;
				builtins["debug"]        = STD::Debug;
				builtins["swap"]         = STD::Swap;
				builtins["gt"]           = STD::Gt;
				builtins["lt"]           = STD::Lt;
				builtins["pow"]          = STD::Pow;
				builtins["string_array"] = STD::StringArray;
				builtins["not"]          = STD::Not;
				builtins["and"]          = STD::And;
				builtins["split"]        = STD::Split;
				builtins["or"]           = STD::Or;

				#ifdef YSL_PLATFORM_WINDOWS
					variables["__platform"] = {1};
				#elif defined(YSL_PLATFORM_APPLE)
					variables["__platform"] = {2};
				#elif defined(YSL_PLATFORM_LINUX)
					variables["__platform"] = {3};
				#elif defined(YSL_PLATFORM_UNIX)
					variables["__platform"] = {4};
				#elif defined(YSL_PLATFORM_UNKNOWN)
					variables["__platform"] = {0};
				#endif
				variables["__platform_windows"] = {1};
				variables["__platform_apple"]   = {2};
				variables["__platform_linux"]   = {3};
				variables["__platform_unix"]    = {4};
				variables["__platform_unknown"] = {5};
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

			void AddFunction(std::string name, Function function) {
				builtins[name] = function;
			}

			void SetVariable(std::string name, std::vector <int> value) {
				variables[name] = value;
			}

			void LoadExtension(const Extension& ext) {
				for (auto it = ext.functions.begin(); it != ext.functions.end(); ++it) {
					std::string functionName = ext.name + "." + it->first;
					builtins[functionName] = it->second;
				}
			}

			std::vector <std::string> AddVariables(const std::vector <std::string>& args) {
				std::vector <std::string> ret;
				for (auto& arg : args) {
					if (arg[0] == '$') {
						if (variables[arg.substr(1)].empty()) {
							fprintf(stderr, "Empty variable: %s\n", arg.substr(1).c_str());
							ExitError();
						}

						ret.push_back(std::to_string(variables[arg.substr(1)][0]));
					}
					else if (arg[0] == '!') {
						/*if (variables[arg.substr(1)].empty()) {
							fprintf(stderr, "Empty variable: %s\n", arg.substr(1).c_str());
							ExitError();
						}*/
					
						std::string value;
						for (auto& ch : variables[arg.substr(1)]) {
							value += ch;
						}
						ret.push_back(value);
					}
					else if (arg[0] == '&') {
						ret.push_back(std::to_string((int) arg[1]));
					}
					else if (arg[0] == '\\') {
						ret.push_back(arg.substr(1));
					}
					else if (arg[0] == '*') {
						if (arg.length() == 1) {
							ret.push_back(arg);
							continue;
						}
					
						bool found = false;
						for (auto it = program.begin(); it != program.end(); ++it) {
							if (
								Util::StringStartsWith(
									Util::TrimString(it->second), arg.substr(1) + ':'
								)
							) {
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
			
				if (Util::TrimString(code)[0] == '#') {
					return; // comment
				}
				auto parts = Util::SplitString(code, ' ', -1);
				if (parts.empty()) {
					return;
				}
				parts[0] = AddVariables(std::vector <std::string>{parts[0]})[0];
				
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
		std::vector <int> Print(const std::vector <std::string>& args, Environment&) {
			for (size_t i = 0; i < args.size(); ++i) {
				printf("%s%s", args[i].c_str(), i == args.size() - 1? "" : " ");
			}
			return {};
		}
		std::vector <int> PrintLn(const std::vector <std::string>& args, Environment& env) {
			Print(args, env);
			puts("");
			return {};
		}
		std::vector <int> Exit(const std::vector <std::string>&, Environment&) {
			exit(0);
			return {};
		}
		std::vector <int> Run(const std::vector <std::string>&, Environment& env) {
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
		std::vector <int> Goto(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 1, "Goto: needs 1 argument");
			env.Assert(Util::IsInteger(args[0]), "Goto: argument must be integer");

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
		std::vector <int> GotoIf(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 1, "GotoIf: needs 1 argument");

			if (!env.returnValues.empty() && (env.returnValues.back()[0] != 0)) {
				return Goto(args, env);
			}
			return {};
		}
		std::vector <int> GoSub(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() >= 1, "GoSub: Needs at least 1 argument");
		
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
		std::vector <int> GoSubIf(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() >= 1, "GoSubIf: Needs at least 1 argument");
		
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
		std::vector <int> Wait(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 1, "Wait: needs 1 argument");
			env.Assert(Util::IsInteger(args[0]), "Wait: argument must be integer");

			std::this_thread::sleep_for(std::chrono::milliseconds(stol(args[0])));
			return {};
		}
		std::vector <int> Cmp(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 2, "Cmp: needs 2 arguments");

			return {args[0] == args[1]? 1 : 0};
		}
		std::vector <int> Var(const std::vector <std::string>& args, Environment& env) {
			if ((args.size() < 3) && !((args.size() > 1) && (args[1] == "p"))) {
				env.Assert(false, "Var: needs at least 3 arguments");
			}
			
			/*if (
				env.variables[args[0]].empty() &&
				(args[1] != "=") && (args[1] != "f") && (args[1] != "c") &&
				(args[1] != "p")
			) {
				fprintf(stderr, "Var: no such variable %s\n", args[0].c_str());
				env.ExitError();
			}*/

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
					env.Assert(
						Util::IsInteger(args[2]), "Var: argument 3 must be an integer"
					);
				
					env.variables[args[0]][0] += stoi(args[2]);
					break;
				}
				case '-': {
					env.Assert(
						Util::IsInteger(args[2]), "Var: argument 3 must be an integer"
					);
				
					env.variables[args[0]][0] -= stoi(args[2]);
					break;
				}
				case '*': {
					env.Assert(
						Util::IsInteger(args[2]), "Var: argument 3 must be an integer"
					);
				
					env.variables[args[0]][0] *= stoi(args[2]);
					break;
				}
				case '/': {
					env.Assert(
						Util::IsInteger(args[2]), "Var: argument 3 must be an integer"
					);
				
					env.variables[args[0]][0] /= stoi(args[2]);
					break;
				}
				case '%': {
					env.Assert(
						Util::IsInteger(args[2]), "Var: argument 3 must be an integer"
					);
				
					env.variables[args[0]][0] %= stoi(args[2]);
					break;
				}
				case 'f': {
					if (args.size() < 3) {
						fprintf(stderr, "Var: from operator needs 3 arguments\n");
						env.ExitError();
					}

					/*if (env.variables[args[2]].empty() && (args[2] != "return")) {
						fprintf(stderr, "Var: no such variable %s\n", args[2].c_str());
						env.ExitError();
					}*/
					size_t index = args.size() == 4? stol(args[3]) : 0;
					auto   arr = args[2] == "return"?
						std::vector <int> {env.returnValues.back()} :
						env.variables[args[2]];

					if (args[2] == "return") {
						env.returnValues.pop_back();
					}

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

					if (args[2] == "return") {
						env.returnValues.pop_back();
					}
					
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
					
					env.Assert(
						Util::IsInteger(args[2]) && Util::IsInteger(args[3]),
						"Var: argument 3 and 4 must be an integer"
					);
				
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
		std::vector <int> Load(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 1, "Load: needs 1 argument");

			env.program.clear();
			std::ifstream fhnd(args[0]);
			std::string   line;
			size_t        lineNum = 10;

			while (getline(fhnd, line)) {
				env.program[lineNum] =  line;
				lineNum              += 10;
			}
			fhnd.close();
			return {};
		}
		std::vector <int> Size(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 1, "Size: needs 1 argument");

			return {(int) env.variables[args[0]].size()};
		}
		std::vector <int> Getch(const std::vector <std::string>&, Environment&) {
			return {getchar()};
		}
		std::vector <int> Input(const std::vector <std::string>&, Environment&) {
			std::string input;
			std::getline(std::cin, input, '\n');

			std::vector <int> ret;
			for (auto& ch : input) {
				ret.push_back(ch);
			}

			return ret;
		}
		std::vector <int> Putch(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 1, "Putch: needs 1 argument");
			env.Assert(Util::IsInteger(args[0]), "Putch: requires integer argument");

			putchar(stoi(args[0]));
			return {};
		}
		std::vector <int> SetSize(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 2, "SetSize: needs 2 arguments");

			auto& arr = env.variables[args[0]];
			arr.resize(stoi(args[1]));
			return {};
		}
		std::vector <int> Return(const std::vector <std::string>& args, Environment& env) {
			env.Assert(!env.calls.empty(), "Return: nowhere to return to");
		
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
		std::vector <int> Debug(const std::vector <std::string>&, Environment& env) {
			env.yslDebug = !env.yslDebug;
			puts(std::to_string(env.yslDebug).c_str());
			return {};
		}
		std::vector <int> Swap(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 2, "Swap: needs 2 arguments");
			
			std::swap(env.variables[args[0]], env.variables[args[1]]);
			return {};
		}
		std::vector <int> Gt(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 2, "Gt: needs at least 2 arguments");
			env.Assert(
				Util::IsInteger(args[0]) && Util::IsInteger(args[1]),
				"Gt: needs integer arguments"
			);

			return {
				(stoi(args[0]) > stoi(args[1]))? 1 : 0
			};
		}
		std::vector <int> Lt(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 2, "Gt: needs 2 arguments");
			env.Assert(
				Util::IsInteger(args[0]) && Util::IsInteger(args[1]),
				"Lt: needs integer arguments"
			);

			return {
				(stoi(args[0]) < stoi(args[1]))? 1 : 0
			};
		}
		std::vector <int> Pow(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() == 2, "Pow: needs 2 arguments");
			env.Assert(
				Util::IsInteger(args[0]) && Util::IsInteger(args[1]),
				"Pow: needs integer arguments"
			);

			return {(int) pow((double) stoi(args[0]), (double) stoi(args[1]))};
		}
		std::vector <int> StringArray(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() >= 2, "StringArray: Needs at least 2 arguments");
		
			switch (args[0][0]) {
				case 'n': {
					std::vector <std::string> newArray;
					for (size_t i = 1; i < args.size(); ++i) {
						newArray.push_back(args[i]);
					}
					return Util::StringVectorToIntVector(newArray);
					break;
				}
				case 'g': {
					env.Assert(
						args.size() == 3, "StringArray: g operator needs 3 arguments"
					);
					env.Assert(
						Util::IsInteger(args[2]), "StringArray: Index must be integer"
					);
					auto array = Util::IntVectorToStringVector(env.variables[args[1]]);
					auto index = stoi(args[2]);

					env.Assert(
						(index >= 0) && ((size_t) index < array.size()),
						"StringArray: Index out of range for array of size " +
						std::to_string(array.size())
					);

					return Util::StringToIntVector(array[index]);
					break;
				}
				case 'l': {
					auto array = Util::IntVectorToStringVector(env.variables[args[1]]);

					return {(int) array.size()};
					break;
				}
				case 'a': {
					env.Assert(
						args.size() == 3, "StringArray: a operator needs 3 arguments"
					);
					for (auto& ch : args[2]) {
						env.variables[args[1]].push_back(ch);
					}
					env.variables[args[1]].push_back(0);
					break;
				}
				case 's': {
					env.Assert(
						args.size() == 4, "StringArray: s operator needs 4 arguments"
					);
					env.Assert(
						Util::IsInteger(args[2]), "StringArray: index must be integer"
					);

					auto array = Util::IntVectorToStringVector(env.variables[args[1]]);
					array[stol(args[2])] = args[3];
					env.variables[args[1]] = Util::StringVectorToIntVector(
						array
					);

					break;
				}
				case 'r': {
					env.Assert(
						args.size() == 2, "StringArray: s operator needs 3 arguments"
					);
					env.Assert(
						Util::IsInteger(args[2]), "StringArray: index must be integer"
					);

					auto array = Util::IntVectorToStringVector(env.variables[args[1]]);
					array.erase(array.begin() + stoi(args[2]));
					env.variables[args[1]] = Util::StringVectorToIntVector(
						array
					);

					break;
				}
				default: {
					env.Assert(false, "StringArray: Unknown operator " + args[0]);
					break;
				}
			}

			return {};
		}
		std::vector <int> Split(const std::vector <std::string>& args, Environment& env) {
			env.Assert(args.size() >= 2, "Split: Needs at least 2 arguments");

			std::vector <std::string> splitted;
			if (args.size() >= 2) {
				env.Assert(Util::IsInteger(args[2]), "Split: max split must be integer");

				splitted = Util::SplitString(args[0], args[1][0], stoi(args[2]));
			}
			else {
				splitted = Util::SplitString(args[0], args[1][0], -1);
			}

			return Util::StringVectorToIntVector(splitted);
		}
		std::vector <int> Not(const std::vector <std::string>& args, Environment& env) {
			if (args.empty()) {
				auto ret = env.returnValues.back();
				env.returnValues.pop_back();
				return std::vector <int>{ret[0] == 0? 1 : 0};
			}
			else {
				env.Assert(
					args.size() == 1, "Not: Needs either no arguments or 1 argument"
				);
				env.Assert(Util::IsInteger(args[0]), "Not: Needs integer argument");

				return std::vector <int>{stoi(args[0]) == 0? 1 : 0};
			}
		}
		std::vector <int> And(const std::vector <std::string>& args, Environment& env) {
			if (args.empty()) {
				env.Assert(
					env.returnValues.size() >= 2, "And: Needs at least 2 return items"
				);
				auto r1 = env.returnValues.back();
				env.returnValues.pop_back();
				auto r2 = env.returnValues.back();
				env.returnValues.pop_back();
				
				return std::vector <int>{(r1[0] != 0) && (r2[0] != 0)};
			}
			else {
				env.Assert(
					args.size() == 2, "And: Needs either no arguments or 2 arguments"
				);
				env.Assert(
					Util::IsInteger(args[0]) && Util::IsInteger(args[1]),
					"And: Needs integer arguments"
				);

				return std::vector <int>{(stoi(args[0]) != 0) && (stoi(args[1]) != 0)};
			}
		}
		std::vector <int> Or(const std::vector <std::string>& args, Environment& env) {
			if (args.empty()) {
				env.Assert(
					env.returnValues.size() >= 2, "Or: Needs at least 2 return items"
				);
				auto r1 = env.returnValues.back();
				env.returnValues.pop_back();
				auto r2 = env.returnValues.back();
				env.returnValues.pop_back();
				
				return std::vector <int>{(r1[0] != 0) || (r2[0] != 0)};
			}
			else {
				env.Assert(
					args.size() == 2, "Or: Needs either no arguments or 2 arguments"
				);
				env.Assert(
					Util::IsInteger(args[0]) && Util::IsInteger(args[1]),
					"Or: Needs integer arguments"
				);

				return std::vector <int>{(stoi(args[0]) != 0) || (stoi(args[1]) != 0)};
			}
		}
	}
}

#endif
