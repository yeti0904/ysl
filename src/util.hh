#ifndef YSL_UTIL_HH
#define YSL_UTIL_HH

#include "_components.hh"

namespace Util {
	bool                      IsInteger(std::string str);
	bool                      StringStartsWith(std::string str, std::string prefix);
	std::string               StringReplace(std::string str, char from, char to);
	std::string               TrimString(std::string str);
	std::vector <int>         StringToIntVector(std::string str);
	std::vector <int>         StringVectorToIntVector(std::vector <std::string> vec);
	std::vector <std::string> IntVectorToStringVector(std::vector <int> vec);
	std::vector <char*>       StringVectorToCharpVector(std::vector <std::string> vec);
	std::vector <std::string> SplitString(std::string str, char splitter, ssize_t maxSplit);
	bool                      ValidYSLMatrix(std::vector <int>& vec);

	std::vector <std::vector <int>> IntVectorTo2dIntVector(std::vector <int> vec);
	
}

#endif
