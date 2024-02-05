#include "boss.hpp"

static const float SCALE = 30.f;
static const float FALLING_SPEED = 7.f;
static const float GRAVITY = 23.f;

using namespace std;
using namespace sf;

void boss::PerformEvents()
{
    if(_life == 0 || _body == NULL || _bodyClone == NULL)
        return;

    //Parameters
    /*float enemySpeed = 0.93;
    float projectileSpeed = 1.6;
    int projectileType = 2;
    int xAdjust = _animation.GetReversed() ? -1 : 0;
    Vector2i projectilePos = Vector2i(18,1);

    //Moving
    if(_hit < 2)
    {
        float movingSpeed = !_animation.GetReversed() ? enemySpeed : -enemySpeed;
        _body->SetLinearVelocity(b2Vec2((!_isArriving && _turning == 0)*movingSpeed, _body->GetLinearVelocity().y));
    }

    //Changing direction
    int collisionType;
    float spaceInFront = _animation.GetReversed() ? -28.f/SCALE : 28.f/SCALE;
    b2Vec2 testPoint = b2Vec2(_body->GetPosition().x+spaceInFront, _body->GetPosition().y);
    if(Box2DTools::TestCollisionPoint(_world, testPoint, false, &collisionType) && collisionType == 1)
        _animation.ReverseSprite(!_animation.GetReversed());*/

    //Moving canon body
    float spaceInFront = _animation.GetReversed() ? -23.f/SCALE : 23.f/SCALE;
    _bodyCanon->SetTransform(b2Vec2(_body->GetPosition().x+spaceInFront, _body->GetPosition().y-18/SCALE), 0);

    //Fight phases
    if(_life > 200)
        PerformPhase1();
    else if(_life > 100)
        PerformPhase2();
    else
        PerformPhase3();
}

void boss::PerformPhase1()
{
    //Moving
    if(fabs(_position.x - _posToGo) > 1)
        _movingSpeed = _position.x < _posToGo ? 0.25 : -0.25;
    else
        _posToGo = _posToGo <= 70 ? 75 + rand()%25 : 35 + rand()%25;
    _body->SetLinearVelocity(b2Vec2(_movingSpeed, _body->GetLinearVelocity().y));
}

void boss::PerformPhase2()
{
}

void boss::PerformPhase3()
{
}

bool boss::IsAnEnemy(b2Body* bodyA, b2Body* bodyB)
{
    return _body != NULL && _bodyClone != NULL && _bodyCanon != NULL &&
           (bodyA == _body || bodyB == _body ||
            bodyA == _bodyClone || bodyB == _bodyClone ||
            bodyA == _bodyCanon || bodyB == _bodyCanon);
}
