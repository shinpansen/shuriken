#include "blueTank.hpp"

static const float GRAVITY = 23.f;

using namespace std;
using namespace sf;

void blueTank::PerformEvents()
{
    if(_life == 0 || _body == NULL || _bodyClone == NULL)
        return;

    //Parameters
    float enemySpeed = 0.6;
    float projectileSpeed = 1.6;
    int projectileType = 2;
    int xAdjust = _animation.GetReversed() ? -1 : 0;
    Vector2i projectilePos = Vector2i(18,1);

    //Arriving
    if(_isArriving && !_onTheFloor)
    {
        //Low gravity with parachute
        _body->ApplyForce(b2Vec2(0.f,-GRAVITY*0.95*_body->GetMass()), _body->GetWorldCenter(), false);
        _parachute.setPosition(_position.x+xAdjust, _position.y-6);
    }
    else
        _isArriving = false;

    //Moving
    if(_hit < 2)
    {
        float movingSpeed = !_animation.GetReversed() ? enemySpeed : -enemySpeed;
        _body->SetLinearVelocity(b2Vec2((!_isArriving && _turning == 0)*movingSpeed, _body->GetLinearVelocity().y));
    }

    //Attacking
    int signe = !_animation.GetReversed() ? 1 : -1;
    if(_attackingCooldown == 0 && _turning == 0)
    {
        _attacking = 20;
        _attackingCooldown = 360;
        int angle = _animation.GetReversed()*180;
        Vector2i projectilePosFinal = Vector2i(_position.x+(signe*projectilePos.x), _position.y-projectilePos.y);
        _projectileBox->AddProjectile(projectilePosFinal, 0.2, 13, 13, 4, 2, angle, Vector2f(signe*projectileSpeed,0.f), projectileType);
        _soundBox->PlaySound(SND_SHOOT2, false);
    }
}
