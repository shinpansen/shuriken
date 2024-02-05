#include "maps.hpp"

using namespace std;
using namespace sf;

static const float NATIVE_WIDTH = 280;
static const float NATIVE_HEIGHT = 192;

maps::maps()
{
}

maps::maps(b2World* world, effects* effectBox, sound* soundBox, scoreManager* scoreManager,
           settings* settings, vector<hero*>* ninjas, int level)
{
    _world = world;
    _effectBox = effectBox;
    _soundBox = soundBox;
    _scoreManager = scoreManager;
    _settings = settings;
    _ninjas = ninjas;
    _level = level;
    _spawnTimer = 180;

    //Resources
    if (!_layer1.loadFromFile(_files.layer1))
    {
        cout << "Unable to find map texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureTank1.loadFromFile(_files.tankTile))
    {
        cout << "Unable to find tank 1 texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureTank2.loadFromFile(_files.tank2Tile))
    {
        cout << "Unable to find tank 2 texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureTank3.loadFromFile(_files.tank3Tile))
    {
        cout << "Unable to find tank 3 texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureClown.loadFromFile(_files.clownTile))
    {
        cout << "Unable to find clown texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureParachute.loadFromFile(_files.parachute))
    {
        cout << "Unable to find parachute texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_texturePlatform.loadFromFile(_files.platform))
    {
        cout << "Unable to find platform texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureFire.loadFromFile(_files.fire))
    {
        cout << "Unable to find fire texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureWheel.loadFromFile(_files.speedwalk_wheel))
    {
        cout << "Unable to find speedwalk wheel texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureCarpet.loadFromFile(_files.speedwalk_carpet))
    {
        cout << "Unable to find speedwalk carpet texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureSwitch.loadFromFile(_files.switchTile))
    {
        cout << "Unable to find switch texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureBlocker.loadFromFile(_files.blocker))
    {
        cout << "Unable to find blocker texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureWarning.loadFromFile(_files.warning))
    {
        cout << "Unable to find warning bubble texture. Stopping execution." << endl;
        exit(1);
    }

    //Portal positions
    _level1Portals.push_back(Vector2f(820, 500));
    _level1Portals.push_back(Vector2f(25, 500));
    _level2Portals.push_back(Vector2f(820, 120));
    _level2Portals.push_back(Vector2f(25, 120));
    _level2Portals.push_back(Vector2f(820, 480));
    _level3Portals.push_back(Vector2f(25, 500));
    _level3Portals.push_back(Vector2f(25, 100));

    //Enemies to killed / allowed to miss
    _enemiesToKill[0] = 30;
    _enemiesToKill[1] = 25;
    _enemiesToKill[2] = 20;
    _enemiesToMiss[0] = 3;
    _enemiesToMiss[1] = 5;
    _enemiesToMiss[2] = 6;

    //Load
    _scoreManager->Reset(_enemiesToKill[_level], _enemiesToMiss[_level], _settings->_language);
    LoadMap(_level);
}

void maps::LoadMap(int level)
{
    //Initialization
    _level = level;
    DestroyCurrentMap();
    CreateLayers();
    CreateCollisions();
    //_scoreManager->Reset(_enemiesToKill[_level], _enemiesToMiss[_level], _settings->_language, true, false);

    //Platforms
    if(_level == 1)
    {
        _platforms.push_back(new platform(Vector2i(432,100), Vector2i(0,-1), Vector2i(600,-20), _world, &_texturePlatform, true));
        _platforms.push_back(new platform(Vector2i(432,400), Vector2i(0,-1), Vector2i(600,-20), _world, &_texturePlatform, true));
        _platforms.push_back(new platform(Vector2i(47,355), Vector2i(0,1), Vector2i(600,355), _world, &_texturePlatform, true));
    }
    else if(_level == 2)
    {
        _speedwalks.push_back(new speedwalk(_ninjas, Vector2i(450, 529), -3, 4, _world, &_textureWheel, &_textureCarpet));
        _speedwalks.push_back(new speedwalk(_ninjas, Vector2i(600, 471), 3, 4, _world, &_textureWheel, &_textureCarpet));
        _speedwalks.push_back(new speedwalk(_ninjas, Vector2i(755, 411), -3, 4, _world, &_textureWheel, &_textureCarpet));
        _speedwalks.push_back(new speedwalk(_ninjas, Vector2i(755, 411), -3, 4, _world, &_textureWheel, &_textureCarpet));
        _speedwalks.push_back(new speedwalk(_ninjas, Vector2i(52, 321), 3, 6, _world, &_textureWheel, &_textureCarpet));
        _speedwalks.push_back(new speedwalk(_ninjas, Vector2i(848, 321), 3, 2, _world, &_textureWheel, &_textureCarpet));
        _platforms.push_back(new platform(Vector2i(432,100), Vector2i(0,-1), Vector2i(360,0), _world, &_texturePlatform, false));
        _fires.push_back(new fire(_ninjas, Vector2i(176,325), _world, &_textureFire));
        _blockers.push_back(new blocker(_ninjas, _effectBox, _soundBox, _world, Vector2f(128,535),
                                        Vector2f(232,535), Vector2f(368,356), Vector2f(496,164),
                                        &_textureSwitch, &_textureBlocker, &_textureWarning));
    }
}

void maps::DestroyCurrentMap()
{
    //Destroying layer 0
    while(_layer0Tiles.size() > 0)
    {
        delete _layer0Tiles[0];
        _layer0Tiles.erase(_layer0Tiles.begin());
    }

    //Destroying layer 1
    while(_layer1Tiles.size() > 0)
    {
        delete _layer1Tiles[0];
        _layer1Tiles.erase(_layer1Tiles.begin());
    }

    //Destroying collisions
    while(_bodies.size() > 0)
    {
        _world->DestroyBody(_bodies[0]);
        _bodies[0] = NULL;
        _bodies.erase(_bodies.begin());
    }

    //Destroying platforms
    while(_platforms.size() > 0)
    {
        delete _platforms[0];
        _platforms.erase(_platforms.begin());
    }

    //Destroying speedwalk
    while(_speedwalks.size() > 0)
    {
        delete _speedwalks[0];
        _speedwalks.erase(_speedwalks.begin());
    }

    //Destroying fires
    while(_fires.size() > 0)
    {
        delete _fires[0];
        _fires.erase(_fires.begin());
    }

    //Destroying blockers
    while(_blockers.size() > 0)
    {
        delete _blockers[0];
        _blockers.erase(_blockers.begin());
    }
}

void maps::CreateLayers()
{
    try
    {
        //Layer 0
        string line;
        ifstream layerMap0(_files.layer0File[_level].c_str(), ios::in);
        while(getline(layerMap0, line))
        {
            vector<float> param = files::readFloatLine(line);
            _layer0Tiles.push_back(new Sprite());
            int i = _layer0Tiles.size() - 1;
            _layer0Tiles[i]->setTexture(_layer1);
            _layer0Tiles[i]->setPosition(param[0],param[1]+2);
            _layer0Tiles[i]->setTextureRect(IntRect((int)param[2],(int)param[3],(int)param[4],(int)param[5]));
        }
        layerMap0.close();

        //Layer 1
        ifstream layerMap1(_files.layer1File[_level].c_str(), ios::in);
        while(getline(layerMap1, line))
        {
            vector<float> param = files::readFloatLine(line);
            _layer1Tiles.push_back(new Sprite());
            int i = _layer1Tiles.size() - 1;
            _layer1Tiles[i]->setTexture(_layer1);
            _layer1Tiles[i]->setPosition(param[0],param[1]+2);
            _layer1Tiles[i]->setTextureRect(IntRect((int)param[2],(int)param[3],(int)param[4],(int)param[5]));
        }
        layerMap1.close();
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void maps::CreateCollisions()
{
    try
    {
        //Collisions
        string line;
        ifstream collisionFile(_files.collisionFile[_level].c_str(), ios::in);
        while(getline(collisionFile, line))
        {
            vector<float> param = files::readFloatLine(line);
            _bodies.push_back(Box2DTools::CreateRectangleBox(_world, Vector2i(param[0], param[1]), param[2], param[3], 0.1, 0, 0, param[4]));
        }
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

int maps::GetLevel()
{
    return _level;
}

void maps::PerformEvents(vector<enemy*>* enemies, scoreManager* scoreManager, effects* effectBox,
                         projectile* projectileBox, sound* soundBox, powerups* powerups)
{
    //Spawn enemies
    _spawnTimer--;
    switch(_level)
    {
        case 0:
            if(_spawnTimer <= 0 && enemies->size() < 15 && scoreManager->GetObjectivesState() == 0)
            {
                bool doubleSpawn = rand()%2 == 0;
                int x = rand()%2 == 0 ? 120 : 720;
                int type = scoreManager->GetEnemiesKilled() < 15 ? 1 : 0;
                Texture* texture = type == 0 ? &_textureTank1 : &_textureTank2;
                if(!doubleSpawn)
                {
                    enemies->push_back(new enemy(type, Vector2i(x, 300), _world, effectBox, projectileBox, soundBox, scoreManager, powerups, texture, &_textureParachute));
                    (*enemies)[enemies->size()-1]->_enemies = enemies;
                }
                else
                {
                    enemies->push_back(new enemy(type, Vector2i(120, -100), _world, effectBox, projectileBox, soundBox, scoreManager, powerups, texture, &_textureParachute));
                    enemies->push_back(new enemy(type, Vector2i(720, -100), _world, effectBox, projectileBox, soundBox, scoreManager, powerups, texture, &_textureParachute));
                    (*enemies)[enemies->size()-1]->_enemies = enemies;
                    (*enemies)[enemies->size()-2]->_enemies = enemies;
                }
                _spawnTimer = 150 + (rand()%150);
            }
            PerformPortals(_level1Portals, enemies, scoreManager, effectBox, soundBox);
            break;
        case 1:
            if(_spawnTimer <= 0 && enemies->size() < 12 && scoreManager->GetObjectivesState() == 0)
            {
                if(rand()%2 == 0)
                    enemies->push_back(new enemy(2, Vector2i(20, 250), _world, effectBox, projectileBox, soundBox, scoreManager, powerups, &_textureClown, &_textureParachute));
                else
                    enemies->push_back(new enemy(2, Vector2i(820, 250), _world, effectBox, projectileBox, soundBox, scoreManager, powerups, &_textureClown, &_textureParachute));
                (*enemies)[enemies->size()-1]->_enemies = enemies;
                _spawnTimer = (scoreManager->GetEnemiesKilled() < 13 ? 200 : 100) + (rand()%150);
            }
            PerformPortals(_level2Portals, enemies, scoreManager, effectBox, soundBox);
            break;
        case 2:
            int type = scoreManager->GetEnemiesKilled() < 12 ? 0 : 3;
            Texture* texture = type == 0 ? &_textureTank1 : &_textureTank3;
            if(_spawnTimer <= 0 && enemies->size() < 10 && scoreManager->GetObjectivesState() == 0)
            {
                int* nb = NbEnemiesByZone(enemies);
                if(rand()%2 == 0 && nb[1] < 1)
                    enemies->push_back(new enemy(type, Vector2i(775, 55), _world, effectBox, projectileBox, soundBox, scoreManager, powerups, texture, &_textureParachute));
                else if(nb[0] < 2)
                    enemies->push_back(new enemy(type, Vector2i(55, 200), _world, effectBox, projectileBox, soundBox, scoreManager, powerups, texture, &_textureParachute));
                (*enemies)[enemies->size()-1]->_enemies = enemies;
                _spawnTimer = (scoreManager->GetEnemiesKilled() < 13 ? 200 : 100) + (rand()%150);
            }
            PerformPortals(_level3Portals, enemies, scoreManager, effectBox, soundBox);
            break;
    }
}

void maps::PerformPortals(vector<Vector2f> portalPos, vector<enemy*>* enemies, scoreManager* scoreManager,
                          effects* effectBox, sound* soundBox)
{
    for(int j=0 ; j<portalPos.size() ; j++)
    {
        for(int i=0 ; i<enemies->size() ; i++)
        {
            if((*enemies)[i] == NULL || (*enemies)[i]->GetBodyA() == NULL || (*enemies)[i]->GetBodyB() == NULL)
                continue;

            if(fabs((*enemies)[i]->GetPosition().x - portalPos[j].x) < 10 &&
               fabs((*enemies)[i]->GetPosition().y - portalPos[j].y) < 50)
            {
                delete (*enemies)[i];
                enemies->erase(enemies->begin()+i);
                int effectPos = portalPos[j].x > 420 ? portalPos[j].x - 20 : portalPos[j].x + 20;
                effectBox->AddEffect(Vector2i(effectPos, portalPos[j].y), 0.2, 75, 75, 6, 3, false, 0);
                scoreManager->AddEnemyMissed(1);
                soundBox->PlaySound(SND_POWERUP2, false);
                break;
            }
        }
    }
}

int* maps::NbEnemiesByZone(vector<enemy*>* enemies)
{
    // FOR LEVEL 3 ONLY
    // Count the number of enemies in each spawn zone (middle left and top right)

    //Declaration
    int* nb = new int[2];
    nb[0] = 0;
    nb[1] = 0;

    //Loop
    for(int i=0 ; i<enemies->size() ; i++)
    {
        Vector2f enemyPos = (*enemies)[i]->GetPosition();
        if(enemyPos.x <= 335 && enemyPos.y <= 360 && enemyPos.y > 165)
            nb[0]++;
        else if(enemyPos.x >= 530 && enemyPos.y <= 165)
            nb[1]++;
    }
    return nb;
}

int maps::GetEnemiesToKill()
{
    return _enemiesToKill[_level];
}

int maps::GetEnemiesToMiss()
{
    return _enemiesToMiss[_level];
}

void maps::DisplayLayer0(RenderWindow* window)
{
    //Blockers
    for(int i=0 ; i<_blockers.size() ; i++)
        _blockers[i]->Display(window);

    //Platforms
    for(int i=0 ; i<_platforms.size() ; i++)
        _platforms[i]->Display(window);

    //Speedwalks
    for(int i=0 ; i<_speedwalks.size() ; i++)
        _speedwalks[i]->Display(window);

    //Fires
    for(int i=0 ; i<_fires.size() ; i++)
        _fires[i]->Display(window);

    //Layer 0
    for(int i=0 ; i<_layer0Tiles.size() ; i++)
        window->draw(*_layer0Tiles[i]);
}

void maps::DisplayLayer1(RenderWindow* window)
{
    //Layer 1
    for(int i=0 ; i<_layer1Tiles.size() ; i++)
        window->draw(*_layer1Tiles[i]);
}
