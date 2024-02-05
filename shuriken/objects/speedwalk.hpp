#ifndef speedwalk_HPP
#define speedwalk_HPP

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

class speedwalk
{
public:
    ~speedwalk();
    speedwalk(vector<hero*>* ninjas, Vector2i pos, int speed, int width, b2World* world,
              Texture* textureWheel, Texture* textureCarpet);
    void Render();
    void Display(RenderWindow* window);
private:
    //Variables
    int _speed;
    int _width;
    Vector2i _initPos;

    //Objects
    b2World* _world;
    b2Body* _carpetBody;
    b2Body* _leftWheelBody;
    b2Body* _rightWheelBody;
    files _files;
    vector<hero*>* _ninjas;
    animation _animationWheelLeft;
    animation _animationWheelRight;
    vector<animation*> _animationCarpets;

    //Graphics
    Texture* _textureWheel;
    Texture* _textureCarpet;
    Sprite _wheelLeft;
    Sprite _wheelRight;
    vector<Sprite*> _carpets;
};
#endif
