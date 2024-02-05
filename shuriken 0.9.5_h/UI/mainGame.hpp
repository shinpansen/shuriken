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
#include "../tools/utils.hpp"
#include "../tools/pixelFont.hpp"
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
#include "../objects/bomb.hpp"
#include "menuList.hpp"
#include "scoreManager.hpp"

using namespace std;
using namespace sf;

enum gamestates
{
    intro,
    mainmenu,
    setting,
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
    mainGame(RenderWindow* window, View* view, settings* settings, Thread* thread, string** scoresString);
    void ResetRoom();
    bool ValidMenu(menuList* menu);
    void OpeningClosingMenu();
    void MainMenuEvents();
    void SettingsMenuEvents();
    void PauseMenuEvents();
    void RedrawUI();
    void SendScoreOnline();
    void ChangeScreenMode();
    void PauseMusic(bool pause);
    void DisplayGameResources();
    void DisplayLeaderboard();
    void GoOn(Keyboard::Key key, bool keyPressed, float deltaTime);
private:
    //Variables
    gamestates _gameState;
    gamestates _lastGameState;
    int _endGameTimer;
    int _gameOverTimer;
    int _lastScore;
    int _currentLevel;
    int _levelTimer;
    int _victoryOrDefeat;
    int _maskKeyDuration;
    int _editingInputNum;
    float _deltaTime;
    bool _menuKeyPressed;
    bool _globalKeyPressed;
    bool _waitingKey;
    bool _showMasks;
    string _nickname;
    string** _scoresString;
    Keyboard::Key _currentKey;

    //Objects
    settings* _settings;
    Thread* _thread;
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
    Texture _textureTitleScreenTitle;
    Texture _textureSettingsFrame;
    Texture _texturePauseFrame;
    Sprite _titleScreenTitle;
    Sprite _settingsFrame;
    Sprite _pauseFrame;
    RectangleShape _squareShape;

    //Inputs
    input _input0;
    input _input1;
    input _input2;
    input* _editingInput;
    vector<input*> _inputs;

    //Ninjas
    hero* _ninja1;
    hero* _ninja2;
    vector<hero*> _ninjas;

    //Enemies
    vector<enemy*> _enemies;

    //UI
    menuList* _mainMenu;
    menuList* _settingsMenu;
    menuList* _pauseMenu;

    //Texts
    pixelFont _gameOverText;
    pixelFont _scoreEnterText;
    pixelFont _nicknameText;
    pixelFont _menuTitleText;
    pixelFont _settingsValue;
    pixelFont _settingsLeftArrow;
    pixelFont _settingsRightArrow;
    pixelFont _settingsBackArrow;
    pixelFont _settingsEditText;
    pixelFont _actionPadTexts[4];

    bomb* _bombTest;
};
#endif
