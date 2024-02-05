#include "utils.hpp"

using namespace std;

utils::utils()
{
}

bool utils::strContains(string searchStr, string findStr)
{
    return searchStr.find(findStr) != std::string::npos;
}

bool utils::ListContains(vector<string> list, string text)
{
    for(int i=0 ; i<list.size() ; i++)
        if(list[i] == text)
            return true;
    return false;
}

float utils::DistanceBetween(Vector2f pt1, Vector2f pt2)
{
    return sqrt(pow(pt2.x-pt1.x,2.f) + pow(pt2.y-pt1.y,2.f));
}
