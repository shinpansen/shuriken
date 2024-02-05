#include "mainGame.hpp"

using namespace std;
using namespace sf;

static const float NATIVE_WIDTH = 280;
static const float NATIVE_HEIGHT = 192;

mainGame::mainGame(RenderWindow* window, View* view, settings* settings)
{
    //Variables
    _gameState = game;
    _deltaTime = 1;
    _menuKeyPressed = false;
    _globalKeyPressed = false;
    _waitingKey = false;
    _showMasks = false;
    _editingInputNum = 0;
    _endGameTimer = 120;
    _gameOverTimer = 0;
    _scoreToLoad = -1;
    _currentLevel = 0;
    _levelTimer = 180;
    _nickname = "";

    //Objects
    _settings = settings;
    _window = window;
    _view = view;
    _soundBox.UpdateVolumes(_settings);
    _levelMusic = _soundBox.PlaySound(SND_LEVEL_01, true);
    _levelMusic->pause();

    //Box2D - World
    _world = new b2World(b2Vec2(0.f, 28.f));

    //Spells - Music
    _projectileBox = new projectile(_world, &_effectBox, &_soundBox);

    //Graphics
    _titleScreenTitleTexture.loadFromFile(_files.titleScreenTitle);
    _titleScreenTitle.setTexture(_titleScreenTitleTexture);
    _titleScreenTitle.setPosition(0,0);
    _squareShape.setSize(Vector2f(NATIVE_WIDTH,NATIVE_HEIGHT));
    _squareShape.setOrigin(NATIVE_WIDTH/2,NATIVE_HEIGHT/2);
    _squareShape.setPosition(NATIVE_WIDTH/2,NATIVE_HEIGHT/2);
    _squareShape.setFillColor(Color(0,0,0));

    //Inputs
    _input0 = input(-1, 30, _window); //Menu input
    _input1 = input(0, 30, _window);
    _input2 = input(1, 30, _window);

    //Ninjas
    _ninja1 = new hero(0, Vector2i(50,160), _world, &_input1, &_effectBox, _projectileBox, &_soundBox, &_scoreManager, 3, _settings);
    _ninja1->ResetLifeText(xmlReader::getTranslationText("game", _settings->_language, "life"));
    //_ninja2 = new hero(1, Vector2i(615,145), _world, &_input2, &_effectBox, _projectileBox, &_soundBox, &_scoreManager, 3);
    _ninjas.push_back(_ninja1);
    _ninja1->_ninjas = _ninjas;
    //_ninja2->_ninjas = _ninjas;
    _ninja1->_enemies = &_enemies;

    _projectileBox->UpdatePointers(&_ninjas, &_enemies);

    //UI
    _mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->_language), Vector2f(140,140), &_input0, &_soundBox);
    _pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->_language), Vector2f(140,96), &_input0, &_soundBox);
    _powerups = new powerups(&_ninjas, &_effectBox, &_soundBox, _world);

    //Map
    _maps = new maps(_world, &_effectBox, &_soundBox, &_scoreManager, _settings, &_ninjas, _currentLevel);

    //UI SELECTOR
    _inputs.push_back(&_input1);
    _inputs.push_back(&_input2);

    //Texts
    _font.loadFromFile(_files.font2);
    _gameOverText.setFont(_font);
    _gameOverText.setCharacterSize(16);
    _gameOverText.setString(xmlReader::getTranslationText("other", _settings->_language, "gameover"));
    _gameOverText.setPosition(420-(_gameOverText.getLocalBounds().width/2), 240);
    _scoreEnterText.setFont(_font);
    _scoreEnterText.setCharacterSize(16);
    _scoreEnterText.setString(xmlReader::getTranslationText("other", _settings->_language, "scoreEnter"));
    _scoreEnterText.setPosition(420-(_scoreEnterText.getLocalBounds().width/2), 280);
    _nicknameText.setFont(_font);
    _nicknameText.setCharacterSize(16);
    _nicknameText.setColor(Color(235,125,35));
    _nicknameText.setString(string(20, '_'));
    _nicknameText.setPosition(420-(_nicknameText.getLocalBounds().width/2), 320);
    _leaderBoardText.setFont(_font);
    _leaderBoardText.setCharacterSize(16);
    _leaderBoardText.setColor(Color(235,125,35));
    _leaderBoardText.setString(xmlReader::getTranslationText("other", _settings->_language, "leaderboard"));
    _leaderBoardText.setPosition(420-(_leaderBoardText.getLocalBounds().width/2), 20);
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
            if((_input0.isKeyPressed("BACK") || _input0.isKeyPressed("PAUSE")) && !_menuKeyPressed)
            {
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

                //Get scores on database
                for(int i=0 ; i<20 ; i++)
                    _scoresOnline[i] = xmlReader::getTranslationText("other", _settings->_language, "loading");
                _scoreToLoad = 25;
                _soundBox.PlaySound(SND_BLIP2, false);
                _gameState = leaderboard;
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
    _gameOverText.setString(xmlReader::getTranslationText("game", _settings->_language, "level") + " " + stream.str());
    _gameOverText.setPosition(420-(_gameOverText.getLocalBounds().width/2), 240);
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
                //Get scores on database
                for(int i=0 ; i<20 ; i++)
                    _scoresOnline[i] = xmlReader::getTranslationText("other", _settings->_language, "loading");
                _scoreToLoad = 25;
                _gameState = leaderboard;
                break;
            case 2:
                if(_mainMenu->GetSelectedText() == "Langue : Francais")
                    _settings->_language = "English";
                else
                    _settings->_language = "Francais";
                RedrawUI();
                _mainMenu->FocusElement(2);
                _settings->save();
                break;
            case 3:
                if(_settings->_fullscreen == "OFF")
                    _settings->_fullscreen = "ON";
                else
                    _settings->_fullscreen = "OFF";
                ChangeScreenMode();
                _settings->save();
                break;
            case 4:
                exit(0);
                break;
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
                _gameState = mainmenu;
                break;
        }
    }
}

void mainGame::RedrawUI()
{
    //Reset UI texts
    _mainMenu = new menuList(xmlReader::getTranslationTexts("menuMain", _settings->_language), Vector2f(140,140), &_input0, &_soundBox);
    _pauseMenu = new menuList(xmlReader::getTranslationTexts("pause", _settings->_language), Vector2f(140,96), &_input0, &_soundBox);
    _scoreManager.AddPoints(0);
    _scoreManager.AddEnemyKilled(0);
    _scoreManager.AddEnemyMissed(0);
    _ninja1->ResetLifeText(xmlReader::getTranslationText("game", _settings->_language, "life"));
    _gameOverText.setString(xmlReader::getTranslationText("other", _settings->_language, "gameover"));
    _gameOverText.setPosition(420-(_gameOverText.getLocalBounds().width/2), 240);
    _scoreEnterText.setString(xmlReader::getTranslationText("other", _settings->_language, "scoreEnter"));
    _scoreEnterText.setPosition(420-(_scoreEnterText.getLocalBounds().width/2), 280);
    _leaderBoardText.setString(xmlReader::getTranslationText("other", _settings->_language, "leaderboard"));
    _leaderBoardText.setPosition(420-(_leaderBoardText.getLocalBounds().width/2), 15);
}

void mainGame::SendScoreOnline()
{
    //Sending request
    sf::Http http("http://shinpansen.esy.es/");
    sf::Http::Request request("/insert_scores.php", sf::Http::Request::Post);
    std::ostringstream stream;
    stream << "pseudo=" << _nickname << "&score=" << _scoreManager.GetScore();
    request.setBody(stream.str());
    sf::Http::Response response = http.sendRequest(request, Time::Zero);
}

string mainGame::GetScoreOnline(int i)
{
    //Sending request
    sf::Http http("http://shinpansen.esy.es/");
    sf::Http::Request request("/get_scores.php", sf::Http::Request::Post);
    std::ostringstream stream;
    stream << "i=" << i;
    request.setBody(stream.str());
    sf::Http::Response response = http.sendRequest(request, Time::Zero);

    //If error connection
    if(response.getStatus() != 200)
        return xmlReader::getTranslationText("other", _settings->_language, "connection");

    return response.getBody() != "" ? response.getBody() : "...";
}

void mainGame::ChangeScreenMode()
{
    //Getting screen resolution
    Vector2i screenSize;
    if(_settings->_fullscreen == "ON")
        screenSize = Vector2i(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height);
    else
        screenSize = Vector2i(NATIVE_WIDTH,NATIVE_HEIGHT);

    //Recreating window
    ContextSettings ctxSettings;
    ctxSettings.antialiasingLevel = 0;
    _window->create(VideoMode(screenSize.x, screenSize.y, 32), "SHURIKEN!",
    _settings->_fullscreen == "ON" ? Style::Fullscreen : Style::Default, ctxSettings);

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
        string score = _scoresOnline[i];
        if(score.length() >= 3 && score.substr(0,3) != "...")
        {
            vector<string> values = _files.readLine(score);
            Text onlineRank(values[0], _font, 16);
            onlineRank.setPosition(40,55+i*25);
            Text nickname(values[1], _font, 16);
            nickname.setPosition(80,55+i*25);
            Text scoreValue(values[2], _font, 16);
            scoreValue.setPosition(350,55+i*25);
            scoreValue.setColor(Color(234,93,240));
            _window->draw(onlineRank);
            _window->draw(nickname);
            _window->draw(scoreValue);
        }
        else
        {
            Text text(score, _font, 16);
            text.setPosition(40,55+i*25);
            _window->draw(text);
        }
    }
    _window->draw(_leaderBoardText);
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

    //Main switch
    switch(_gameState)
    {
        case mainmenu:
            _window->draw(_titleScreenTitle);
            _mainMenu->Display(_window, _deltaTime, true);
            MainMenuEvents();
            break;
        case level:
            //Display + render last effects
            _effectBox.RenderEffects(_world, _window, _view);
            _squareShape.setScale(4, 4);
            _window->draw(_squareShape);
            _window->draw(_gameOverText);

            if(_levelTimer == 160)
                _soundBox.PlaySound(SND_LEVEL, false);

            //Timer
            if(_levelTimer > 0)
                _levelTimer--;
            else
            {
                _maps->LoadMap(_currentLevel);
                _scoreManager.Reset(_maps->GetEnemiesToKill(), _maps->GetEnemiesToMiss(), _settings->_language, false, _currentLevel == 0);
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
                if(!_ninjas[0]->IsSpawning())
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

            _scoreManager.Display(_window);
            for(int i=0 ; i<_ninjas.size() ; i++)
                _ninjas[i]->DrawLife(_window);

            //Game over or victory
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
                    _levelTimer = 180;

                     //Changing UI texts
                    if(_victoryOrDefeat == 2 || _currentLevel == 3)
                    {
                        stringstream stream;
                        stream << _scoreManager.GetScore();
                        string id = _victoryOrDefeat == 1 ? "scorefinal": "gameover";
                        string text = xmlReader::getTranslationText("other", _settings->_language, id);
                        text += _victoryOrDefeat == 1 ? " : " + stream.str() : "";
                        _gameOverText.setString(text);
                        _gameOverText.setPosition(420-(_gameOverText.getLocalBounds().width/2), 240);
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

                    if(_currentLevel < 3 || _victoryOrDefeat == 2)
                        _gameState = _victoryOrDefeat == 1 ? level : gameOver;
                    else
                    {
                        _soundBox.PlaySound(SND_VICTORY, false);
                        _gameState = score;
                    }
                }
            }
            break;
        case pause:
            PauseMenuEvents();
            _pauseMenu->Display(_window, _deltaTime);
            break;
        case gameOver:
            _gameOverTimer -= _gameOverTimer > 0 ? 1 : 0;
            _window->draw(_gameOverText);

            if(_gameOverTimer == 0)
                _gameState = mainmenu;
            break;
        case score:
            _window->draw(_gameOverText);
            _window->draw(_scoreEnterText);

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
            _nicknameText.setString(_nickname + string(20-_nickname.length(), '_'));
            _nicknameText.setPosition(420-(_nicknameText.getLocalBounds().width/2), 320);
            _window->draw(_nicknameText);
            break;
        case leaderboard:
            DisplayLeaderboard();

            //Loading scores
            if(_scoreToLoad >= 0 && _scoreToLoad < 20)
            {
                _scoresOnline[19-_scoreToLoad] = GetScoreOnline(19-_scoreToLoad);
                _soundBox.PlaySound(SND_LOAD, false);
            }
            _scoreToLoad -= _scoreToLoad >= 0 ? 1 : 0;
            break;
    }

    //DEBUG - Don't work anymore with Box2d v2.3
    _showMasks = Keyboard::isKeyPressed(Keyboard::M) ? true : _showMasks;
    _showMasks = Keyboard::isKeyPressed(Keyboard::L) ? false : _showMasks;

    if(_showMasks && _gameState == game)
        Box2DTools::DrawCollisionMask(_world, _window);
    }
    catch(exception e)
    {
        throw e;
        cout << e.what() << endl;
    }
}
