#ifndef powerups_HPP
#define powerups_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>

#include "../tools/files.hpp"
#include "../characters/hero.hpp"
#include "../effects/effects.hpp"
#include "../effects/sound.hpp"

using namespace std;
using namespace sf;

class hero;
class powerups
{
public:
    powerups(vector<hero*>* ninjas, effects* effectBox, sound* soundBox, b2World* world);
    void AddPowerUp(Vector2f pos, int type);
    void AddRandomPowerup(Vector2f pos);
    void DeletePowerUp(int i);
    void DeleteAllPowerUps();
    void Render();
    void Display(RenderWindow* window);
private:
    //Variables - objects
    b2World* _world;
    files _files;
    vector<hero*>* _ninjas;
    effects* _effectBox;
    sound* _soundBox;

    //Graphics
    Texture _texturePowerUps;
    vector<Sprite*> _powerups;
    vector<int> _powerupsTimer;
};
#endif
