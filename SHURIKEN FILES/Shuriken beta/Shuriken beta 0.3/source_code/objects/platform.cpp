#include "platform.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

platform::~platform()
{
    _world->DestroyBody(_body);
    _body = NULL;
}

platform::platform(Vector2i pos, Vector2i direction, Vector2i maxY, b2World* world, Texture* texturePlatform, bool enemySensor)
{
    _world = world;
    _body = Box2DTools::CreateRectangleBox(_world, pos, 96, 23, 0.1, 0, 0, enemySensor ? 2 : 11);
    _initPos = pos;
    _direction = direction;
    _maxY = maxY;

    //Graphics
    _texturePlatform = texturePlatform;
    _platform.setTexture(*_texturePlatform);
    _platform.setOrigin(_platform.getLocalBounds().width/2, 0);
}

Vector2f platform::GetPosition()
{
    return _platform.getPosition();
}

FloatRect platform::GetBoundingBox()
{
    _platform.getLocalBounds();
}

void platform::Render()
{
    if(_body == NULL)
        return;

    _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
    _body->SetLinearVelocity(b2Vec2(_direction.x*2.5f, _direction.y*2.5f));
    int resetPos = _direction.y < 0 ? _maxY.x : _maxY.y;
    if(_position.y > _maxY.x || _position.y < _maxY.y)
        _body->SetTransform(b2Vec2(_initPos.x/SCALE, resetPos/SCALE), 0);
}

void platform::Display(RenderWindow* window)
{
    Render();
    _platform.setPosition(_position.x, _position.y-14);
    window->draw(_platform);
}
