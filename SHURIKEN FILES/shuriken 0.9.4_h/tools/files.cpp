#include "files.hpp"

using namespace std;
using namespace sf;

files::files()
{
    //Resources textures
    purpleNinjaTile = "resources//characters//purpleNinja.png";
    tankTile = "resources//characters//tank.png";
    tank2Tile = "resources//characters//tank2.png";
    tank3Tile = "resources//characters//tank3.png";
    clownTile = "resources//characters//clown.png";
    bossTile = "resources//characters//boss.png";
    parachute = "resources//objects//parachute.png";
    projectileTile = "resources//effects//projectiles.png";
    projectile2Tile = "resources//effects//projectiles2.png";
    powerUps = "resources//objects//powerups.png";
    gameHud = "resources//UI//game_hud.png";
    bonusHud = "resources//UI//bonus_hud.png";
    controlArrows = "resources//UI//control_arrows.png";
    explosionTile = "resources//effects//explosions.png";
    explosion2Tile = "resources//effects//explosions2.png";
    teleportEffect = "resources//effects//teleportEffect.png";
    platform = "resources//objects//platform.png";
    speedwalk_wheel = "resources//objects//speedwalk_wheel.png";
    speedwalk_carpet = "resources//objects//speedwalk_carpet.png";
    switchTile = "resources//objects//switch.png";
    blocker = "resources//objects//blocker.png";
    warning = "resources//UI//warning.png";
    arrowBlocks = "resources//objects//arrowBlocks.png";
    fire = "resources//objects//fire.png";
    life = "resources//UI//life.png";
    lives = "resources//UI//lives.png";
    layer0 = "resources//tiles//layer0.png";
    layer1 = "resources//tiles//layer1.png";
    titleScreenTitle = "resources//UI//title.png";
    settingsFrame = "resources//UI//settings_frame.png";
    pauseFrame = "resources//UI//pause_frame.png";
    pixelfont = "resources//fonts//pixel_font.png";

    //Configuration files
    layer0File[0] = "environment//mapFiles//LEVEL_1//layer0";
    layer0File[1] = "environment//mapFiles//LEVEL_2//layer0";
    layer0File[2] = "environment//mapFiles//LEVEL_3//layer0";
    layer0File[3] = "environment//mapFiles//LEVEL_4//layer0";
    layer1File[0] = "environment//mapFiles//LEVEL_1//layer1";
    layer1File[1] = "environment//mapFiles//LEVEL_2//layer1";
    layer1File[2] = "environment//mapFiles//LEVEL_3//layer1";
    layer1File[3] = "environment//mapFiles//LEVEL_4//layer1";
    collisionFile[0] = "environment//mapFiles//LEVEL_1//collision";
    collisionFile[1] = "environment//mapFiles//LEVEL_2//collision";
    collisionFile[2] = "environment//mapFiles//LEVEL_3//collision";
    collisionFile[3] = "environment//mapFiles//LEVEL_4//collision";

    //Keys
    keysFile = new string[4];
    keysFile[0] = "resources//xml//keys_1.xml";
    keysFile[1] = "resources//xml//keys_2.xml";
    keysFile[2] = "resources//xml//keys_3.xml";
    keysFile[3] = "resources//xml//keys_4.xml";
    settingsFile = "resources//xml//settings.xml";

    //Fonts
    font1 = "resources//fonts//rainyhearts.ttf";
    font2 = "resources//fonts//retro.otf";
}

vector<string> files::readLine(string line)
{
    try
    {
        int i = 0, width = 0;
        intstr flux;
        vector<string> resultVector;
        while (i < line.size())
        {
            flux = getLineValue(i, width, line);
            i = width = flux.integer;
            resultVector.push_back(flux.text);
        }
        return resultVector;
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

vector<float> files::readFloatLine(string line)
{
    try
    {
        int i = 0, width = 0;
        intstr flux;
        vector<float> resultVector;
        float value;
        while (i < line.size())
        {
            flux = getLineValue(i, width, line);
            i = width = flux.integer;
            istringstream(flux.text) >> value;
            resultVector.push_back(value);
        }
        return resultVector;
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

intstr files::getLineValue(int i, int width, string content)
{
    try
    {
        string str;
        while(i <= content.size())
        {
            if(content.substr(i,1) == ",")
            {
                str = content.substr(width, i-width);
                width = i + 1;
                break;
            }
            i++;
        }
        intstr result;
        result.integer = width;
        result.text = str;
        return result;//Vector2f(width, nb);
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

bool files::FileExists(string name)
{
    ifstream f(name.c_str());
    if (f.good())
    {
        f.close();
        return true;
    }
    else
    {
        f.close();
        return false;
    }
}
