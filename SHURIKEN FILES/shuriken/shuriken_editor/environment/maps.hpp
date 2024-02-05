#ifndef maps_HPP
#define maps_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "tree.hpp"
#include "../tools/files.hpp"
#include "../tools/Box2DTools.hpp"

using namespace std;
using namespace sf;

class maps
{
public:
    maps();
    maps(b2World* world, effects* effectBox);
    void CreateLayers();
    void CreateCollisions();
    void RenderMap(RenderWindow* window, int mode);

    //Resources
    Texture _layer1;
    Texture _treeTexture;
    vector<tree*> _treeList;
    vector<Sprite*> _layer0Tiles;
    vector<Sprite*> _layer1Tiles;
private:
    //Objects
    b2World* _world;
    effects* _effectBox;
    files _files;
};
#endif
