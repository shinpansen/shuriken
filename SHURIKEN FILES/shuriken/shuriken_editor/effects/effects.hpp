#ifndef effects_hpp
#define effects_hpp

#include <iostream>
#include <math.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../tools/animation.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/files.hpp"

using namespace std;
using namespace sf;

class effects
{
public:
    effects();
    Sprite* AddEffect(Vector2i pos, float frameDelay, int width, int height, int nbFrames,
                   int numAnimation, bool reversed, int textureId = 0);
    void AddParticles(b2World* world, Color color, Vector2i pos, int minRand, int maxRand);
    void AddStar(Color color, Vector2f pos, int number);
    void RenderEffects(b2World* world, RenderWindow* window);
    void Display(RenderWindow* window);
private:
    files _files;
    vector<Sprite*> _tiles;
    vector<animation*> _animations;
    vector<RectangleShape*> _particles;
    vector<CircleShape*> _stars;
    vector<b2Body*> _particlesBodies;
    Texture _dustTexture;
    Texture _spellsTexture;
    Texture _wizardDyingTexture;
};
#endif
