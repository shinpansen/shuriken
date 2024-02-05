#ifndef arrowBlock_HPP
#define arrowBlock_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>

#include "../characters/hero.hpp"
#include "../tools/files.hpp"
#include "../tools/Box2DTools.hpp"

using namespace std;
using namespace sf;

enum ARROW_TYPE
{
    ARROW_TELEPORT,
    ARROW_SUPERJUMP
};

class arrowBlock
{
public:
    arrowBlock(vector<hero*>* ninjas, Vector2i pos, b2World* world, Texture* textureArrowBlock, ARROW_TYPE type);
    void Render(bool disableAction);
    void Display(RenderWindow* window);
private:
    //Variables - objects
    ARROW_TYPE _type;
    b2World* _world;
    files _files;
    vector<hero*>* _ninjas;
    animation _animation;
    Vector2i _initPos;
    Vector2f _position;

    //Graphics
    Texture* _textureArrowBlock;
    Sprite _arrowBlock;
};
#endif
