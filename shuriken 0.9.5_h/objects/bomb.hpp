#ifndef bomb_HPP
#define bomb_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>

#include "../tools/files.hpp"
#include "../tools/animation.hpp"
#include "../tools/Box2DTools.hpp"
#include "../effects/effects.hpp"

using namespace std;
using namespace sf;

class bomb
{
public:
    ~bomb();
    bomb(Vector2i pos, b2World* world, effects* effectBox, Texture* textureBomb);
    Vector2f GetPosition();
    FloatRect GetBoundingBox();
    bool HasExploded();
    void Render();
    void Display(RenderWindow* window);
private:
    //Variables - objects
    int _timer;
    b2World* _world;
    b2Body* _body;
    files _files;
    animation _animation;
    effects* _effectBox;
    Vector2f _position;

    //Graphics
    Texture _textureBomb;
    Sprite _bomb;
};
#endif
