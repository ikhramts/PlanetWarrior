// Available under GPLv3.
// Adapted from code by Albert Zeyer at http://github.com/albertz/planet_wars-cpp/blob/master/
// Author: Iouri Khramtsov

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>


//Trim spaces from the begining and end of a string.
std::string TrimSpaces(const std::string& str);

//Split a string into tokens given delimeters to use during tokenization.
std::vector<std::string> Tokenize(const std::string& s, const std::string& delimiters);


#endif // UTILS_H
