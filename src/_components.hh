#ifndef YSL__COMPONENTS_HH
#define YSL__COMPONENTS_HH

// C standard libraries
#include <math.h>
#include <ctype.h>
#include <string.h>

// C++ standard libraries
#include <map>
#include <regex>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>
#include <algorithm>

// macros
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

#endif
