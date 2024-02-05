#ifndef redTank_hpp
#define redTank_hpp

#include <math.h>
#include <string.h>
#include "enemy.hpp"

using namespace std;
using namespace sf;

class redTank : public enemy
{
public:
    redTank(Vector2i pos, b2World* world, effects* effectBox, projectile* projectileBox,
             sound* soundBox, scoreManager* scoreManager, powerups* powerups, Texture* textureEnemy,
             Texture* textureParachute) :
    enemy(Vector2i(26,15), Vector2i(38,23), Vector2i(17,14), 15, 4, ENEMY_RED_TANK,
          pos, world, effectBox, projectileBox, soundBox, scoreManager, powerups, textureEnemy, textureParachute)
    {
        _isArriving = false;
        _pointsReward = 360;
    };

    //Override methods
    using enemy::PerformEvents;
    void PerformEvents();
};
#endif
