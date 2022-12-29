#include "util.hh"

bool Util::IsInteger(std::string str) {
    try {
    	stoi(str);
    }
    catch (...) {
    	return false;
    }
    return true;
}

bool Util::StringStartsWith(std::string str, std::string prefix) {
	if (prefix.length() > str.length()) {
		return false;
	}
	return str.substr(0, prefix.length()) == prefix;
}

std::string Util::StringReplace(std::string str, char from, char to) {
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

std::string Util::TrimString(std::string str) {
	// too lazy to make it trim the end too
	for (size_t i = 0; i < str.length(); ++i) {
		if (!isspace(str[i])) {
			return str.substr(i, str.length() - i);
		}
	}
	return str;
}

std::vector <int> Util::StringToIntVector(std::string str) {
	std::vector <int> ret;

	for (auto& ch : str) {
		ret.push_back(ch);
	}

	return ret;
}
std::vector <int> Util::StringVectorToIntVector(std::vector <std::string> vec) {
	std::vector <int> ret;

	for (auto& str : vec) {
		for (auto& ch : str) {
			ret.push_back(ch);
		}
		ret.push_back(0);
	}

	return ret;
}
std::vector <std::string> Util::IntVectorToStringVector(std::vector <int> vec) {
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
std::vector <char*> Util::StringVectorToCharpVector(std::vector <std::string> vec) {
	std::vector <char*> ret;

	for (auto& str : vec) {
		ret.push_back(strdup(str.c_str()));
	}

	ret.push_back(nullptr);

	return ret;
}

std::vector <std::string> Util::SplitString(
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
    				break;
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

bool Util::ValidYSLMatrix(std::vector <int>& vec) {
	if (vec.size() < 2) {
		return false; // no metadata
	}
	if ((int) vec.size() != 2 + (vec[0] * vec[1])) {
		return false;
	}
	return true;
}

std::vector <std::vector <int>> Util::IntVectorTo2dIntVector(std::vector <int> vec) {
	std::vector <std::vector <int>> ret;
	int width  = vec[0];
	int height = vec[1];

	for (int y = 0, i = 0; y < height; ++y) {
		ret.push_back({});
		for (int x = 0; (i < (int) vec.size() - 2) && (x < width); ++x, ++i) {
			ret.back().push_back(vec[i]);
		}
		++ i;
	}

	return ret;
}
