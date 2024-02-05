#ifndef clown_hpp
#define clown_hpp

#include <math.h>
#include <string.h>
#include "enemy.hpp"

using namespace std;
using namespace sf;

class clown : public enemy
{
public:
    clown(Vector2i pos, b2World* world, effects* effectBox, projectile* projectileBox,
             sound* soundBox, scoreManager* scoreManager, powerups* powerups, Texture* textureEnemy,
             Texture* textureParachute) :
    enemy(Vector2i(8,23), Vector2i(32,26), Vector2i(14,13), 10, 10, ENEMY_CLOWN,
          pos, world, effectBox, projectileBox, soundBox, scoreManager, powerups, textureEnemy, textureParachute)
    {
        _isArriving = false;
        _ignorePlatform = rand()%3 > 0 && pos.x < 140 ? true : false;
        _pointsReward = 142;
    };

    //Override methods
    using enemy::PerformEvents;
    void PerformEvents();
    using enemy::AnimeSprite;
    void AnimeSprite();
};
#endif
