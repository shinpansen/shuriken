#ifndef utils_HPP
#define utils_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class utils
{
public:
    utils();
    static bool strContains(string searchStr, string findStr);
    static bool ListContains(vector<string> list, string text);
    static float DistanceBetween(Vector2f pt1, Vector2f pt2);
};
#endif
