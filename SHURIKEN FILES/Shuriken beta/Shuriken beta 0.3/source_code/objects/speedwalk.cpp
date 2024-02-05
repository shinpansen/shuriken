#include "speedwalk.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

speedwalk::~speedwalk()
{
    _world->DestroyBody(_carpetBody);
    _world->DestroyBody(_leftWheelBody);
    _world->DestroyBody(_rightWheelBody);
    _carpetBody = NULL;
    _leftWheelBody = NULL;
    _rightWheelBody = NULL;
}

speedwalk::speedwalk(vector<hero*>* ninjas, Vector2i pos, int speed, int width, b2World* world,
                     Texture* textureWheel, Texture* textureCarpet)
{
    _speed = speed;
    _width = width <= 28 ? width : 28; //Number of blocks (20px)
    _ninjas = ninjas;
    _world = world;
    _initPos = pos;

    //Graphics
    _textureWheel = textureWheel;
    _textureCarpet = textureCarpet;

    //Wheels
    _wheelLeft.setTexture(*_textureWheel);
    _wheelRight.setTexture(*_textureWheel);
    _wheelLeft.setOrigin(42.f/2.f, 33.f/2.f);
    _wheelRight.setOrigin(42.f/2.f, 33.f/2.f);
    _wheelLeft.setPosition(pos.x-(width*30/2-3), pos.y);
    _wheelRight.setPosition(pos.x+(width*30/2-3), pos.y);
    _animationWheelLeft = animation(&_wheelLeft, (float)fabs(speed)/20.f, 42, 33, 4, 2, 0);
    _animationWheelRight = animation(&_wheelRight, (float)fabs(speed)/20.f, 42, 33, 4, 2, 0);
    _animationWheelLeft.SetAnimation(speed < 0 ? 0 : 1, 0);
    _animationWheelRight.SetAnimation(speed < 0 ? 1 : 0, 0);
    _animationWheelRight.ReverseSprite(true);

    //Carpets
    int posX = pos.x - ((float)width/2)*30 + 15;
    for(int i=0 ; i<width ; i++)
    {
        _carpets.push_back(new Sprite());
        _animationCarpets.push_back(new animation());
        _carpets[_carpets.size()-1]->setTexture(*_textureCarpet);
        _carpets[_carpets.size()-1]->setOrigin(30.f/2.f, 33.f/2.f);
        _carpets[_carpets.size()-1]->setPosition(posX + i*30, pos.y);
        *_animationCarpets[_animationCarpets.size()-1] =
            animation(_carpets[_carpets.size()-1], (float)fabs(speed)/5.5f, 30, 33, 10, 2, 0);
        _animationCarpets[_animationCarpets.size()-1]->SetAnimation(speed < 0 ? 1 : 0, 0);
    }

    //Box2D - Collision
    _carpetBody = Box2DTools::CreateRectangleBox(_world, pos, width*30, 33, 0.1, 0, 0, 9);
    _leftWheelBody = Box2DTools::CreateCircleBox(_world, Vector2i(pos.x-(width*30/2-3)-3,pos.y), 33, 0.1, 0, 9);
    _rightWheelBody = Box2DTools::CreateCircleBox(_world, Vector2i(pos.x+(width*30/2-3)+3,pos.y), 33, 0.1, 0, 9);
}

void speedwalk::Render()
{
    //Perform contact with hero
    for(int i=0 ; i<_ninjas->size() ; i++)
    {
        b2Body* ninjaBody = (*_ninjas)[i]->GetBodyA();
        if(ninjaBody == NULL || !(*_ninjas)[i]->IsOnTheFloor())
            continue;

        b2Vec2 ninjaPos = ninjaBody->GetPosition();
        b2Vec2 posLeft = b2Vec2(ninjaBody->GetPosition().x-(13/SCALE), ninjaBody->GetPosition().y+(20/SCALE));
        b2Vec2 posRight = b2Vec2(ninjaBody->GetPosition().x+(13/SCALE), ninjaBody->GetPosition().y+(20/SCALE));
        b2Vec2 posMiddle = b2Vec2(ninjaBody->GetPosition().x, ninjaBody->GetPosition().y+(20/SCALE));

        if(_carpetBody->GetFixtureList()[0].TestPoint(posLeft) ||
           _carpetBody->GetFixtureList()[0].TestPoint(posRight) ||
           _carpetBody->GetFixtureList()[0].TestPoint(posMiddle))
            ninjaBody->SetTransform(b2Vec2(ninjaPos.x+(float)_speed/2/SCALE, ninjaPos.y), 0);
        else if(_leftWheelBody->GetFixtureList()[0].TestPoint(posLeft) ||
           _leftWheelBody->GetFixtureList()[0].TestPoint(posRight) ||
           _leftWheelBody->GetFixtureList()[0].TestPoint(posMiddle) ||
           _rightWheelBody->GetFixtureList()[0].TestPoint(posLeft) ||
           _rightWheelBody->GetFixtureList()[0].TestPoint(posRight) ||
           _rightWheelBody->GetFixtureList()[0].TestPoint(posMiddle))
            ninjaBody->SetTransform(b2Vec2(ninjaPos.x+(float)_speed/SCALE, ninjaPos.y+0.1), 0);
    }

    //Animation
    if(_speed != 0)
    {
        _animationWheelLeft.PerformAnimation(1);
        _animationWheelRight.PerformAnimation(1);
        for(int i=0 ; i<_animationCarpets.size() ; i++)
            _animationCarpets[i]->PerformAnimation(1);
    }
}

void speedwalk::Display(RenderWindow* window)
{
    //Render
    if(_carpetBody != NULL && _leftWheelBody != NULL & _rightWheelBody != NULL)
        Render();

    //Draw
    for(int i=0 ; i<_carpets.size() ; i++)
        window->draw(*_carpets[i]);
    window->draw(_wheelLeft);
    window->draw(_wheelRight);
}
