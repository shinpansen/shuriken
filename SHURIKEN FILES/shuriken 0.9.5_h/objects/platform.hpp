#ifndef platform_HPP
#define platform_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>

#include "../tools/files.hpp"
#include "../tools/Box2DTools.hpp"

using namespace std;
using namespace sf;

class platform
{
public:
    ~platform();
    platform(Vector2i pos, Vector2i direction, Vector2i maxY, b2World* world, Texture* texturePlatform, bool enemySensor);
    Vector2f GetPosition();
    FloatRect GetBoundingBox();
    void Render();
    void Display(RenderWindow* window);
private:
    //Variables - objects
    b2World* _world;
    b2Body* _body;
    files _files;
    Vector2i _initPos;
    Vector2f _position;
    Vector2i _direction;
    Vector2i _maxY;

    //Graphics
    Texture* _texturePlatform;
    Sprite _platform;
};
#endif
