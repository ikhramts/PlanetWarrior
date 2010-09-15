// Available under GPLv3.
// Adapted from code by Albert Zeyer at http://github.com/albertz/planet_wars-cpp/blob/master/
// Author: Iouri Khramtsov

#include "utils.h"

typedef unsigned char uchar;

std::string TrimSpaces(const std::string& str) {
    size_t start = 0;
    for(size_t i = 0; i < str.size(); ++i)
        if(!isspace((uchar)str[i]) || isgraph((uchar)str[i])) {
            start = i;
            break;
        }

    size_t n = 0;
    for(size_t i = str.size(); i > start; --i)
        if(!isspace((uchar)str[i-1]) || isgraph((uchar)str[i-1])) {
            n = i - start;
            break;
        }

    return str.substr(start, n);
}

std::vector<std::string> Tokenize(const std::string& s, const std::string& delimiters) {
    std::vector<std::string> tokens;

    std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
    std::string::size_type pos = s.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos) {
        tokens.push_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(delimiters, pos);
        pos = s.find_first_of(delimiters, lastPos);
    }

    return tokens;
}
