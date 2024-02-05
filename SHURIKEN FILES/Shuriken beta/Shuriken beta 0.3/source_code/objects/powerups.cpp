#include "powerups.hpp"

static const float SCALE = 30.f;
static const int POWERUP_SIZE = 33;

using namespace std;
using namespace sf;

powerups::powerups(vector<hero*>* ninjas, effects* effectBox, sound* soundBox, b2World* world)
{
    //Objects
    _ninjas = ninjas;
    _effectBox = effectBox;
    _soundBox = soundBox;
    _world = world;

    //Graphics
    if (!_texturePowerUps.loadFromFile(_files.powerUps))
    {
        cout << "Unable to find power ups texture. Stopping execution." << endl;
        exit(1);
    }
}

void powerups::AddPowerUp(Vector2f pos, int type)
{
    _powerups.push_back(new Sprite);
    _powerupsTimer.push_back(600);
    _powerups[_powerups.size()-1]->setTexture(_texturePowerUps);
    _powerups[_powerups.size()-1]->setTextureRect(IntRect(type*POWERUP_SIZE,0,POWERUP_SIZE,POWERUP_SIZE));
    _powerups[_powerups.size()-1]->setOrigin((float)POWERUP_SIZE/2, (float)POWERUP_SIZE/2);
    _powerups[_powerups.size()-1]->setPosition(pos);
}

void powerups::AddRandomPowerup(Vector2f pos)
{
    //If none of the powerups are usefull : cancel drop
    if(!(*_ninjas)[0]->AddLife(true) && !(*_ninjas)[0]->DoubleDommages(true) &&
       !(*_ninjas)[0]->AddSpecialAttack(true) && !(*_ninjas)[0]->AddLive(true) &&
       !(*_ninjas)[0]->DoubleShuriken(true))
        return;

    //Trying to get a random powerup usefull for hero
    int type = rand()%5;
    while((type == 0 && !(*_ninjas)[0]->AddLife(true)) ||
          (type == 1 && !(*_ninjas)[0]->DoubleDommages(true)) ||
          (type == 2 && !(*_ninjas)[0]->AddSpecialAttack(true)) ||
          (type == 3 && !(*_ninjas)[0]->AddLive(true)) ||
          (type == 4 && !(*_ninjas)[0]->DoubleShuriken(true)))
           type = rand()%5;

    //Adding powerup
    AddPowerUp(pos, type);
}

void powerups::DeletePowerUp(int i)
{
    if(i >= 0 && i < _powerups.size())
    {
        Vector2i pos = Vector2i(_powerups[i]->getPosition().x, _powerups[i]->getPosition().y);
        delete _powerups[i];
        _powerups.erase(_powerups.begin()+i);
        _powerupsTimer.erase(_powerupsTimer.begin()+i);
        _effectBox->AddEffect(pos, 0.2, 75, 75, 6, 4, false, 0);
    }
}

void powerups::DeleteAllPowerUps()
{
    while(_powerups.size() > 0)
        DeletePowerUp(0);
}

void powerups::Render()
{
    for(int i=0; i<_powerups.size() ; i++)
    {
        //PowerUp type
        int type = _powerups[i]->getTextureRect().left/POWERUP_SIZE;

        //Falling on the ground
        b2Vec2 point = b2Vec2(_powerups[i]->getPosition().x/SCALE, (_powerups[i]->getPosition().y+17)/SCALE);
        if(!Box2DTools::TestCollisionPoint(_world, point, true, new int))
            _powerups[i]->move(0,1);

        //Destroying if timer ends
        _powerupsTimer[i]--;
        if(_powerupsTimer[i] <= 0)
        {
            DeletePowerUp(i);
            break;
        }

        //Collision with hero
        if(fabs((*_ninjas)[0]->GetPosition().x - _powerups[i]->getPosition().x) < (POWERUP_SIZE/2)+15 &&
           fabs((*_ninjas)[0]->GetPosition().y - _powerups[i]->getPosition().y) < (POWERUP_SIZE/2)+25)
        {
            if(type == 0 && (*_ninjas)[0]->AddLife())
            {
                DeletePowerUp(i);
                _soundBox->PlaySound(SND_POWERUP3, false);
            }
            else if(type == 1 && (*_ninjas)[0]->DoubleDommages())
            {
                DeletePowerUp(i);
                _soundBox->PlaySound(SND_POWERUP4, false);
            }
            else if(type == 2 && (*_ninjas)[0]->AddSpecialAttack())
            {
                DeletePowerUp(i);
                _soundBox->PlaySound(SND_POWERUP5, false);
            }
            else if(type == 3 && (*_ninjas)[0]->AddLive())
            {
                DeletePowerUp(i);
                _soundBox->PlaySound(SND_POWERUP6, false);
            }
            else if(type == 4 && (*_ninjas)[0]->DoubleShuriken())
            {
                DeletePowerUp(i);
                _soundBox->PlaySound(SND_POWERUP7, false);
            }
            else
                continue;
            break;
        }
    }
}

void powerups::Display(RenderWindow* window)
{
    Render();
    for(int i=0; i<_powerups.size() ; i++)
    {
        //PowerUp blinking if about to disappear
        if(_powerupsTimer[i] > 180 || _powerupsTimer[i] % 10 <= 5)
            window->draw(*_powerups[i]);
    }
}
