#ifndef YSL_ENVIRONMENT_HH
#define YSL_ENVIRONMENT_HH

#include "_components.hh"

class Environment;
typedef std::vector <int> (*Function)(const std::vector <std::string>&, Environment&);

class Extension {
	public:
		std::string                      name;
		std::map <std::string, Function> functions;
};

struct LocalEntry {
	std::string       name;
	std::vector <int> oldValue;
};

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
		std::vector <std::vector <LocalEntry>>    locals;

		std::vector <std::map <size_t, std::string>::iterator> calls;

		Environment();
		void                      ExitError(bool showExit = true);
		void                      Assert(bool b, std::string error);
		void                      AddFunction(std::string name, Function function);
		void                      SetVariable(std::string name, std::vector <int> value);
		void                      LoadExtension(const Extension& ext);
		std::vector <std::string> AddVariables(const std::vector <std::string>& args);
		void                      Interpret(std::string code);
};

#endif
