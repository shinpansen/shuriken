#include "projectile.hpp"

static const float SCALE = 30.f;
static const double PI = 3.14159265359;

static const uint16 GROUND_ENTITY = 0x0001;
static const uint16 PLATFORM_ENTITY = 0x0002;
static const uint16 HERO_ENTITY = 0x0004;
static const uint16 ENEMY_ENTITY = 0x0008;
static const uint16 SHURIKEN_ENTITY = 0x0016;
static const uint16 PROJECTILE_ENTITY = 0x0032;
static const uint16 GLASS_ENTITY = 0x0064;
static const uint16 FIRE_ENTITY = 0x0128;
static const uint16 SPEEDWALK_ENTITY = 0x0256;

using namespace std;
using namespace sf;

projectile::projectile()
{
}

projectile::projectile(b2World* world, effects* effectBox, sound* soundBox)
{
    _dommagesRatio = 1;
    _world = world;
    _effectBox = effectBox;
    _soundBox = soundBox;
    if (!_projectileTexture.loadFromFile(_files.projectileTile))
        cout << "Unable to find projectile texture '" << _files.projectileTile << "'" << endl;

    //First projectile to run the machine
    AddProjectile(Vector2i(-19,-19), 0.3, 1, 1, 6, 1, 0, Vector2f(0, 0), 0);
    _projectileBodies[0]->SetActive(false);
}

void projectile::AddProjectile(Vector2i pos, float frameDelay, int width, int height, int nbFrames,
                               int numAnimation, int angle, Vector2f speed, int type)
{
    try
    {
        //Create spell objects
        _tiles.push_back(new Sprite());
        _animations.push_back(new animation());
        _speedOfProjectile.push_back(speed);
        _lifeOfProjectile.push_back(type <= 0 ? 100 : 300);

        //Settings
        _tiles[_tiles.size()-1]->setTexture(_projectileTexture);
        _tiles[_tiles.size()-1]->setOrigin(width/2, height/2);
        _tiles[_tiles.size()-1]->setPosition(pos.x, pos.y);
        _tiles[_tiles.size()-1]->setRotation(angle);

        //Type of hit box
        if(type <= 0)
            _projectileBodies.push_back(Box2DTools::CreateCircleBox(_world, pos, 25, 0.5, 1, 5));
        else
            _projectileBodies.push_back(Box2DTools::CreateRectangleBox(_world, pos, 21, 12, 0.5, 1, 0, 6));

        _projectileBodies[_projectileBodies.size()-1]->SetLinearVelocity(b2Vec2(speed.x, speed.y));
        _projectileBodies[_projectileBodies.size()-1]->ApplyForce(b2Vec2(0.f,-70.f*_projectileBodies[_projectileBodies.size()-1]->GetMass()), _projectileBodies[_projectileBodies.size()-1]->GetWorldCenter(), false);
        _projectileBodies[_projectileBodies.size()-1]->SetTransform(b2Vec2(pos.x/SCALE, pos.y/SCALE), angle*PI/180);
        *_animations[_animations.size()-1] =
            animation(_tiles[_tiles.size()-1], frameDelay, width, height, nbFrames, 5, -1, true);
        _animations[_animations.size()-1]->SetAnimation(type+1, 0);
    }
    catch(exception e)
    {
        cout << "An error occurred while adding projectile : " << e.what() << endl;
    }
}

void projectile::DestroyProjectile(int i, bool showEffect, bool playSound)
{
    if(i > 0 && i < _projectileBodies.size())
    {
        if(_projectileBodies[i] == NULL)
            return;

        float x = _projectileBodies[i]->GetPosition().x*SCALE;
        float y = _projectileBodies[i]->GetPosition().y*SCALE;
        int numAnimation = _animations[i]->GetAnimation();
        _world->DestroyBody(_projectileBodies[i]);
        _projectileBodies[i] = NULL;
        _projectileBodies.erase(_projectileBodies.begin()+i);
        delete _animations[i];
        delete _tiles[i];
        _animations.erase(_animations.begin()+i);
        _tiles.erase(_tiles.begin()+i);
        _speedOfProjectile.erase(_speedOfProjectile.begin()+i);
        _lifeOfProjectile.erase(_lifeOfProjectile.begin()+i);

        if(showEffect)
            _effectBox->AddEffect(Vector2i(x,y), 0.3, 75, 75, 6, 0, false, 1);
        if(playSound)
            _soundBox->PlaySound(SND_HIT, false);
    }
}

void projectile::DestroyAllProjectiles()
{
    while(_projectileBodies.size() > 1)
        DestroyProjectile(1, false, false);
}

void projectile::DestroyLine(int posY)
{
    //Destroying enemies
    for(int i=0 ; i<_enemies->size() ; i++)
        if(fabs(posY - (*_enemies)[i]->GetPosition().y) < 50)
            (*_enemies)[i]->SetDommages(999);

    //Destroying projectiles
    while(ProjectileInLine(posY))
    {
        for(int i=0 ; i<_projectileBodies.size() ; i++)
        {
            if(_projectileBodies[i] == NULL)
                continue;
            if(fabs(posY - _projectileBodies[i]->GetPosition().y*SCALE) < 50)
            {
                DestroyProjectile(i, true, false);
                break;
            }
        }
    }
}

bool projectile::ProjectileInLine(int posY)
{
    for(int i=0 ; i<_projectileBodies.size() ; i++)
        if(_projectileBodies[i] != NULL && fabs(posY - _projectileBodies[i]->GetPosition().y*SCALE) < 50)
            return true;
    return false;
}

void projectile::ChangeDommages(float value)
{
    if(value < 1)
        _dommagesRatio = 1;
    else
        _dommagesRatio = _dommagesRatio+value < 4 ? _dommagesRatio+value : 4;
}

int projectile::GetDommagesRatio()
{
    return _dommagesRatio;
}

void projectile::UpdatePointers(vector<hero*>* ninjas, vector<enemy*>* enemies)
{
    _ninjas = ninjas;
    _enemies = enemies;
}

void projectile::PerformContacts(b2Body* projectile, Vector2f* speedOfProjectile, int projectileNumber)
{
    try
    {
        for (b2Contact* contact = _world->GetContactList() ; contact ; contact = contact->GetNext())
        {
            if(!contact->IsTouching())
                continue;

            int projectileNumberContact;
            //Ground contact
            if((contact->GetFixtureA()->GetBody() == projectile ||
                contact->GetFixtureB()->GetBody() == projectile) &&
               (contact->GetFixtureA()->GetFilterData().categoryBits == GROUND_ENTITY ||
                contact->GetFixtureB()->GetFilterData().categoryBits == GROUND_ENTITY ||
                contact->GetFixtureA()->GetFilterData().categoryBits == SPEEDWALK_ENTITY ||
                contact->GetFixtureB()->GetFilterData().categoryBits == SPEEDWALK_ENTITY))
            {
                DestroyProjectile(projectileNumber, true, true);
                break;
            }
            //Contact with dynamic body
            else if(contact->GetFixtureA()->GetBody() == projectile ||
                    contact->GetFixtureB()->GetBody() == projectile)
            {
                for(int i=0 ; i<_enemies->size() ; i++)
                {
                    if((*_enemies)[i]->IsAnEnemy(contact->GetFixtureA()->GetBody(), contact->GetFixtureB()->GetBody()))
                    {
                        float dommageBase = fabs(_speedOfProjectile[projectileNumber].x) > 20 ? 2.f : 1.f;
                        DestroyProjectile(projectileNumber, true, true);
                        (*_enemies)[i]->SetDommages(dommageBase*_dommagesRatio);
                    }
                }

                for(int i=0 ; i<_ninjas->size() ; i++)
                {
                    if((*_ninjas)[i]->isAHero(contact->GetFixtureA()->GetBody(), contact->GetFixtureB()->GetBody()))
                    {
                        int signe = speedOfProjectile->x > 0 ? 1 : -1;
                        DestroyProjectile(projectileNumber, true, true);
                        (*_ninjas)[i]->SetStunned(Vector2f(signe*15.f, -10.f), 15);
                    }
                }
            }
        }
    }
    catch(exception e) { cout << e.what() << endl; }
}

bool projectile::isAProjectile(b2Body* projectile, int* projectileNumber)
{
    for(int i=1 ; i<_projectileBodies.size() ; i++)
    {
        if(_projectileBodies[i] != NULL && projectile == _projectileBodies[i])
        {
            *projectileNumber = i;
            return true;
        }
    }
    return false;
}

bool projectile::AboveAProjectile(Vector2f pos, Vector2f* projectilePos)
{
    for(int i=1 ; i<_projectileBodies.size() ; i++)
    {
        if(_projectileBodies[i] != NULL)
        {
            uint16 category = _projectileBodies[i]->GetFixtureList()[0].GetFilterData().categoryBits;
            int x = _projectileBodies[i]->GetPosition().x*SCALE;
            int y = _projectileBodies[i]->GetPosition().y*SCALE;
            if(category == PROJECTILE_ENTITY && pos.y < y && fabs(pos.y - y) < 70 && fabs(pos.x - x) < 20)
            {
                *projectilePos = Vector2f(x+_speedOfProjectile[i].x*5, y-15); // Position in 5 frames
                return true;
            }
        }
    }
    return false;
}

void projectile::RenderProjectiles(float deltaTime)
{
    try
    {
        _deltaTime = deltaTime;
        //projectiles encountering collision
        for(int i=1 ; i<_projectileBodies.size() ; i++)
            PerformContacts(_projectileBodies[i], &_speedOfProjectile[i], i);

        //projectiles vs projectiles
        for (b2Contact* contact = _world->GetContactList() ; contact ; contact = contact->GetNext())
        {
            int projectileA;
            int projectileB;
            if(contact->IsTouching() &&
               isAProjectile(contact->GetFixtureA()->GetBody(), &projectileA) &&
               isAProjectile(contact->GetFixtureB()->GetBody(), &projectileB))
            {
                DestroyProjectile(projectileA, true, true);
                DestroyProjectile(projectileB < projectileA ? projectileB : projectileB-1, true, false);
            }
        }

        for(int i=1 ; i<_tiles.size() ; i++)
        {
            if(_projectileBodies[i] == NULL)
                continue;

            //Variables
            float x = _projectileBodies[i]->GetPosition().x*SCALE;
            float y = _projectileBodies[i]->GetPosition().y*SCALE;
            float speed = fabs(_projectileBodies[i]->GetLinearVelocity().x)+fabs(_projectileBodies[i]->GetLinearVelocity().y);

            _projectileBodies[i]->SetLinearVelocity(b2Vec2(_speedOfProjectile[i].x, _speedOfProjectile[i].y));
            _projectileBodies[i]->ApplyForce(b2Vec2(0.f,-70.f*_projectileBodies[i]->GetMass()), _projectileBodies[i]->GetWorldCenter(), false); //Remove gravity
            _animations[i]->PerformAnimation(1);
            _tiles[i]->setPosition(x, y);
        }

        //Deleting old projectiles
        for(int i=1 ; i<_lifeOfProjectile.size() ; i++)
        {
            _lifeOfProjectile[i]--;
            if(_lifeOfProjectile[i] <= 0)
            {
                DestroyProjectile(i, false, false);
                break;
            }
        }
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void projectile::Display(RenderWindow* window)
{
    //Drawing spells
    for(int i=1 ; i<_tiles.size() ; i++)
        window->draw(*_tiles[i]);
}
