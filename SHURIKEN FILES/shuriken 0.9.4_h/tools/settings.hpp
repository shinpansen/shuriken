#ifndef settings_HPP
#define settings_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "xmlReader.hpp"
#include "utils.hpp"

using namespace std;
using namespace sf;

class settings
{
public:
    //Methods
    settings();
    void Save();
    void ChooseNextSetting(int num);
    void ChoosePreviousSetting(int num);
    int GetLanguageID();
    bool IsAnInteger(string val);
    bool IsAnBoolean(string val);

    //Variables
    string language;
    vector<string> languageList;
    int volume;
    int screenRatio;
    int maxScreenRatio;
    bool fullscreen;
};
#endif
