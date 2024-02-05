#ifndef mainGame_HPP
#define mainGame_HPP

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <Box2D/Box2D.h>

#include "../tools/Box2DTools.hpp"
#include "../tools/input.hpp"
#include "../tools/xmlReader.hpp"
#include "../tools/settings.hpp"
#include "../effects/effects.hpp"
#include "../effects/projectile.hpp"
#include "../effects/sound.hpp"
#include "../characters/hero.hpp"
#include "../characters/enemy.hpp"
#include "../environment/maps.hpp"
#include "../objects/powerups.hpp"
#include "../objects/platform.hpp"
#include "../objects/fire.hpp"
#include "../objects/speedwalk.hpp"
#include "menuList.hpp"
#include "scoreManager.hpp"

using namespace std;
using namespace sf;

enum gamestates
{
    intro,
    mainmenu,
    level,
    game,
    pause,
    gameOver,
    score,
    leaderboard
};

class mainGame
{
public:
    mainGame(RenderWindow* window, View* view, settings* settings);
    bool ValidMenu(menuList* menu);
    void OpeningClosingMenu();
    void ResetRoom();
    void MainMenuEvents();
    void PauseMenuEvents();
    void RedrawUI();
    void SendScoreOnline();
    string GetScoreOnline(int i);
    void ChangeScreenMode();
    void DisplayGameResources();
    void DisplayLeaderboard();
    void GoOn(Keyboard::Key key, bool keyPressed, float deltaTime);
private:
    //Variables
    gamestates _gameState;
    int _endGameTimer;
    int _gameOverTimer;
    float _deltaTime;
    bool _menuKeyPressed;
    bool _globalKeyPressed;
    bool _waitingKey;
    bool _showMasks;
    string _nickname;
    string _scoresOnline[20];
    int _scoreToLoad;
    int _currentLevel;
    int _levelTimer;
    int _victoryOrDefeat;
    Keyboard::Key _currentKey;

    //Objects
    settings* _settings;
    RenderWindow* _window;
    View* _view;
    files _files;
    scoreManager _scoreManager;
    powerups* _powerups;

    //Box2D
    b2World* _world;
    maps* _maps;

    //Effects - Spells
    effects _effectBox;
    sound _soundBox;
    Sound* _levelMusic;
    projectile* _projectileBox;

    //Graphics
    Texture _titleScreenTitleTexture;
    Texture _treeTexture;
    Sprite _titleScreenTitle;
    RectangleShape _squareShape;

    //Inputs
    input _input0;
    input _input1;
    input _input2;
    input* _editingInput;
    vector<input*> _inputs;
    int _editingInputNum;

    //Ninjas
    hero* _ninja1;
    hero* _ninja2;
    vector<hero*> _ninjas;

    //Enemies
    vector<enemy*> _enemies;

    //UI
    menuList* _mainMenu;
    menuList* _pauseMenu;

    //Texts
    Font _font;
    Text _gameOverText;
    Text _scoreEnterText;
    Text _nicknameText;
    Text _leaderBoardText;
};
#endif
