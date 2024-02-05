#include "mainGame.hpp"

using namespace std;
using namespace sf;

static const float NATIVE_WIDTH = 280;
static const float NATIVE_HEIGHT = 192;
static const float GRAVITY = 23.f;

mainGame::mainGame(RenderWindow* window, View* view, settings* settings, Thread* thread, string** scoresString)
{
    //Variables
    _gameState = game;
    _lastGameState = _gameState;
    _endGameTimer = 120;
    _gameOverTimer = 0;
    _lastScore = -1;
    _currentLevel = 3;
    _levelTimer = 180;
    _victoryOrDefeat = -1;
    _maskKeyDuration = 0;
    _editingInputNum = 0;
    _deltaTime = 1;
    _menuKeyPressed = false;
    _globalKeyPressed = false;
    _waitingKey = false;
    _showMasks = false;
    _nickname = "";

    //Objects
    _settings = settings;
    _window = window;
    _view = view;
    _thread = thread;
    _scoresString = scoresString;
    _soundBox.UpdateVolumes(_settings->volume);
    _levelMusic = _soundBox.PlaySound(SND_LEVEL_01, true);
    _levelMusic->pause();

    //Box2D - World
    _world = new b2World(b2Vec2(0.f, GRAVITY));

    //Spells - Music
    _projectileBox = new projectile(_world, &_effectBox, &_soundBox);

    //Textures
    _textureTitleScreenTitle.loadFromFile(_files.titleScreenTitle);
    _textureSettingsFrame.loadFromFile(_files.settingsFrame);
    _texturePauseFrame.loadFromFile(_files.pauseFrame);

    //Sprites
    _titleScreenTitle.setTexture(_textureTitleScreenTitle);
    _settingsFrame.setTexture(_textureSettingsFrame);
    _settingsFrame.setTextureRect(IntRect(0,0,222,146));
    _settingsFrame.setOrigin(_settingsFrame.getLocalBounds().width/2, _settingsFrame.getLocalBounds().height/2);
    _settingsFrame.setPosition(NATIVE_WIDTH/2, NATIVE_HEIGHT/2);
    _pauseFrame.setTexture(_texturePauseFrame);
    _pauseFrame.setOrigin((int)(_pauseFrame.getLocalBounds().width/2), _pauseFrame.getLocalBounds().height/2);
    _pauseFrame.setPosition(NATIVE_WIDTH/2, NATIVE_HEIGHT/2);

    //Shapes
    _squareShape.setSize(Vector2f(NATIVE_WIDTH,NATIVE_HEIGHT));
    _squareShape.setOrigin(NATIVE_WIDTH/2,NATIVE_HEIGHT/2);
    _squareShape.setPosition(NATIVE_WIDTH/2,NATIVE_HEIGHT/2);
    _squareShape.setFillColor(Color(0,0,0));

    //Inputs
    _input0 = input(-1, 30, _window); //Menu input
    _input1 = input(0, 30, _window);
    _input2 = input(1, 30, _window);

    //Ninjas
    _ninja1 = new hero(0, Vector2i(50,20), _world, &_input1, &_effectBox, _projectileBox, &_soundBox, &_scoreManager, 3, _settings);
    _ninja1->ResetLifeText(xmlReader::getTranslationText("game", _settings->language, "life"));
    //_ninja2 = new hero(1, Vector2i(615,145), _world, &_input2, &_effectBox, _projectileBox, &_soundBox, &_scoreManager, 3);
    _ninjas.push_back(_ninja1);
    _ninja1->_ninjas = _ninjas;
    //_ninja2->_ninjas = _ninjas;
    _ninja1->_enemies = &_enemies;

    _projectileBox->UpdatePointers(&_ninjas, &_enemies);

    //UI
    _mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->language), Vector2f(140,140), &_input0, &_soundBox);
    _settingsMenu = new menuList(xmlReader::getTranslationTexts("settings", _settings->language), Vector2f(80,33), &_input0, &_soundBox, ALIGNEMENT_CENTER, true);
    _pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->language), Vector2f(140,77), &_input0, &_soundBox);
    _powerups = new powerups(&_ninjas, &_effectBox, &_soundBox, _world);

    //Map
    _maps = new maps(_world, &_effectBox, &_soundBox, &_scoreManager, _settings, &_ninjas, _currentLevel);

    //UI SELECTOR
    _inputs.push_back(&_input1);
    _inputs.push_back(&_input2);

    //Texts
    _gameOverText.SetString(xmlReader::getTranslationText("other", _settings->language, "gameover"));
    _gameOverText.SetPosition(140, 80);
    _scoreEnterText.SetString(xmlReader::getTranslationText("other", _settings->language, "scoreEnter"));
    _scoreEnterText.SetPosition(140, 95);
    _nicknameText.SetString(string(20, '_'));
    _nicknameText.SetPosition(140, 105);
    _nicknameText.SetColor(Color(235,125,35));
    _menuTitleText.SetString(xmlReader::getTranslationText("menuMain", _settings->language, "MenuMainB"));
    _menuTitleText.SetPosition(140, 8);
    _menuTitleText.Highlight(Color(235,125,35), Color(133,59,81));
    _menuTitleText.ActivateWaveMode(true);
    _settingsValue.SetString("42");
    _settingsValue.SetColor(Color(0,0,0));
    _settingsValue.SetPosition(187,39);
    _settingsLeftArrow.SetString(" < ");
    _settingsLeftArrow.SetColor(Color(0,0,0));
    _settingsLeftArrow.SetPosition(137,39);
    _settingsRightArrow.SetString(" > ");
    _settingsRightArrow.SetColor(Color(0,0,0));
    _settingsRightArrow.SetPosition(237,39);
    _settingsBackArrow.SetString("<");
    _settingsBackArrow.SetCharacterSizeRatio(2);
    _settingsBackArrow.SetPosition(270,180);
    _settingsEditText.SetString(xmlReader::getTranslationText("other", _settings->language, "edit"));
    _settingsEditText.SetColor(Color(0,0,0));
    _settingsEditText.SetPosition(187,50);
    vector<string> actionPadTexts = xmlReader::getTranslationTexts("actionPad", _settings->language);
    for(int i=0 ; i<4 ; i++)
    {
        _actionPadTexts[i].SetString(i <= actionPadTexts.size()-1 ? actionPadTexts[i] : "missing translation");
        _actionPadTexts[i].SetColor(Color(0,0,0));
        _actionPadTexts[i].SetPosition(158,(i > 0 ? 105 : 103) + i*14, ALIGNEMENT_LEFT);
    }
    //string ca = "+";
    //cout << ca << " : " << (int)(ca[0]) << endl;
    //cout << typeid(enemyTest).name() << " " << utils::strContains(typeid(enemyTest).name(), "enemy") <<  endl;

    /*enemy* enemyTest = new enemy(2, Vector2i(275, 85), _world, &_effectBox, _projectileBox, &_soundBox, &_scoreManager, _powerups, &_texturePauseFrame, &_texturePauseFrame);
    blueTank* blueTankTest = new blueTank(Vector2i(275, 85), _world, &_effectBox, _projectileBox, &_soundBox, &_scoreManager, _powerups, &_texturePauseFrame, &_texturePauseFrame);
    enemyTest->test();
    blueTankTest->test();*/

    /*_enemies.push_back(new enemy(2, Vector2i(275, 85), _world, &_effectBox, _projectileBox, &_soundBox, &_scoreManager, _powerups, &_texturePauseFrame, &_texturePauseFrame));
    _enemies.push_back(new blueTank(Vector2i(275, 85), _world, &_effectBox, _projectileBox, &_soundBox, &_scoreManager, _powerups, &_texturePauseFrame, &_texturePauseFrame));
    _enemies[0]->Step();
    _enemies[1]->Step();*/
}

void mainGame::ResetRoom()
{
    //Reset objects
    if(_currentLevel == 0)
        _ninja1->ResetHero(0, 3, Vector2i(35,60));
    else if(_currentLevel == 1)
        _ninja1->ResetHero(0, 3, Vector2i(70,140));
    else
        _ninja1->ResetHero(0, 3, Vector2i(60,170));
    _projectileBox->DestroyAllProjectiles();
    _projectileBox->ChangeDommages(0);
    _powerups->DeleteAllPowerUps();
    _endGameTimer = 120;
    _levelTimer = 180;

    //Destroying all enemies
    while(_enemies.size() > 0)
    {
        delete _enemies[0];
        _enemies.erase(_enemies.begin());
    }

    //Music
    if(_currentLevel == 0)
    {
        _levelMusic = _soundBox.PlaySound(SND_LEVEL_01, true);
        _levelMusic->pause();
    }

    //Level text
    stringstream stream;
    stream << _currentLevel+1;
    _gameOverText.SetString(xmlReader::getTranslationText("game", _settings->language, "level") + " " + stream.str());
    _gameOverText.SetPosition(140, 80);
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
    Vector2f mousePos = _window->mapPixelToCoords(sf::Mouse::getPosition(*_window));
    switch(_gameState)
    {
        case intro:
            if(_input0.isKeyPressed("VALID") || Mouse::isButtonPressed(Mouse::Left))
               _gameState = mainmenu;
            break;
        case setting:
            //Arrow Focus + Edit text focus
            _settingsBackArrow.SetColor(_settingsBackArrow.GetGlobalBounds().contains(mousePos) ? Color(234,93,240) : Color(255,255,255));
            _settingsEditText.SetColor(_settingsEditText.GetGlobalBounds().contains(mousePos) ? Color(234,93,240) : Color(133,59,81));

            //Action
            if((_input0.isKeyPressed("BACK") || _input0.isKeyPressed("PAUSE") ||
               (_settingsBackArrow.GetGlobalBounds().contains(mousePos) && Mouse::isButtonPressed(Mouse::Left))) &&
               !_menuKeyPressed)
            {

                if(_lastGameState == pause)
                {
                    _menuTitleText.SetString(xmlReader::getTranslationText("other", _settings->language, "pause"));
                    _menuTitleText.Highlight(Color(235,125,35), Color(133,59,81));
                    _menuTitleText.SetPosition(140, 40);
                }
                _soundBox.PlaySound(SND_BLIP, false);
                _menuKeyPressed = true;
                _gameState = _lastGameState;
            }
            break;
        case game:
            if(_input0.isKeyPressed("PAUSE") && !_menuKeyPressed)
            {
                _menuTitleText.SetString(xmlReader::getTranslationText("other", _settings->language, "pause"));
                _menuTitleText.Highlight(Color(235,125,35), Color(133,59,81));
                _menuTitleText.SetPosition(140, 40);

                _pauseMenu->ResetAnimation();
                _menuKeyPressed = true;
                _soundBox.PlaySound(SND_BLIP2, false);
                _levelMusic->pause();
                _gameState = pause;
            }
            break;
        case pause:
            if((_input0.isKeyPressed("BACK") || _input0.isKeyPressed("PAUSE")) && !_menuKeyPressed)
            {
                _pauseMenu->ResetAnimation();
                _menuKeyPressed = true;
                _soundBox.PlaySound(SND_BLIP2, false);
                _levelMusic->play();
                _gameState = game;
            }
            break;
        case leaderboard:
            _settingsBackArrow.SetColor(_settingsBackArrow.GetGlobalBounds().contains(mousePos) ? Color(234,93,240) : Color(255,255,255));
            if((_input0.isKeyPressed("BACK") || _input0.isKeyPressed("PAUSE") ||
               (_settingsBackArrow.GetGlobalBounds().contains(mousePos) && Mouse::isButtonPressed(Mouse::Left))) &&
               !_menuKeyPressed)
            {
                _thread->terminate();
                _soundBox.PlaySound(SND_BLIP, false);
                _gameState = mainmenu;
            }
            break;
        case score:
            if(_input0.isKeyPressed("PAUSE") && !_menuKeyPressed)
            {
                _soundBox.PlaySound(SND_BLIP, false);
                _gameState = mainmenu;
            }
            else if(_input0.isKeyPressed("VALID") && _nickname.length() > 0 && !_menuKeyPressed)
            {
                //Sending score
                SendScoreOnline();

                //Changing title
                _menuTitleText.SetString(xmlReader::getTranslationText("menuMain", _settings->language, "MenuMainB"));
                _menuTitleText.Highlight(Color(235,125,35), Color(133,59,81));
                _menuTitleText.SetPosition(140, 8);

                //Get scores on database
                for(int i=0 ; i<20 ; i++)
                    (*_scoresString)[i] = xmlReader::getTranslationText("other", _settings->language, "loading");
                _thread->launch(); // Downloading scores in parallel thread
                _gameState = leaderboard;
            }
            break;
    }
    if(!_waitingKey && !Keyboard::isKeyPressed(Keyboard::Num0) &&
       !_input0.isKeyPressed("BACK") && !_input0.isKeyPressed("PAUSE") &&
       !_input0.isKeyPressed("LEFT") && !_input0.isKeyPressed("RIGHT") &&
       !_input0.isKeyPressed("VALID") && !Mouse::isButtonPressed(Mouse::Left) &&
       !_input1.isKeyPressed("ROLL") && !_input2.isKeyPressed("ROLL") &&
       !_input1.isKeyPressed("JUMP") && !_input2.isKeyPressed("JUMP"))
       _menuKeyPressed = false;
}

void mainGame::MainMenuEvents()
{
    if(ValidMenu(_mainMenu) && !_menuKeyPressed)
    {
        _menuKeyPressed = true;
        _soundBox.PlaySound(SND_BLIP2, false);
        switch(_mainMenu->GetSelectedItem())
        {
            case 0:
                _currentLevel = 0;
                ResetRoom();
                _gameState = level;
                break;
            case 1:
                //Init score strings
                for(int i=0 ; i<20 ; i++)
                    (*_scoresString)[i] = xmlReader::getTranslationText("other", _settings->language, "loading");
                _thread->launch(); // Downloading scores in parallel thread

                //Changing title
                _menuTitleText.SetString(xmlReader::getTranslationText("menuMain", _settings->language, "MenuMainB"));
                _menuTitleText.Highlight(Color(235,125,35), Color(133,59,81));
                _menuTitleText.SetPosition(140, 8);

                _gameState = leaderboard;
                break;
            case 2:
                if(_settings->language == "Francais")
                    _settings->language = "English";
                else
                    _settings->language = "Francais";
                RedrawUI();
                _mainMenu->FocusElement(2);
                _settings->Save();
                break;
            case 3:
                _menuTitleText.SetString(xmlReader::getTranslationText("menuMain", _settings->language, "MenuMainD"));
                _menuTitleText.Highlight(Color(235,125,35), Color(133,59,81));
                _menuTitleText.SetPosition(140, 8);
                _gameState = setting;
                _lastGameState = mainmenu;
                break;
            case 4:
                exit(0);
                break;
        }
    }
}

void mainGame::SettingsMenuEvents()
{
    stringstream stream;
    _settingsFrame.setTextureRect(IntRect(0,0,222,146));
    switch(_settingsMenu->GetSelectedItem())
    {
        case 0:
            if(_settings->language == "Francais") //ç too tricky to handle in XML file
                _settingsValue.SetString("Français");
            else
                _settingsValue.SetString(_settings->language);
            break;
        case 1:
            stream << _settings->volume;
            _settingsValue.SetString(stream.str());
            _levelMusic->setVolume(_settings->volume);
            break;
        case 2:
            stream << "x" << _settings->screenRatio;
            _settingsValue.SetString(stream.str());
            break;
        case 3:
            _settingsValue.SetString(_settings->fullscreen ? "ON" : "OFF");
            break;
        case 5:
            _settingsValue.SetString(_input1.GetKeyString(LEFT));
            break;
        case 6:
            _settingsValue.SetString(_input1.GetKeyString(RIGHT));
            break;
        case 7:
            _settingsValue.SetString(_input1.GetKeyString(UP));
            break;
        case 8:
            _settingsValue.SetString(_input1.GetKeyString(DOWN));
            break;
        case 9:
            _settingsValue.SetString(_input1.GetKeyString(JUMP));
            break;
        case 10:
            _settingsValue.SetString(_input1.GetKeyString(ATTACK));
            break;
        case 11:
            _settingsValue.SetString(_input1.GetKeyString(SPECIAL));
            break;
        case 12:
            _settingsFrame.setTextureRect(IntRect(0,146,222,146));
            break;
    }

    //Arrows
    Vector2f mousePos = _window->mapPixelToCoords(sf::Mouse::getPosition(*_window));
    if(_input0.GetMouseController() && _settingsLeftArrow.GetGlobalBounds().contains(mousePos))
        _settingsLeftArrow.SetColor(!Mouse::isButtonPressed(Mouse::Left) ? Color(234,93,240) : Color(246,185,202));
    else
        _settingsLeftArrow.SetColor(Color(0,0,0));
    if(_input0.GetMouseController() && _settingsRightArrow.GetGlobalBounds().contains(mousePos))
        _settingsRightArrow.SetColor(!Mouse::isButtonPressed(Mouse::Left) ? Color(234,93,240) : Color(246,185,202));
    else
        _settingsRightArrow.SetColor(Color(0,0,0));

    //Changing settings
    int screenRatioCurrent = _settings->screenRatio;
    string languageCurrent = _settings->language;
    if(!_menuKeyPressed && (_input0.isKeyPressed("LEFT") || _settingsLeftArrow.GetColor() == Color(246,185,202)))
    {
        _menuKeyPressed = true;
        _soundBox.PlaySound(SND_BLIP2, false);
        _settings->ChoosePreviousSetting(_settingsMenu->GetSelectedItem());
    }
    else if(!_menuKeyPressed && (_input0.isKeyPressed("RIGHT") || _settingsRightArrow.GetColor() == Color(246,185,202)))
    {
        _menuKeyPressed = true;
        _soundBox.PlaySound(SND_BLIP2, false);
        _settings->ChooseNextSetting(_settingsMenu->GetSelectedItem());
    }

    //Applying settings
    _soundBox.UpdateVolumes(_settings->volume);
    if(_settings->screenRatio != screenRatioCurrent)
        _window->create(VideoMode(NATIVE_WIDTH*_settings->screenRatio, NATIVE_HEIGHT*_settings->screenRatio, 32), "SHURIKEN!", Style::Close, _window->getSettings());
    if(_settings->language != languageCurrent)
        RedrawUI();

    //Key configuration
    if(_settingsMenu->GetSelectedItem() >= 5 && _settingsMenu->GetSelectedItem() < 12)
    {
        if(_waitingKey)
            _settingsValue.SetString(xmlReader::getTranslationText("other", _settings->language, "keyChanging"));
        if(!_waitingKey && !_menuKeyPressed && (_input0.isKeyPressed("VALID") ||
           _settingsEditText.GetGlobalBounds().contains(mousePos) && Mouse::isButtonPressed(Mouse::Left)))
        {
            _menuKeyPressed = true;
            _waitingKey = true;
            _settingsValue.ActivateColorFlashing(true);
            _soundBox.PlaySound(SND_BLIP2, false);
        }
        else if(_globalKeyPressed && _waitingKey)
        {
            _menuKeyPressed = true;
            _settingsMenu->_keyPressed = true;
            _waitingKey = false;
            _soundBox.PlaySound(SND_RELEASE, false);
            _settingsValue.ActivateColorFlashing(false, Color(0,0,0));
            _input1.setKey(_settingsMenu->GetSelectedItem(), _currentKey);
            _input1.WriteXML();
        }
    }
}

void mainGame::PauseMenuEvents()
{
    if(ValidMenu(_pauseMenu) && !_menuKeyPressed)
    {
        _menuKeyPressed = true;
        _soundBox.PlaySound(SND_BLIP2, false);
        switch(_pauseMenu->GetSelectedItem())
        {
            case 0:
                _ninja1->_hasJumped = true;
                _levelMusic->play();
                _gameState = game;
                break;
            case 1:
                _menuTitleText.SetString(xmlReader::getTranslationText("menuMain", _settings->language, "MenuMainD"));
                _menuTitleText.Highlight(Color(235,125,35), Color(133,59,81));
                _menuTitleText.SetPosition(140, 8);
                _lastGameState = pause;
                _gameState = setting;
                break;
            case 2:
                _gameState = mainmenu;
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
    _mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->language), Vector2f(140,140), &_input0, &_soundBox);
    _settingsMenu = new menuList(xmlReader::getTranslationTexts("settings", _settings->language), Vector2f(80,33), &_input0, &_soundBox, ALIGNEMENT_CENTER, true);
    _pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->language), Vector2f(140,77), &_input0, &_soundBox);
    _menuTitleText.SetString(xmlReader::getTranslationText("menuMain", _settings->language, "MenuMainD"));
    _menuTitleText.Highlight(Color(235,125,35), Color(133,59,81));
    _menuTitleText.SetPosition(140, 8);
    _settingsEditText.SetString(xmlReader::getTranslationText("other", _settings->language, "edit"));
    _settingsEditText.SetColor(Color(0,0,0));
    _settingsEditText.SetPosition(187,50);
    _scoreManager.AddPoints(0);
    _scoreManager.AddEnemyKilled(0);
    _scoreManager.AddEnemyMissed(0);
    _ninja1->ResetLifeText(xmlReader::getTranslationText("game", _settings->language, "life"));
    _gameOverText.SetString(xmlReader::getTranslationText("other", _settings->language, "gameover"));
    _gameOverText.SetPosition(140, 80);
    _scoreEnterText.SetString(xmlReader::getTranslationText("other", _settings->language, "scoreEnter"));
    _scoreEnterText.SetPosition(140, 95);
    vector<string> actionPadTexts = xmlReader::getTranslationTexts("actionPad", _settings->language);
    for(int i=0 ; i<4 ; i++)
    {
        _actionPadTexts[i].SetString(i <= actionPadTexts.size()-1 ? actionPadTexts[i] : "missing translation");
        _actionPadTexts[i].SetColor(Color(0,0,0));
        _actionPadTexts[i].SetPosition(158,(i > 0 ? 105 : 103) + i*14, ALIGNEMENT_LEFT);
    }
}

void mainGame::SendScoreOnline()
{
    //Sending request
    _lastScore = _scoreManager.GetScore();
    sf::Http http("http://shinpansen.esy.es/");
    sf::Http::Request request("/insert_scores.php", sf::Http::Request::Post);
    std::ostringstream stream;
    stream << "pseudo=" << _nickname << "&score=" << _scoreManager.GetScore();
    request.setBody(stream.str());
    sf::Http::Response response = http.sendRequest(request, Time::Zero);
}

void mainGame::ChangeScreenMode()
{
    //Getting screen resolution
    Vector2i screenSize;
    if(_settings->fullscreen)
        screenSize = Vector2i(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height);
    else
        screenSize = Vector2i(NATIVE_WIDTH,NATIVE_HEIGHT);

    //Recreating window
    ContextSettings ctxSettings;
    ctxSettings.antialiasingLevel = 0;
    _window->create(VideoMode(screenSize.x, screenSize.y, 32), "SHURIKEN!",
    _settings->fullscreen ? Style::Fullscreen : Style::Default, ctxSettings);

    //View
    FloatRect rect;
    if(NATIVE_HEIGHT*screenSize.x/screenSize.y >= NATIVE_WIDTH)
        rect = FloatRect(0,0,NATIVE_HEIGHT*screenSize.x/screenSize.y,NATIVE_HEIGHT);
    else
        rect = FloatRect(0,0,NATIVE_WIDTH,NATIVE_WIDTH*screenSize.y/screenSize.x);
    View view(rect);
    view.setCenter(NATIVE_WIDTH/2,NATIVE_HEIGHT/2);
    _window->setView(view);
}

void mainGame::PauseMusic(bool pause)
{
    if(pause &&  _levelMusic->getStatus() == SoundSource::Playing)
        _levelMusic->pause();
    else if(!pause && _gameState == game && _levelMusic->getStatus() == SoundSource::Paused)
        _levelMusic->play();
}

void mainGame::DisplayGameResources()
{
    //Spawn
    if(_gameState == game)
        _maps->PerformEvents(&_enemies, &_scoreManager, &_effectBox, _projectileBox, &_soundBox, _powerups);

    //Display
    if(!_ninjas[0]->isUsingSpecialAttack())
    {
        _maps->DisplayLayer0(_window);
        for(int i=0 ; i<_enemies.size() ; i++)
            if(_enemies[i] != NULL)
                _enemies[i]->Display(_window, _deltaTime);
    }

    for(int i=0 ; i<_ninjas.size() ; i++)
        _ninjas[i]->Display(_window, _deltaTime);

    if(!_ninjas[0]->isUsingSpecialAttack())
    {
        _projectileBox->Display(_window);
        _maps->DisplayLayer1(_window);
        _powerups->Display(_window);
    }
}

void mainGame::DisplayLeaderboard()
{
    //Drawing scores text
    for(int i=0 ; i<20 ; i++)
    {
        string score = (*_scoresString)[i];//_scoresOnline[i];
        if(score.length() >= 3 && score.substr(0,1) != ";")
        {
            //Creating texts
            vector<string> values = _files.readLine(score);
            pixelFont onlineRank(values[0], Vector2f(10,24+i*8), ALIGNEMENT_LEFT);
            onlineRank.SetColor(Color(246,185,202));
            string dots = values[1].length() < 20 ? string(20-values[1].length(), ';') : "";
            pixelFont nickname(values[1] + dots, Vector2f(25,24+i*8), ALIGNEMENT_LEFT);
            pixelFont scoreValue(values[2], Vector2f(150,24+i*8), ALIGNEMENT_LEFT);
            scoreValue.SetColor(Color(234,93,240));

            //Focus last score
            if(values[1] == _nickname && stoi(values[2]) == _lastScore)
                nickname.SetColor(Color(235,127,35));

            //Display
            onlineRank.Display(_window);
            nickname.Display(_window);
            scoreValue.Display(_window);
        }
        else
        {
            pixelFont text(score, Vector2f(10,24+i*8),ALIGNEMENT_LEFT);
            text.Display(_window);
        }
    }
    _menuTitleText.Display(_window);
}

void mainGame::GoOn(Keyboard::Key key, bool keyPressed, float deltaTime)
{
    try
    {
    //Updating variables
    _currentKey = key;
    _globalKeyPressed = keyPressed;
    _deltaTime = deltaTime;

    //Updating last key (keyboard or pad)
    _input0.PerformLastController(_window);

    //Events
    OpeningClosingMenu();
    _soundBox.PerformSounds();

    //{Main switch
    switch(_gameState)
    {
        case mainmenu:
            _window->draw(_titleScreenTitle);
            _mainMenu->Display(_window, _deltaTime, true);
            MainMenuEvents();
            break;
        case setting:
            _window->draw(_settingsFrame);
            _menuTitleText.Display(_window);
            if(_settingsMenu->GetSelectedItem() < 12)
                _settingsValue.Display(_window);
            _settingsMenu->Display(_window, _deltaTime, !_waitingKey);
            if(_settingsMenu->GetSelectedItem() < 4)
            {
                _settingsLeftArrow.Display(_window);
                _settingsRightArrow.Display(_window);
            }
            if(_input0.GetMouseController())
                _settingsBackArrow.Display(_window);
            if(!_waitingKey && _settingsMenu->GetSelectedItem() >= 5 && _settingsMenu->GetSelectedItem() < 12)
                _settingsEditText.Display(_window);
            if(_settingsMenu->GetSelectedItem() == 12)
                for(int i=0 ; i<4 ; i++)
                    _actionPadTexts[i].Display(_window);
            SettingsMenuEvents();
            break;
        case level:
            //Display + render last effects
            _effectBox.RenderEffects(_world, _window, _view);
            _squareShape.setScale(4, 4);
            _window->draw(_squareShape);
            _gameOverText.Display(_window);

            if(_levelTimer == 160)
                _soundBox.PlaySound(SND_LEVEL, false);

            //Timer
            if(_levelTimer > 0)
                _levelTimer--;
            else
            {
                _maps->LoadMap(_currentLevel);
                _scoreManager.Reset(_maps->GetEnemiesToKill(), _maps->GetEnemiesToMiss(), _settings->language, false, _currentLevel == 0);
                _scoreManager.AddPoints(0);
                _scoreManager.AddEnemyKilled(0);
                _scoreManager.AddEnemyMissed(0);
                _levelMusic->play();
                _gameState = game;
            }
            break;
        case game:
            DisplayGameResources();

            //Render
            if(!_ninjas[0]->isUsingSpecialAttack() && _endGameTimer > 30)
            {
                if(!_ninjas[0]->IsSpawningOrTeleporting())
                {
                    _world->Step(_deltaTime/60.f, 8, 3);
                    Box2DTools::ManageBodyLoop(_world);
                    _projectileBox->RenderProjectiles(_deltaTime);
                    for(int i=0 ; i<_enemies.size() ; i++)
                        if(_enemies[i] != NULL)
                            _enemies[i]->Step();
                }
                _effectBox.RenderEffects(_world, _window, _view);
            }
            for(int i=0 ; i<_ninjas.size() ; i++)
                _ninjas[i]->Step();

            _scoreManager.Display(_window, _currentLevel < 3);
            for(int i=0 ; i<_ninjas.size() ; i++)
                _ninjas[i]->DrawLife(_window);

            //{Game over or victory
            if(_ninja1->IsDead() || _scoreManager.GetObjectivesState() > 0)
            {
                _victoryOrDefeat = _ninja1->IsDead() ? 2 : _scoreManager.GetObjectivesState();
                //Killing all enemies if victory
                if(_endGameTimer == 70 && _scoreManager.GetObjectivesState() == 1)
                {
                    for(int i=0 ; i<_enemies.size() ; i++)
                        _enemies[i]->SetDommages(999);
                }

                //Ending animation
                if(_endGameTimer <= 30)
                {
                    float shapeRatio = _endGameTimer > 20 ? 1.f/(float)(_endGameTimer-20) : 1.f;
                    _squareShape.setScale(shapeRatio, shapeRatio);
                    _window->draw(_squareShape);
                }
                if(_endGameTimer > 0)
                    _endGameTimer--;
                else
                {
                    //Score + sound + game state
                    _gameOverTimer = _victoryOrDefeat== 1 ? 600 : 300;
                    _currentLevel++;
                    ResetRoom();
                    _levelTimer = 10;//180;

                     //Changing UI texts
                    if(_victoryOrDefeat == 2 || _currentLevel == 4)
                    {
                        stringstream stream;
                        stream << _scoreManager.GetScore();
                        string id = _victoryOrDefeat == 1 ? "scorefinal": "gameover";
                        string text = xmlReader::getTranslationText("other", _settings->language, id);
                        text += _victoryOrDefeat == 1 ? " : " + stream.str() : "";
                        _gameOverText.SetString(text);
                        _gameOverText.SetPosition(140, 80);
                        if(_victoryOrDefeat == 2 || _ninja1->IsDead())
                            _soundBox.PlaySound(SND_GAMEOVER, false);
                    }

                    //Music
                    _levelMusic->pause();
                    delete _levelMusic;
                    if(_currentLevel == 1)
                        _levelMusic = _soundBox.PlaySound(SND_LEVEL_02, true);
                    else
                        _levelMusic = _soundBox.PlaySound(SND_LEVEL_03, true);
                    _levelMusic->pause();

                    if(_currentLevel <= 3 || _victoryOrDefeat == 2)
                        _gameState = _victoryOrDefeat == 1 ? level : gameOver;
                    else
                    {
                        _soundBox.PlaySound(SND_VICTORY, false);
                        _gameState = score;
                    }
                }
            }//}
            break;
        case pause:
            _menuTitleText.Display(_window);
            _window->draw(_pauseFrame);
            _pauseMenu->Display(_window, _deltaTime);
            PauseMenuEvents();
            break;
        case gameOver:
            _gameOverTimer -= _gameOverTimer > 0 ? 1 : 0;
            _gameOverText.Display(_window);

            if(_gameOverTimer == 0)
                _gameState = mainmenu;
            break;
        case score:
            _gameOverText.Display(_window);
            _scoreEnterText.Display(_window);

            //Perform input
            if(_globalKeyPressed)
            {
                _soundBox.PlaySound(SND_LOAD, false);
                if(_input0.isKeyPressed("BACK") && _nickname.length() > 0)
                    _nickname = _nickname.substr(0, _nickname.length()-1);
                else if(_nickname.length() < 20)
                    _nickname += _input0.GetKeyName(_currentKey);
            }

            //Drawing nickname text
            _nicknameText.SetString(_nickname + string(20-_nickname.length(), '_'));
            _nicknameText.SetPosition(140, 105);
            _nicknameText.Display(_window);
            break;
        case leaderboard:
            if(_input0.GetMouseController())
                _settingsBackArrow.Display(_window);
            DisplayLeaderboard();
            break;
    }
    //}

    //{DEBUG
    if(Keyboard::isKeyPressed(Keyboard::Tab))
    {
        _maskKeyDuration ++;
        if(_maskKeyDuration >= 40)
            _showMasks = true;
        else if (_maskKeyDuration < 40 && _showMasks)
            _showMasks = false;
    }
    else
        _maskKeyDuration = 0;

    if(_showMasks && _gameState == game)
        Box2DTools::DrawCollisionMask(_world, _window);
    //}
    }
    catch(exception e)
    {
        throw e;
        cout << e.what() << endl;
    }
}
