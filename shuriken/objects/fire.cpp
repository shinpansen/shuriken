#include "fire.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

fire::~fire()
{
    _world->DestroyBody(_body);
    _world->DestroyBody(_bodyBottom);
    _body = NULL;
    _bodyBottom = NULL;
}

fire::fire(vector<hero*>* ninjas, Vector2i pos, b2World* world, Texture* textureFire)
{
    _ninjas = ninjas;
    _world = world;
    _body = Box2DTools::CreateRectangleBox(_world, pos, 35, 28, 0.1, 0, 0, 8);
    _bodyBottom = Box2DTools::CreateRectangleBox(_world, Vector2i(pos.x, pos.y+27), 30, 33, 0.1, 0, 0, 1);
    _initPos = pos;
    _position = Vector2f(pos.x, pos.y);

    //Graphics
    _textureFire = textureFire;
    _fire.setTexture(*_textureFire);
    _fire.setOrigin(27, 54);
    _animation = animation(&_fire, 0.1, 54, 75, 3, 1, 0);
    _animation.SetAnimation(0, 0);
}

Vector2f fire::GetPosition()
{
    return _fire.getPosition();
}

FloatRect fire::GetBoundingBox()
{
    _fire.getLocalBounds();
}

void fire::Render()
{
    //Perform contact with hero
    for(int i=0 ; i<_ninjas->size() ; i++)
    {
        b2Body* ninjaBody = (*_ninjas)[i]->GetBodyA();
        if(ninjaBody == NULL)
            continue;

        b2Vec2 posLeft = b2Vec2(ninjaBody->GetPosition().x-(13/SCALE), ninjaBody->GetPosition().y);
        b2Vec2 posRight = b2Vec2(ninjaBody->GetPosition().x+(13/SCALE), ninjaBody->GetPosition().y);
        b2Vec2 posTop = b2Vec2(ninjaBody->GetPosition().x, ninjaBody->GetPosition().y-(25/SCALE));
        b2Vec2 posBottom = b2Vec2(ninjaBody->GetPosition().x, ninjaBody->GetPosition().y+(25/SCALE));

        if(_body->GetFixtureList()[0].TestPoint(ninjaBody->GetPosition()) ||
           _body->GetFixtureList()[0].TestPoint(posLeft) ||
           _body->GetFixtureList()[0].TestPoint(posRight) ||
           _body->GetFixtureList()[0].TestPoint(posTop) ||
           _body->GetFixtureList()[0].TestPoint(posBottom))
        {
            int signe = ninjaBody->GetPosition().x > _body->GetPosition().x > 0 ? 1 : -1;
            (*_ninjas)[i]->SetStunned(Vector2f(signe*10.f, -8.f), 10);
        }
    }

    //Animation - Position
    if(_body != NULL)
        _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
    _animation.PerformAnimation(1);
}

void fire::Display(RenderWindow* window)
{
    if(_body != NULL)
        Render();
    _fire.setPosition(_position.x, _position.y+24);
    window->draw(_fire);
}
