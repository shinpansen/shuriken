#ifndef boss_hpp
#define boss_hpp

#include <math.h>
#include <string.h>
#include "enemy.hpp"

using namespace std;
using namespace sf;

class boss : public enemy
{
public:
    boss(Vector2i pos, b2World* world, effects* effectBox, projectile* projectileBox,
             sound* soundBox, scoreManager* scoreManager, powerups* powerups, Texture* textureEnemy,
             Texture* textureParachute) :
    enemy(Vector2i(60,50), Vector2i(64,58), Vector2i(26,31), 300, 4, ENEMY_BOSS,
          pos, world, effectBox, projectileBox, soundBox, scoreManager, powerups, textureEnemy, textureParachute)
    {
        _isArriving = false;
        _pointsReward = 9999;
        _movingSpeed = 0;
        _posToGo = 100;
        _animation = animation(&_sprite, 0.25, _tileWidth, _tileHeight, 4, 4, 0);

        //Hit box redefinition
        vector<Vector2f> vertices;
        vertices.push_back(Vector2f(10,-25));
        vertices.push_back(Vector2f(25,25));
        vertices.push_back(Vector2f(-25,25));
        vertices.push_back(Vector2f(-10,-25));
        _world->DestroyBody(_body);
        _body = Box2DTools::CreatePolygonBox(_world, pos, vertices, 0.1, 0.f, _maskType);
        _body->SetTransform(_body->GetPosition(), 0);

        //Initializing canon body
        _bodyCanon = Box2DTools::CreateRectangleBox(_world, pos, 18, 6, 0.1, 0.f, 0, _maskType);
    };

    void PerformPhase1();
    void PerformPhase2();
    void PerformPhase3();

    //Override methods
    using enemy::PerformEvents;
    void PerformEvents();
    using enemy::IsAnEnemy;
    bool IsAnEnemy(b2Body* bodyA, b2Body* bodyB);

private:
    float _movingSpeed;
    int _posToGo;
    b2Body* _bodyCanon;
};
#endif
