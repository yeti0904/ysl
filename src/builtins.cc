#include "builtins.hh"
#include "environment.hh"
#include "util.hh"

std::vector <int> STD::Print(const std::vector <std::string>& args, Environment&) {
	for (size_t i = 0; i < args.size(); ++i) {
		printf("%s%s", args[i].c_str(), i == args.size() - 1? "" : " ");
	}
	return {};
}

std::vector <int> STD::PrintLn(const std::vector <std::string>& args, Environment& env) {
	Print(args, env);
	puts("");
	return {};
}

std::vector <int> STD::Exit(const std::vector <std::string>&, Environment&) {
	exit(0);
	return {};
}

std::vector <int> STD::Run(const std::vector <std::string>&, Environment& env) {
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

std::vector <int> STD::Goto(const std::vector <std::string>& args, Environment& env) {
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

std::vector <int> STD::GotoIf(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 1, "GotoIf: needs 1 argument");

	if (!env.returnValues.empty() && (env.returnValues.back()[0] != 0)) {
		return Goto(args, env);
	}
	return {};
}

std::vector <int> STD::GoSub(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() >= 1, "GoSub: Needs at least 1 argument");

	env.calls.push_back(env.lineAt);
	env.locals.push_back({});

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
	
	return Goto({args[0]}, env);
}

std::vector <int> STD::GoSubIf(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() >= 1, "GoSubIf: Needs at least 1 argument");

	if (!env.returnValues.empty() && (env.returnValues.back()[0] != 0)) {
		env.calls.push_back(env.lineAt);
		env.locals.push_back({});
	}

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
	
	return GotoIf({args[0]}, env);
}

std::vector <int> STD::Wait(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 1, "Wait: needs 1 argument");
	env.Assert(Util::IsInteger(args[0]), "Wait: argument must be integer");

	std::this_thread::sleep_for(std::chrono::milliseconds(stol(args[0])));
	return {};
}

std::vector <int> STD::Cmp(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 2, "Cmp: needs 2 arguments");

	return {args[0] == args[1]? 1 : 0};
}

std::vector <int> STD::Var(const std::vector <std::string>& args, Environment& env) {
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
			env.Assert(
				!env.variables[args[0]].empty(), "Var: empty variable " + args[0]
			);
		
			env.variables[args[0]][0] += stoi(args[2]);
			break;
		}
		case '-': {
			env.Assert(
				Util::IsInteger(args[2]), "Var: argument 3 must be an integer"
			);
			env.Assert(
				!env.variables[args[0]].empty(), "Var: empty variable " + args[0]
			);
		
			env.variables[args[0]][0] -= stoi(args[2]);
			break;
		}
		case '*': {
			env.Assert(
				Util::IsInteger(args[2]), "Var: argument 3 must be an integer"
			);
			env.Assert(
				!env.variables[args[0]].empty(), "Var: empty variable " + args[0]
			);
		
			env.variables[args[0]][0] *= stoi(args[2]);
			break;
		}
		case '/': {
			env.Assert(
				Util::IsInteger(args[2]), "Var: argument 3 must be an integer"
			);
			env.Assert(
				!env.variables[args[0]].empty(), "Var: empty variable " + args[0]
			);
		
			env.variables[args[0]][0] /= stoi(args[2]);
			break;
		}
		case '%': {
			env.Assert(
				Util::IsInteger(args[2]), "Var: argument 3 must be an integer"
			);
			env.Assert(
				!env.variables[args[0]].empty(), "Var: empty variable " + args[0]
			);
		
			env.variables[args[0]][0] %= stoi(args[2]);
			break;
		}
		case '^': {
			env.Assert(
				Util::IsInteger(args[2]), "Var: argument 3 must be an integer"
			);
			env.Assert(
				!env.variables[args[0]].empty(), "Var: empty variable " + args[0]
			);

			env.variables[args[0]][0] = (int) pow(
				(double) env.variables[args[0]][0], (double) stoi(args[2])
			);
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
				env.Assert(!env.returnValues.empty(), "Var: return stack empty");
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
				env.Assert(!env.returnValues.empty(), "Var: return stack empty");
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
		case 'j': {
			env.variables[args[0]] = Util::StringToIntVector(
				Util::IntVectorToString(env.variables[args[0]]) + args[3]
			);
			break;
		}
		default: {
			fprintf(stderr, "Var: unknown operation %c\n", args[1][0]);
			env.ExitError();
		}
	}

	return {};
}

std::vector <int> STD::Load(const std::vector <std::string>& args, Environment& env) {
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

std::vector <int> STD::Size(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 1, "Size: needs 1 argument");

	return {(int) env.variables[args[0]].size()};
}

std::vector <int> STD::Getch(const std::vector <std::string>&, Environment&) {
	return {getchar()};
}

std::vector <int> STD::Input(const std::vector <std::string>&, Environment&) {
	std::string input;
	std::getline(std::cin, input, '\n');

	std::vector <int> ret;
	for (auto& ch : input) {
		ret.push_back(ch);
	}

	return ret;
}

std::vector <int> STD::Putch(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 1, "Putch: needs 1 argument");
	env.Assert(Util::IsInteger(args[0]), "Putch: requires integer argument");

	putchar(stoi(args[0]));
	return {};
}

std::vector <int> STD::SetSize(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 2, "SetSize: needs 2 arguments");

	auto& arr = env.variables[args[0]];
	arr.resize(stoi(args[1]));
	return {};
}

std::vector <int> STD::Return(const std::vector <std::string>& args, Environment& env) {
	env.Assert(!env.calls.empty(), "Return: nowhere to return to");
	env.Assert(!env.locals.empty(), "Return: missing locals (this is a bug");

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

	// restore locals
	for (auto& local : env.locals.back()) {
		env.variables[local.name] = local.oldValue;
	}
	env.locals.pop_back();
	
	return {};
}

std::vector <int> STD::Debug(const std::vector <std::string>&, Environment& env) {
	env.yslDebug = !env.yslDebug;
	puts(std::to_string(env.yslDebug).c_str());
	return {};
}

std::vector <int> STD::Swap(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 2, "Swap: needs 2 arguments");
	
	std::swap(env.variables[args[0]], env.variables[args[1]]);
	return {};
}

std::vector <int> STD::Gt(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 2, "Gt: needs at least 2 arguments");
	env.Assert(
		Util::IsInteger(args[0]) && Util::IsInteger(args[1]),
		"Gt: needs integer arguments"
	);

	return {
		(stoi(args[0]) > stoi(args[1]))? 1 : 0
	};
}

std::vector <int> STD::Lt(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 2, "Gt: needs 2 arguments");
	env.Assert(
		Util::IsInteger(args[0]) && Util::IsInteger(args[1]),
		"Lt: needs integer arguments"
	);

	return {
		(stoi(args[0]) < stoi(args[1]))? 1 : 0
	};
}

std::vector <int> STD::Pow(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 2, "Pow: needs 2 arguments");
	env.Assert(
		Util::IsInteger(args[0]) && Util::IsInteger(args[1]),
		"Pow: needs integer arguments"
	);

	return {(int) pow((double) stoi(args[0]), (double) stoi(args[1]))};
}

std::vector <int> STD::StringArray(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() >= 2, "StringArray: Needs at least 2 arguments");

	switch (args[0][0]) {
		case 'n': {
			std::vector <std::string> newArray;
			for (size_t i = 1; i < args.size(); ++i) {
				newArray.push_back(args[i]);
			}
			env.returnValues.push_back(Util::StringVectorToIntVector(newArray));
			return {};
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
				"StringArray: Index " + std::to_string(index) +
				" out of range for array of size " +
				std::to_string(array.size())
			);

			env.returnValues.push_back(Util::StringToIntVector(array[index]));
			return {};
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
				args.size() == 3, "StringArray: r operator needs 3 arguments"
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

std::vector <int> STD::Split(const std::vector <std::string>&, Environment&) {
	fprintf(stderr, "YSL: Split is no longer supported\n");
	exit(1);
	return {};
}

std::vector <int> STD::Not(const std::vector <std::string>& args, Environment& env) {
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

std::vector <int> STD::And(const std::vector <std::string>& args, Environment& env) {
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

std::vector <int> STD::Or(const std::vector <std::string>& args, Environment& env) {
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

std::vector <int> STD::IsNum(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 1, "IsNum: Needs 1 argument");

	return std::vector <int>{Util::IsInteger(args[0])? 1 : 0};
}

std::vector <int> STD::Atoi(const std::vector <std::string>& args, Environment& env) {
	env.Assert(!args.empty(), "Atoi: Needs 1 argument");
	env.Assert(
		Util::IsInteger(args[0]), "Atoi: String must be numeric"
	);

	return std::vector <int>{stoi(args[0])};
}

std::vector <int> STD::Itoa(const std::vector <std::string>& args, Environment& env) {
	env.Assert(!args.empty(), "Itoa: needs 1 argument");
	env.Assert(Util::IsInteger(args[0]), "Itoa: needs integer arguments");

	return Util::StringToIntVector(args[0]);
}

std::vector <int> STD::LoadEnd(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 1, "LoadEnd: needs 1 argument");

	std::ifstream fhnd(args[0]);

	env.Assert(fhnd.good(), "LoadEnd: Failed to read file: " + args[0]);
	
	std::string   line;
	size_t        lineNum = 10;

	if (!env.program.empty()) {
		lineNum += env.program.rbegin()->first;
	}

	while (getline(fhnd, line)) {
		env.program[lineNum] =  line;
		lineNum              += 10;
	}
	fhnd.close();
	return {};
}

std::vector <int> STD::Error(const std::vector <std::string>&, Environment& env) {
	env.ExitError();
	return {};
}

std::vector <int> STD::Sqrt(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() == 1, "Sqrt: Requires 1 argument");
	env.Assert(Util::IsInteger(args[0]), "Sqrt: Requires integer argument");

	return {(int) round(sqrt((double) stoi(args[0])))};
}

std::vector <int> STD::Local(const std::vector <std::string>& args, Environment& env) {
	for (auto& arg : args) {
		env.locals.back().push_back({arg, env.variables[arg]});
	}

	return {};
}

std::vector <int> STD::Matrix(const std::vector <std::string>& args, Environment& env) {
	env.Assert(args.size() >= 2, "Matrix: Operation/variable name not given");

	auto& variable = env.variables[args[0]];

	if (args[1][0] != 'c') {
		env.Assert(
			(variable.size() >= 2) &&
			(variable.size() == (size_t) (variable[0] * variable[1]) + 2),
			"Matrix: Invalid matrix"
		);
	}

	switch (args[1][0]) {
		case 'c': {
			env.Assert(args.size() == 4, "Matrix: C requires 2 extra arguments");
			env.Assert(
				Util::IsInteger(args[2]) && Util::IsInteger(args[3]),
				"Matrix: C requires 2 integer arguments"
			);
			std::vector <int> matrix;
			matrix.resize(2 + (stoi(args[2]) * stoi(args[3])));
			matrix[0] = stoi(args[2]);
			matrix[1] = stoi(args[3]);

			env.variables[args[0]] = matrix;
			break;
		}
		case 'g': {
			env.Assert(args.size() == 4, "Matrix: G requires 2 extra arguments");
			env.Assert(
				Util::IsInteger(args[2]) && Util::IsInteger(args[3]),
				"Matrix: G requires 2 integer arguments"
			);
			env.Assert(
				(stoi(args[2]) < variable[0]) && (stoi(args[3]) < variable[1]),
				"Matrix: Indexes out of bounds"
			);

			return {variable[
				 ((stoi(args[2]) * variable[0]) + stoi(args[3])) + 2
			]};
		}
		case 's': {
			auto& variable = env.variables[args[0]];
			env.Assert(args.size() == 5, "Matrix: S requires 3 extra arguments");
			env.Assert(
				Util::IsInteger(args[2]) && Util::IsInteger(args[3]) &&
				Util::IsInteger(args[4]),
				"Matrix: S requires 2 integer arguments"
			);
			env.Assert(
				(stoi(args[2]) < variable[0]) && (stoi(args[3]) < variable[1]),
				"Matrix: Indexes out of bounds"
			);

			env.variables[args[0]][
				((stoi(args[2]) * variable[0]) + stoi(args[3])) + 2
			] = stoi(args[4]);
			break;
		}
		default: {
			fprintf(stderr, "Matrix: Unknown operation %c\n", args[1][0]);
			env.ExitError();
		}
	}

	return {};
}
