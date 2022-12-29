#include "environment.hh"
#include "util.hh"
#include "builtins.hh"

Environment::Environment():
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
	builtins["is_num"]       = STD::IsNum;
	builtins["atoi"]         = STD::Atoi;
	builtins["itoa"]         = STD::Itoa;
	builtins["load_end"]     = STD::LoadEnd;
	builtins["error"]        = STD::Error;
	builtins["sqrt"]         = STD::Sqrt;
	builtins["local"]        = STD::Local;

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

void Environment::ExitError(bool showExit) {
	if (fromProgram) {
		if (showExit) {
			fprintf(
				stderr, "Exited at line %i\n",
				(int) lineAt->first
			);
		}
		fprintf(stderr, "\t%s\n", lineAt->second.c_str());
		fprintf(stderr, "backtrace:\n");
		for (size_t i = 0; i < calls.size(); ++i) {
			fprintf(
				stderr, "#%lli (At %i): %s\n", (long long int) i,
				(int) calls[i]->first,
				calls[i]->second.c_str()
			);
		}
	}
	exit(1);
}

void Environment::Assert(bool b, std::string error) {
	if (!b) {
		fprintf(stderr, "%s\n", error.c_str());
		ExitError();
	}
}

void Environment::AddFunction(std::string name, Function function) {
	builtins[name] = function;
}

void Environment::SetVariable(std::string name, std::vector <int> value) {
	variables[name] = value;
}

void Environment::LoadExtension(const Extension& ext) {
	for (auto it = ext.functions.begin(); it != ext.functions.end(); ++it) {
		std::string functionName = ext.name + "." + it->first;
		builtins[functionName]   = it->second;
	}
	for (auto it = ext.variables.begin(); it != ext.variables.end(); ++it) {
		std::string varName = ext.name + "." + it->first;
		variables[varName]  = it->second;
	}
}

std::vector <std::string> Environment::AddVariables(const std::vector <std::string>& args) {
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

			auto it = program.begin();
			bool found = false;
			if (arg[1] == '.') { // sub-label
				auto it2 = lineAt;
				while (it2 != program.begin()) {
					-- it2;
					if (
						(it2->second.length() > 0) &&
						(it2->second[it2->second.length() - 1] == ':') &&
						(Util::TrimString(it2->second)[0] != '.')
					) {
						break;
					}
					/*printf("%s (%c)(%c)\n",
					Util::TrimString(it2->second).c_str(),
					it2->second[0],
					it2->second[it2->second.length() - 1]
					);*/
				}
				it = it2;
			}
		
			for (; it != program.end(); ++it) {
				if (found) {
					break;
				}
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

void Environment::Interpret(std::string code) {
	code = Util::StringReplace(code, '\t', ' ');

	if (Util::TrimString(code)[0] == '#') {
		return; // comment
	}
	auto parts = Util::SplitString(code, ' ', -1);
	if (parts.empty()) {
		return;
	}

	{
		auto expanded = Util::SplitString(
			AddVariables(std::vector <std::string>{parts[0]})[0], ' ', -1
		);
		auto newParts = expanded;
		parts.erase(parts.begin());
		for (auto& part : parts) {
			newParts.push_back(part);
		}
		parts = newParts;
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
			ExitError();
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
				ExitError(false);
			}
		}
		if (!ret.empty()) {
			returnValues.push_back(ret);
		}
	}
}
