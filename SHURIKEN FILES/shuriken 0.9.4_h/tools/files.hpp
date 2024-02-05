#ifndef files_HPP
#define files_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class intstr
{
public:
    int integer;
    string text;
};

class files
{
public:
    files();
    static vector<string> readLine(string line);
    static vector<float> readFloatLine(string line);
    static intstr getLineValue(int i, int width, string content);
    static bool FileExists(string name);

    //Resources textures
    string purpleNinjaTile;
    string tankTile;
    string tank2Tile;
    string tank3Tile;
    string clownTile;
    string bossTile;
    string parachute;
    string projectileTile;
    string projectile2Tile;
    string powerUps;
    string gameHud;
    string bonusHud;
    string controlArrows;
    string explosionTile;
    string explosion2Tile;
    string teleportEffect;
    string platform;
    string speedwalk_wheel;
    string speedwalk_carpet;
    string switchTile;
    string blocker;
    string warning;
    string arrowBlocks;
    string fire;
    string life;
    string lives;
    string layer0;
    string layer1;
    string titleScreenTitle;
    string settingsFrame;
    string pauseFrame;
    string pixelfont;

    //Configuration files
    string layer0File[4];
    string layer1File[4];
    string collisionFile[4];
    string* keysFile;
    string settingsFile;

    //Fonts
    string font1;
    string font2;
};
#endif
