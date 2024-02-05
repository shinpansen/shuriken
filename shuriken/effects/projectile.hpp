#ifndef projectile_hpp
#define projectile_hpp

#include <iostream>
#include <math.h>
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../tools/animation.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/files.hpp"
#include "../characters/hero.hpp"
#include "../characters/enemy.hpp"
#include "effects.hpp"
#include "sound.hpp"

using namespace std;
using namespace sf;

class hero;
class enemy;
class projectile
{
public:
    projectile();
    projectile(b2World* world, effects* effectBox, sound* soundBox);
    void AddProjectile(Vector2i pos, float frameDelay, int width, int height, int nbFrames,
                       int numAnimation, int angle, Vector2f speed, int type);
    void DestroyProjectile(int i, bool showEffect, bool playSound);
    void DestroyAllProjectiles();
    void DestroyLine(int posY);
    bool ProjectileInLine(int posY);
    void ChangeDommages(float value);
    int GetDommagesRatio();
    void UpdatePointers(vector<hero*>* ninjas, vector<enemy*>* enemies);
    void PerformContacts(b2Body* projectile, Vector2f* speedOfProjectile, int projectileNumber);
    bool isAProjectile(b2Body* projectile, int* projectileNumber);
    bool AboveAProjectile(Vector2f pos, Vector2f* projectilePos);
    void RenderProjectiles(float deltaTime);
    void Display(RenderWindow* window);
private:
    float _deltaTime;
    float _dommagesRatio;
    files _files;
    b2World* _world;
    effects* _effectBox;
    sound* _soundBox;
    vector<Sprite*> _tiles;
    vector<animation*> _animations;
    vector<b2Body*> _projectileBodies;
    vector<Vector2f> _speedOfProjectile;
    vector<int> _lifeOfProjectile;
    vector<hero*>* _ninjas;
    vector<enemy*>* _enemies;
    Texture _projectileTexture;
};
#endif
