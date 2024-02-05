#include "bomb.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

bomb::~bomb()
{
    _world->DestroyBody(_body);
    _body = NULL;
}

bomb::bomb(Vector2i pos, b2World* world, effects* effectBox, Texture* textureBomb)
{
    _timer = 175;
    _world = world;
    _effectBox = effectBox;
    _body = Box2DTools::CreateCircleBox(_world, pos, 20, 0.1, 0, 5);
    _position = Vector2f(pos.x, pos.y);

    //Graphics
    _textureBomb.loadFromFile(_files.bomb); //= textureBomb;
    _bomb.setTexture(_textureBomb);
    _bomb.setOrigin(10, 18);
    _animation = animation(&_bomb, 0.17, 20, 30, 4, 3, 0);
    _animation.SetAnimation(0, 0);
}

Vector2f bomb::GetPosition()
{
    return _bomb.getPosition();
}

FloatRect bomb::GetBoundingBox()
{
    _bomb.getLocalBounds();
}

bool bomb::HasExploded()
{
    return _timer <= 0;
}

void bomb::Render()
{
    //Timer
    _timer -= _timer > 0 ? 1 : 0;

    //Animation
    if(_timer == 120)
        _animation.SetAnimation(1, 0);
    else if(_timer == 50)
        _animation.SetAnimation(2, 0);
    _animation.PerformAnimation(1);

    //Position
    if(_body != NULL)
        _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);

    //Explosion
    if(_timer <= 0)
        _effectBox->AddEffect(Vector2i(_position.x, _position.y-2), 0.2, 46, 27, 6, 0, false, SFX_EXPLOSION2, true);
}

void bomb::Display(RenderWindow* window)
{
    if(_body == NULL || _timer <= 0)
        return;

    Render();
    _bomb.setPosition(_position.x, _position.y);
    window->draw(_bomb);
}
