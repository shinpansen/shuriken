#ifndef blueTank_hpp
#define blueTank_hpp

#include <math.h>
#include <string.h>
#include "enemy.hpp"

using namespace std;
using namespace sf;

class blueTank : public enemy
{
public:
    blueTank(Vector2i pos, b2World* world, effects* effectBox, projectile* projectileBox,
             sound* soundBox, scoreManager* scoreManager, powerups* powerups, Texture* textureEnemy,
             Texture* textureParachute) :
    enemy(Vector2i(14,10), Vector2i(32,18), Vector2i(14,11), 6, 4, ENEMY_BLUE_TANK,
          pos, world, effectBox, projectileBox, soundBox, scoreManager, powerups, textureEnemy, textureParachute)
    {
        _pointsReward = 50;
    };

    //Override methods
    using enemy::PerformEvents;
    void PerformEvents();
};
#endif
