#include "settings.hpp"

using namespace std;
using namespace sf;

static const float NATIVE_WIDTH = 280;
static const float NATIVE_HEIGHT = 192;

settings::settings()
{
    //Init
    languageList = xmlReader::getLanguagesList();
    if(languageList.size() > 0)
        language = languageList[0];
    else
    {
        cout << "Error. Language file is corrupted." << endl;
        exit(1);
    }

    volume = 10;
    screenRatio = 1;
    fullscreen = false;

    //Searching for max screen ratio
    int width = VideoMode::getDesktopMode().width;
    int height = VideoMode::getDesktopMode().height;
    maxScreenRatio = 1;
    while(NATIVE_WIDTH*(maxScreenRatio+1) < width && NATIVE_HEIGHT*(maxScreenRatio+1) < height)
        maxScreenRatio ++;

    //Read xml file
    vector<string> settingsFromXml = xmlReader::getSettingsValue();
    if(settingsFromXml.size() >= 4)
    {
        if(utils::ListContains(languageList, settingsFromXml[0]))
            language = settingsFromXml[0];
        if(IsAnInteger(settingsFromXml[1]) && stoi(settingsFromXml[1]) <= 10)
            volume = stoi(settingsFromXml[1]);
        if(IsAnInteger(settingsFromXml[2]) && stoi(settingsFromXml[2]) <= maxScreenRatio)
            screenRatio = stoi(settingsFromXml[2]);
        fullscreen = IsAnBoolean(settingsFromXml[3]) ? stoi(settingsFromXml[3]) : false;
        Save();
    }
    else
        Save();
}

void settings::Save()
{
    files _files;
    ofstream inputFile(_files.settingsFile.c_str(), ios::out | ios::trunc);
    if(inputFile)
    {
        inputFile << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
        inputFile << "<settings>\n";
        inputFile << "\t<option type=\"language\">\n\t\t" << language << "\n\t</option>\n";
        inputFile << "\t<option type=\"volume\">\n\t\t" << volume << "\n\t</option>\n";
        inputFile << "\t<option type=\"screenRatio\">\n\t\t" << screenRatio << "\n\t</option>\n";
        inputFile << "\t<option type=\"fullscreen\">\n\t\t" << fullscreen << "\n\t</option>\n";
        inputFile << "</settings>";
    }
}

void settings::ChooseNextSetting(int num)
{
    int languageID = GetLanguageID();
    switch(num)
    {
        case 0:
            language = languageID < languageList.size()-1 ? languageList[languageID+1] : languageList[0];
            break;
        case 1:
            volume = volume < 10 ? volume + 1 : 0;
            break;
        case 2:
            screenRatio = screenRatio < maxScreenRatio ? screenRatio + 1 : 1;
            break;
        case 3:
            fullscreen = !fullscreen;
            break;
    }
    Save();
}

void settings::ChoosePreviousSetting(int num)
{
    int languageID = GetLanguageID();
    switch(num)
    {
        case 0:
            language = languageID > 0 ? languageList[languageID-1] : languageList[languageList.size()-1];
            break;
        case 1:
            volume = volume > 0 ? volume - 1 : 10;
            break;
        case 2:
            screenRatio = screenRatio > 1 ? screenRatio - 1 : maxScreenRatio;
            break;
        case 3:
            fullscreen = !fullscreen;
            break;
    }
    Save();
}

int settings::GetLanguageID()
{
    for(int i=0 ; i<languageList.size() ; i++)
        if(languageList[i] == language)
            return i;
    return -1;
}

bool settings::IsAnInteger(string val)
{
    try
    {
        int integer = stoi(val);
        return integer >= 0;
    }
    catch(exception)
    {
        return false;
    }
}

bool settings::IsAnBoolean(string val)
{
    try
    {
        int boolean = stoi(val);
        return boolean >= 0 && boolean <= 1;
    }
    catch(exception)
    {
        return false;
    }
}
