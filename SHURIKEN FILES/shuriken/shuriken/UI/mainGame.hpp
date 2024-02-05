#ifndef mainGame_HPP
#define mainGame_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>

#include "../tools/Box2DTools.hpp"
#include "../tools/input.hpp"
#include "../tools/xmlReader.hpp"
#include "../tools/settings.hpp"
#include "../effects/effects.hpp"
#include "../effects/spells.hpp"
#include "../effects/sound.hpp"
#include "../characters/hero.hpp"
#include "../environment/maps.hpp"
#include "../environment/tree.hpp"
#include "../objects/ladder.hpp"
#include "menuList.hpp"
#include "scoreManager.hpp"

using namespace std;
using namespace sf;

enum gamestates
{
    intro,
    mainmenu,
    game,
    gameEnded,
    pause
};

class mainGame
{
public:
    mainGame(RenderWindow* window, settings* settings);
    bool ValidMenu(menuList* menu);
    void OpeningClosingMenu();
    void ManageNinjaCreation();
    void MainMenuEvents();
    void PauseMenuEvents();
    void RedrawUI();
    void ChangeScreenMode();
    void DisplayGameResources();
    bool IsMusicPaused();
    void PauseMusic(bool pause);
    void GoOn(Keyboard::Key key, bool keyPressed, float deltaTime);
private:
    //Variables
    gamestates _gameState;
    float _deltaTime;
    bool _menuKeyPressed;
    bool _globalKeyPressed;
    bool _waitingKey;
    bool _showMasks;
    Keyboard::Key _currentKey;

    //Objects
    settings* _settings;
    RenderWindow* _window;
    files _files;
    scoreManager _scoreManager;

    //Box2D
    b2World* _world;
    maps* _maps;

    //Effects - Spells
    effects _effectBox;
    sound _soundBox;
    spells* _spellBox;

    //Graphics
    Texture _titleScreenTitleTexture;
    Texture _treeTexture;
    Sprite _titleScreenTitle;

    //Inputs
    input _input0;
    input _input1;
    input _input2;
    input* _editingInput;
    vector<input*> _inputs;
    int _editingInputNum;

    //Wizards
    hero* _ninja1;
    hero* _ninja2;
    vector<hero*> _ninjas;

    //UI
    menuList* _mainMenu;
    menuList* _pauseMenu;

    //Texts
    Font _font;
};
#endif
