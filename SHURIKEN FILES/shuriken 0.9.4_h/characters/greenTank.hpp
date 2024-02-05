#ifndef greenTank_hpp
#define greenTank_hpp

#include <math.h>
#include <string.h>
#include "enemy.hpp"

using namespace std;
using namespace sf;

class greenTank : public enemy
{
public:
    greenTank(Vector2i pos, b2World* world, effects* effectBox, projectile* projectileBox,
             sound* soundBox, scoreManager* scoreManager, powerups* powerups, Texture* textureEnemy,
             Texture* textureParachute) :
    enemy(Vector2i(22,12), Vector2i(32,18), Vector2i(14,10), 10, 4, ENEMY_GREEN_TANK,
          pos, world, effectBox, projectileBox, soundBox, scoreManager, powerups, textureEnemy, textureParachute)
    {
        _pointsReward = 100;
    };

    //Override methods
    using enemy::PerformEvents;
    void PerformEvents();
};
#endif
