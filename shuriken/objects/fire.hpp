#ifndef fire_HPP
#define fire_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>

#include "../characters/hero.hpp"
#include "../tools/files.hpp"
#include "../tools/animation.hpp"
#include "../tools/Box2DTools.hpp"

using namespace std;
using namespace sf;

class fire
{
public:
    ~fire();
    fire(vector<hero*>* ninjas, Vector2i pos, b2World* world, Texture* textureFire);
    Vector2f GetPosition();
    FloatRect GetBoundingBox();
    void Render();
    void Display(RenderWindow* window);
private:
    //Variables - objects
    b2World* _world;
    b2Body* _body;
    b2Body* _bodyBottom;
    files _files;
    vector<hero*>* _ninjas;
    animation _animation;
    Vector2i _initPos;
    Vector2f _position;

    //Graphics
    Texture* _textureFire;
    Sprite _fire;
};
#endif
