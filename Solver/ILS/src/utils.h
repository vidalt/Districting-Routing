#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include "functional"
#include "random"
#include <fstream>
#include <map>

using namespace std;

namespace utils {

    // Given an string s and some character c, returns and array of strings with the substrings of s separated by c
    vector<string> splitString(const string &s, const char &c);

    // Try to open file, if it is busy try some more times
    ifstream openCommonFile(string path);
}

#endif