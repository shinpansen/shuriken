#include "arrowBlock.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

arrowBlock::arrowBlock(vector<hero*>* ninjas, Vector2i pos, b2World* world, Texture* textureArrowBlock, ARROW_TYPE type)
{
    _type = type;
    _ninjas = ninjas;
    _world = world;
    _initPos = pos;
    _position = Vector2f(pos.x, pos.y);

    //Graphics
    _textureArrowBlock = textureArrowBlock;
    _arrowBlock.setTexture(*_textureArrowBlock);
    _arrowBlock.setOrigin(5, 5);
    _arrowBlock.setPosition(pos.x, pos.y);
}

void arrowBlock::Render(bool disableAction)
{
    //Texture Rect
    if(_type == ARROW_TELEPORT)
        _arrowBlock.setTextureRect(IntRect(0,0,10,10));
    else if(_type == ARROW_SUPERJUMP)
        _arrowBlock.setTextureRect(IntRect(0,10,10,10));

    //Perform contact with hero
    for(int i=0 ; i<_ninjas->size() ; i++)
    {
        if(disableAction)
        {
            (*_ninjas)[i]->_controlKey = HELP_NONE;
            if(_type == ARROW_TELEPORT)
                (*_ninjas)[i]->_canTeleport = false;
            else if(_type == ARROW_SUPERJUMP)
                (*_ninjas)[i]->_canSuperJump = false;
        }

        b2Body* ninjaBody = (*_ninjas)[i]->GetBodyA();
        if(ninjaBody == NULL)
            continue;

        b2Vec2 posLeft = b2Vec2((_arrowBlock.getPosition().x-3.f)/SCALE, (_arrowBlock.getPosition().y-8.f)/SCALE);
        b2Vec2 posRight = b2Vec2((_arrowBlock.getPosition().x+3.f)/SCALE, (_arrowBlock.getPosition().y-8.f)/SCALE);
        b2Vec2 posMiddle = b2Vec2(_arrowBlock.getPosition().x/SCALE, (_arrowBlock.getPosition().y-8.f)/SCALE);

        if(ninjaBody->GetFixtureList()[0].TestPoint(posLeft) ||
           ninjaBody->GetFixtureList()[0].TestPoint(posRight) ||
           ninjaBody->GetFixtureList()[0].TestPoint(posMiddle))
        {
            (*_ninjas)[i]->_teleportPos = Vector2f(_arrowBlock.getPosition().x, _arrowBlock.getPosition().y+4);
            if(_type == ARROW_TELEPORT)
            {
                (*_ninjas)[i]->_controlKey = HELP_DOWN;
                (*_ninjas)[i]->_canTeleport = true;
                _arrowBlock.setTextureRect(IntRect(10,0,10,10));
            }
            else if(_type == ARROW_SUPERJUMP)
            {
                (*_ninjas)[i]->_controlKey = HELP_UP;
                (*_ninjas)[i]->_canSuperJump = true;
                _arrowBlock.setTextureRect(IntRect(10,10,10,10));
            }
            break;
        }
    }
}

void arrowBlock::Display(RenderWindow* window)
{
    window->draw(_arrowBlock);
}
