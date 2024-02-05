#ifndef blocker_HPP
#define blocker_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>

#include "../characters/hero.hpp"
#include "../effects/effects.hpp"
#include "../effects/sound.hpp"
#include "../tools/files.hpp"
#include "../tools/animation.hpp"
#include "../tools/Box2DTools.hpp"

using namespace std;
using namespace sf;

class blocker
{
public:
    ~blocker();
    blocker(vector<hero*>* ninjas, effects* effectBox, sound* soundBox, b2World* world,
            Vector2f pinkSwitchPos, Vector2f blueSwitchPos, Vector2f pinkBlockerPos, Vector2f blueBlockerPos,
            Texture* textureSwitch, Texture* textureBlocker, Texture* textureWarning);
    void Render();
    void PerformContact();
    void Display(RenderWindow* window);
private:
    //Variables
    bool _pinkPressed;
    bool _bluePressed;
    int _warningTimer;
    int _switchDisabled;
    int _bubbleTransitionFrame;
    int* _bubbleTransitions;
    Vector2i _pinkBlockerPos;
    Vector2i _blueBlockerPos;

    //Objects
    b2World* _world;
    b2Body* _pinkBody;
    b2Body* _blueBody;
    files _files;
    effects* _effectBox;
    sound* _soundBox;
    vector<hero*>* _ninjas;

    //Graphics
    Texture* _textureSwitch;
    Texture* _textureBlocker;
    Texture* _textureWarning;
    Sprite _pinkSwitch;
    Sprite _blueSwitch;
    Sprite _pinkBlocker;
    Sprite _blueBlocker;
    Sprite _pinkWarning;
    Sprite _blueWarning;
};
#endif
