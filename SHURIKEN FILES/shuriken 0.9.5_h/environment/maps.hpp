#ifndef maps_HPP
#define maps_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../tools/files.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/settings.hpp"
#include "../UI/scoreManager.hpp"
#include "../effects/effects.hpp"
#include "../characters/enemy.hpp"
#include "../characters/blueTank.hpp"
#include "../characters/greenTank.hpp"
#include "../characters/redTank.hpp"
#include "../characters/clown.hpp"
#include "../characters/boss.hpp"
#include "../objects/platform.hpp"
#include "../objects/fire.hpp"
#include "../objects/speedwalk.hpp"
#include "../objects/blocker.hpp"
#include "../objects/arrowBlock.hpp"

using namespace std;
using namespace sf;

class maps
{
public:
    maps();
    maps(b2World* world, effects* effectBox, sound* soundBox, scoreManager* scoreManager,
         settings* settings, vector<hero*>* ninjas, int level);
    void LoadMap(int level);
    void DestroyCurrentMap();
    void CreateLayers();
    void CreateCollisions();
    int GetLevel();
    void PerformEvents(vector<enemy*>* enemies, scoreManager* scoreManager, effects* effectBox,
                       projectile* projectileBox, sound* soundBox, powerups* powerups);
    void PerformPortals(vector<Vector2f> portalPos, vector<enemy*>* enemies, scoreManager* scoreManager,
                       effects* effectBox, sound* soundBox);
    int* NbEnemiesByZone(vector<enemy*>* enemies);
    int GetEnemiesToKill();
    int GetEnemiesToMiss();
    void DisplayLayer0(RenderWindow* window);
    void DisplayLayer1(RenderWindow* window);

    //Resources
    Texture _layer1;
    vector<Sprite*> _layer0Tiles;
    vector<Sprite*> _layer1Tiles;
    vector<b2Body*> _bodies;
private:
    //Objects
    b2World* _world;
    files _files;
    effects* _effectBox;
    sound* _soundBox;
    scoreManager* _scoreManager;
    settings* _settings;
    vector<hero*>* _ninjas;
    vector<platform*> _platforms;
    vector<fire*> _fires;
    vector<speedwalk*> _speedwalks;
    vector<blocker*> _blockers;
    vector<arrowBlock*> _arrowsTeleportA;
    vector<arrowBlock*> _arrowsTeleportB;
    vector<arrowBlock*> _arrowsSuperJump;

    //Variable
    int _level;
    int _enemiesToKill[3];
    int _enemiesToMiss[3];
    int _spawnTimer;
    vector<Vector2f> _level1Portals;
    vector<Vector2f> _level2Portals;
    vector<Vector2f> _level3Portals;

    //Textures
    Texture _textureTank1;
    Texture _textureTank2;
    Texture _textureTank3;
    Texture _textureClown;
    Texture _textureBoss;
    Texture _textureParachute;
    Texture _texturePlatform;
    Texture _textureFire;
    Texture _textureWheel;
    Texture _textureCarpet;
    Texture _textureSwitch;
    Texture _textureBlocker;
    Texture _textureWarning;
    Texture _textureArrowBlock;

    bool _stop;
};
#endif
