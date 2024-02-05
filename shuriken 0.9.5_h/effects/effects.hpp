#ifndef effects_hpp
#define effects_hpp

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <math.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>
#include "../tools/animation.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/files.hpp"
#include "../tools/pixelFont.hpp"
#include "../tools/utils.hpp"

using namespace std;
using namespace sf;

enum SFX
{
    SFX_EXPLOSION,
    SFX_EXPLOSION2,
    SFX_TELEPORT
};

class effects
{
public:
    effects();
    Sprite* AddEffect(Vector2i pos, float frameDelay, int width, int height, int nbFrames,
                   int numAnimation, bool reversed, SFX textureId = SFX_EXPLOSION, bool aoe = false);
    void AddText(string text, Vector2f pos, Vector2f speed, int frames);
    void ShakeScreen(float force);
    int InAOE(Vector2f pt1, Vector2f pt2, Vector2f pt3, Vector2f pt4, Vector2f center);
    void RenderEffects(b2World* world, RenderWindow* window, View* view);
    void Display(RenderWindow* window);
private:
    //Variables
    int _shakeFrame;
    Vector2f _shakePositions[14];

    //Vectors
    vector<Sprite*> _tiles;
    vector<animation*> _animations;
    vector<pixelFont*> _texts;
    vector<Vector2f> _textSpeed;
    vector<int> _textFrames;
    vector<CircleShape*> _areasOfEffect;
    vector<int> _aoeDuration;

    //Objects
    files _files;

    //Graphics - fonts
    Texture _explosionsTexture;
    Texture _explosions2Texture;
    Texture _teleportEffectTexture;
};
#endif
