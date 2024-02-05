#ifndef utils_HPP
#define utils_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

class utils
{
public:
    utils();
    static bool strContains(string searchStr, string findStr);
    static bool ListContains(vector<string> list, string text);
};
#endif
