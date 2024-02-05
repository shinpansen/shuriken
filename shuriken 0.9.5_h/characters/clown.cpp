#include "clown.hpp"

static const float SCALE = 30.f;
static const float GRAVITY = 23.f;
static const int JUMPING_TIMER = 160;

using namespace std;
using namespace sf;

void clown::PerformEvents()
{
    if(_life == 0 || _body == NULL || _bodyClone == NULL)
        return;

    //Parameters
    int enemySpeed = !_onTheFloor ? 1.6 : 0;
    int xAdjust = _animation.GetReversed() ? -1 : 0;

    //Bug correction : stuck between box2D bodies
    if((int)_position.y == 42 && ((int)_position.x == 133 || (int)_position.x == 163))
        _body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y-(1.f/SCALE)), 0);

    //Moving
    if(_hit < 2)
    {
        float movingSpeed = !_animation.GetReversed() ? enemySpeed : -enemySpeed;
        _body->SetLinearVelocity(b2Vec2((!_isArriving && _turning == 0)*movingSpeed, _body->GetLinearVelocity().y));
    }

    //Jumping out of platform (LEVEL 2)
    if(_collisionType == 2 && (_position.y < 65 || (_ignorePlatform && !_animation.GetReversed() && _position.y > 150)))
        _jumpingTimer = 0;

    //Jumping timer
    _jumpingTimer -= _jumpingTimer > 0 ? 1 : 0;
    if(_jumpingTimer == 0 && (_collisionType != 2 || _position.y < 65 || (_ignorePlatform && !_animation.GetReversed() && _position.y > 150)))
    {
        _jumpingTimer = JUMPING_TIMER;
        _body->SetLinearVelocity(b2Vec2(0, -6));
        _soundBox->PlaySound(SND_BOUNCE, false);
    }
}

void clown::AnimeSprite()
{
    if(_life == 0 || _body == NULL || _bodyClone == NULL)
        return;

    //Perform animation + position + drawing
    if(_hit != 0)
        _animation.SetAnimation(_onTheFloor ? 2 : 3, 0);
    else if(!_onTheFloor)
         _animation.SetAnimation(1, 0);
    else
        _animation.SetAnimation(0, 0);

    _animation.PerformAnimation(_deltaTime);
    _sprite.setPosition(_position.x, _position.y);
}
