#include "mainGame.hpp"

using namespace std;
using namespace sf;

mainGame::mainGame(RenderWindow* window, settings* settings)
{
    //Variables
    _gameState = mainmenu;
    _deltaTime = 1;
    _menuKeyPressed = false;
    _globalKeyPressed = false;
    _waitingKey = false;
    _showMasks = false;
    _editingInputNum = 0;

    //Objects
    _settings = settings;
    _window = window;
    _soundBox.UpdateVolumes(_settings);

    //Box2D - World
    _world = new b2World(b2Vec2(0.f, 42.f), false);
    _maps = new maps(_world, &_effectBox);

    //Spells - Music
    _spellBox = new spells(_world, &_effectBox, &_soundBox);

    //Graphics
    _titleScreenTitleTexture.loadFromFile(_files.titleScreenTitle);
    _titleScreenTitle.setTexture(_titleScreenTitleTexture);
    //_titleScreenTitle.setOrigin(_titleScreenTitle.getLocalBounds().width/2, _titleScreenTitle.getLocalBounds().height/2);
    _titleScreenTitle.setPosition(0,0);

    //Inputs
    _input0 = input(-1, 30, _window); //Menu input
    _input1 = input(0, 30, _window);
    _input2 = input(1, 30, _window);

    //Wizards
    _ninja1 = new hero(0, Vector2i(185,145), _world, &_input1, &_effectBox, _spellBox, &_soundBox, &_scoreManager, 1);
    _ninja2 = new hero(1, Vector2i(615,145), _world, &_input2, &_effectBox, _spellBox, &_soundBox, &_scoreManager, 1);
    _ninjas.push_back(_ninja1);
    _ninjas.push_back(_ninja2);
    _spellBox->UpdateWizardsPointers(&_ninjas);
    _ninja1->_ninjas = _ninjas;
    _ninja2->_ninjas = _ninjas;

    //UI
    _mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->_language), Vector2f(420,425), &_input0);
    //_pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->_language), Vector2f(420,300), &_input0);
    _scoreManager.Reset(_settings->_timeInSec,_settings->_language);

    //UI SELECTOR
    _inputs.push_back(&_input1);
    _inputs.push_back(&_input2);

    //Texts
    _font.loadFromFile(_files.font2);
}

bool mainGame::ValidMenu(menuList* menu)
{
    if((_input0.isKeyPressed("VALID") ||
       (Mouse::isButtonPressed(Mouse::Left) && menu->mouseOnSelectedText(_window))) &&
       menu->GetSelectedItem() >= 0)
       return true;
    else
        return false;
}

void mainGame::OpeningClosingMenu()
{
    switch(_gameState)
    {
        case intro:
            if(_input0.isKeyPressed("VALID") || Mouse::isButtonPressed(Mouse::Left))
               _gameState = mainmenu;
            break;
        case game:
            if(_input0.isKeyPressed("PAUSE") && !_menuKeyPressed)
            {
                _menuKeyPressed = true;
                _gameState = pause;
            }
            break;
        case gameEnded:
            break;
        case pause:
            if((_input0.isKeyPressed("BACK") || _input0.isKeyPressed("PAUSE")) && !_menuKeyPressed)
            {
                _pauseMenu->ResetAnimation();
                _menuKeyPressed = true;
                _gameState = game;
            }
            break;
    }
    if(!Keyboard::isKeyPressed(Keyboard::Num0) &&
       !_input0.isKeyPressed("BACK") && !_input0.isKeyPressed("PAUSE") &&
       !_input0.isKeyPressed("VALID") && !Mouse::isButtonPressed(Mouse::Left) &&
       !_input1.isKeyPressed("ROLL") && !_input2.isKeyPressed("ROLL") &&
       !_input1.isKeyPressed("JUMP") && !_input2.isKeyPressed("JUMP"))
       _menuKeyPressed = false;
}

void mainGame::ManageNinjaCreation()
{
    _ninjas.clear();
    _ninja1->DisableBody();
    _ninja2->DisableBody();
    _spellBox->UpdateWizardsPointers(&_ninjas);
    _ninja1->_ninjas = _ninjas;
    _ninja2->_ninjas = _ninjas;
}

void mainGame::MainMenuEvents()
{
    if(ValidMenu(_mainMenu) && !_menuKeyPressed)
    {
        _menuKeyPressed = true;
        switch(_mainMenu->GetSelectedItem())
        {
            case 2:
                if(_mainMenu->GetSelectedText() == "Langue : Francais")
                    _settings->_language = "English";
                else
                    _settings->_language = "Francais";
                RedrawUI();
                _settings->save();
                break;
            case 3:
                exit(0);
                break;
        }
    }
}

void mainGame::RedrawUI()
{
    //Reset UI texts
    _mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->_language), Vector2f(420,425), &_input0);
    //_pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->_language), Vector2f(400,300), &_input0);
}

void mainGame::ChangeScreenMode()
{
    //Getting screen resolution
    Vector2i screenSize;
    if(_settings->_fullscreen == "ON")
        screenSize = Vector2i(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height);
    else
        screenSize = Vector2i(840,576);

    //Recreating window
    ContextSettings ctxSettings;
    ctxSettings.antialiasingLevel = 0;
    _window->create(VideoMode(screenSize.x, screenSize.y, 32), "SHURIKEN!",
    _settings->_fullscreen == "ON" ? Style::Fullscreen : Style::Default, ctxSettings);

    //View
    FloatRect rect;
    if(576.0*screenSize.x/screenSize.y >= 840)
        rect = FloatRect(0,0,576.0*screenSize.x/screenSize.y,576);
    else
        rect = FloatRect(0,0,840,840*screenSize.y/screenSize.x);
    View view(rect);
    view.setCenter(420,288);
    _window->setView(view);
}

void mainGame::PauseMenuEvents()
{
    if(ValidMenu(_pauseMenu) && !_menuKeyPressed)
    {
        switch(_pauseMenu->GetSelectedItem())
        {

        }
    }
}

void mainGame::DisplayGameResources()
{
    _maps->DisplayMap(_window);
    for(int i=0 ; i<_ninjas.size() ; i++)
        _ninjas[i]->Display(_window, _deltaTime);
}

bool mainGame::IsMusicPaused()
{
    return _soundBox.IsMusicPaused();
}

void mainGame::PauseMusic(bool pause)
{
    _soundBox.PauseMusic(pause);
}

void mainGame::GoOn(Keyboard::Key key, bool keyPressed, float deltaTime)
{
    //Updating variables
    _currentKey = key;
    _globalKeyPressed = keyPressed;
    _deltaTime = deltaTime;

    //Updating last key (keyboard or pad)
    _input0.PerformLastController(_window);

    //Playing music
    if(IsMusicPaused())
        PauseMusic(false);

    //Events
    OpeningClosingMenu();

    //Main switch
    switch(_gameState)
    {
        case mainmenu:
            _window->draw(_titleScreenTitle);
            _mainMenu->Display(_window, _deltaTime, true);
            MainMenuEvents();
            break;
        case game:
            DisplayGameResources();
            _world->Step(_deltaTime/60.f, 8, 3);
            Box2DTools::ManageBodyLoop(_world);
            _maps->RenderMap();
            _soundBox.PerformSounds();
            for(int i=0 ; i<_ninjas.size() ; i++)
                _ninjas[i]->Step();
            _effectBox.RenderEffects(_world, _window, _deltaTime);
            _spellBox->RenderSpells(_deltaTime);
            _spellBox->Display(_window);
            break;
        case gameEnded:
            DisplayGameResources();
            _spellBox->Display(_window);
            _effectBox.Display(_window);
            _scoreManager.Display(_window, true);
            break;
        case pause:
            DisplayGameResources();
            PauseMenuEvents();
            _spellBox->Display(_window);
            _effectBox.Display(_window);
            _scoreManager.Display(_window);
            _pauseMenu->Display(_window, _deltaTime);
            break;
    }

    //DEBUG
    _showMasks = Keyboard::isKeyPressed(Keyboard::M) ? true : _showMasks;
    _showMasks = Keyboard::isKeyPressed(Keyboard::L) ? false : _showMasks;

    if(_showMasks && _gameState == game)
        Box2DTools::DrawCollisionMask(_world, _window);
}
