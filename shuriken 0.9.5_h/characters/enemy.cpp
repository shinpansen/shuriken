#include "enemy.hpp"

static const float SCALE = 30.f;
static const float RUNNING_SPEED = 5.f;
static const float FALLING_SPEED = 7.f;
static const float GRAVITY = 23.f;
static const int JUMPING_TIMER = 160;

static const uint16 GROUND_ENTITY = 0x0001;
static const uint16 PLATFORM_ENTITY = 0x0002;
static const uint16 HERO_ENTITY = 0x0004;
static const uint16 ENEMY_ENTITY = 0x0008;
static const uint16 SHURIKEN_ENTITY = 0x0016;
static const uint16 PROJECTILE_ENTITY = 0x0032;
static const uint16 GLASS_ENTITY = 0x0064;
static const uint16 FIRE_ENTITY = 0x0128;
static const uint16 SPEEDWALK_ENTITY = 0x0256;

static const float NATIVE_WIDTH = 280;
static const float NATIVE_HEIGHT = 192;

using namespace std;
using namespace sf;

enemy::enemy()
{
}

enemy::~enemy()
{
    if(_body != NULL && _bodyClone != NULL)
    {
        //Destroying Box2D objects
        _world->DestroyBody(_body);
        _world->DestroyBody(_bodyClone);
        _body = NULL;
        _bodyClone = NULL;
    }
}

void enemy::KillYourself() //it'll only take a minute
{
    //Removing enemy from enemy list
    for(int i=0 ; i<(*_enemies).size() ; i++)
    {
        if((*_enemies)[i]->GetBodyA() == _body)
        {
            _enemies->erase(_enemies->begin()+i);
            break;
        }
    }

    //Destroying box2d bodies
    if(_body != NULL && _bodyClone != NULL)
    {
        //Destroying Box2D objects
        _world->DestroyBody(_body);
        _world->DestroyBody(_bodyClone);
        _body = NULL;
        _bodyClone = NULL;
    }
    //delete this; //Not safe from now
}

enemy::enemy(Vector2i hitbox, Vector2i tile, Vector2i origin, int maxLife, int maskType,
          ENEMY_TYPE type, Vector2i pos, b2World* world, effects* effectBox, projectile* projectileBox,
          sound* soundBox, scoreManager* scoreManager, powerups* powerups, Texture* textureEnemy,
          Texture* textureParachute)
{
    //Variable
    _dead = false;
    _type = type;
    _isArriving = true;
    _onTheFloor = false;
    _lastOnTheFloor = false;
    _ignorePlatform = false;
    _attacking = 0;
    _attackingCooldown = 360;
    _turning = 0;
    _hit = 0;
    _jumpingTimer = JUMPING_TIMER;
    _pointsReward = 1;
    _deltaTime = 1;

    //Vectors
    _initPos = pos;
    _speed = Vector2f(0.f, 0.f); //In Box2D unit
    _maxSpeed = Vector2f(RUNNING_SPEED, FALLING_SPEED);
    _position = Vector2f(pos.x, pos.y); //In pixel unit
    _positionClone = Vector2f(pos.x, pos.y); //In pixel unit

    //Objects
    _effectBox = effectBox;
    _projectileBox = projectileBox;
    _soundBox = soundBox;
    _scoreManager = scoreManager;
    _powerups = powerups;

    //Parameters
    _hitboxWidth = hitbox.x;
    _hitboxHeight = hitbox.y;
    _tileWidth = tile.x;
    _tileHeight = tile.y;
    _originX = origin.x;
    _originY = origin.y;
    _maxLife = maxLife;
    _life = maxLife;
    _maskType = maskType;

    //Box2D
    _world = world;
    _body = Box2DTools::CreateRectangleBox(_world, pos, _hitboxWidth, _hitboxHeight, 0.1, 0.f, 0, _maskType);
    _body->SetTransform(_body->GetPosition(), 0);
    _bodyClone = Box2DTools::CreateRectangleBox(_world, pos, _hitboxWidth, _hitboxHeight, 0.1, 0.f, 0, _maskType);
    _bodyClone->SetActive(false);

    //Sprites
    _texture = textureEnemy;
    _textureParachute = textureParachute;
    _sprite.setTexture(*_texture);
    _sprite.setOrigin(_originX, _originY);
    _spriteClone.setTexture(*_texture);
    _spriteClone.setOrigin(_sprite.getOrigin());
    _parachute.setTexture(*_textureParachute);
    _parachute.setOrigin(_parachute.getLocalBounds().width/2,_parachute.getLocalBounds().height);
    _animation = animation(&_sprite, 0.1, _tileWidth, _tileHeight, 2, 4, 0);
    _animation.ReverseSprite(pos.x > NATIVE_WIDTH/2);

    if(_isArriving)
    {
        _sprite.setTextureRect(IntRect(0,0,_tileWidth, _tileHeight));
        _sprite.setScale(pos.x > NATIVE_WIDTH/2 ? -1 : 1, 1);
    }
}

void enemy::Update()
{
    if(_body == NULL || _bodyClone == NULL)
        return;

    //Body speed + position
    _speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
    _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
    _positionClone = Vector2f(_bodyClone->GetPosition().x*SCALE, _bodyClone->GetPosition().y*SCALE);

    //Attack duration
    _attacking -= _attacking > 0 ? 1 : 0;
    _attackingCooldown -= _attackingCooldown > 0 ? 1 : 0;

    //Hit duration
    _hit -= _hit > 0 ? 1 : 0;

    //Limiting falling speed
    if(_body->GetLinearVelocity().y > _maxSpeed.y)
        _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x, _maxSpeed.y));

    //Body speed + position (re-update)
    _speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
    _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);

    //Turning
    _turning -= _turning > 0 ? 1 : 0;

    //Testing if body on the floor
    _lastOnTheFloor = _onTheFloor;
    _onTheFloor = false;
    float middleWidth = ((float)_hitboxWidth)/2;
    _collisionType = -1;
    b2Body* bodyTest;
    for(double i=_position.x-middleWidth ; i<_position.x+middleWidth ; i+=0.1)
    {
        if(Box2DTools::TestCollisionPoint(_world, b2Vec2(i/SCALE, (_position.y+(_hitboxHeight/2)+2)/SCALE), true,
                                          &_collisionType, &bodyTest))
        {
            _onTheFloor = true;
            break;
        }
    }
    if(!_lastOnTheFloor && _onTheFloor) // Fall on floor/enemy
    {
        _soundBox->PlaySound(SND_FALL, false);
        if(_collisionType == 4 && IsAnEnemy(bodyTest) && bodyTest != _body && bodyTest != _bodyClone)
            GetAnEnemy(bodyTest)->SetDommages(99); //Kill the enemy
    }

    //Clone of the sprite, if loop horizontal
    if(_position.x-_hitboxWidth <= 0)
    {
        _spriteClone.setPosition(NATIVE_WIDTH+_position.x, _position.y);
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2((NATIVE_WIDTH+_position.x)/SCALE, _position.y/SCALE),0);
    }
    else if(_position.x+_hitboxWidth >= NATIVE_WIDTH)
    {
        _spriteClone.setPosition(_position.x-NATIVE_WIDTH, _position.y);
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2((_position.x-NATIVE_WIDTH)/SCALE, _position.y/SCALE),0);
    }

    //Disabling collision of clone (if not on the edge)
    if(_position.x-_hitboxWidth > 0 && _position.x+_hitboxWidth < NATIVE_WIDTH && _position.y-30 > 0 && _position.y+30 < NATIVE_HEIGHT)
    {
        _bodyClone->SetTransform(b2Vec2(0,0),0);
        _bodyClone->SetActive(false);
    }

    //Changing direction if something in front
    int signe = !_animation.GetReversed() ? 1 : -1;
    int spaceInFront = _hitboxWidth/2 + 2;
    b2Vec2 pointA = b2Vec2(_body->GetPosition().x+(signe*spaceInFront/SCALE), _body->GetPosition().y-(5/SCALE));
    b2Vec2 pointB = b2Vec2(_body->GetPosition().x+(signe*spaceInFront/SCALE), _body->GetPosition().y-(2/SCALE));
    int collisionPoint;
    if((Box2DTools::TestCollisionPoint(_world, pointA, true, &collisionPoint, &bodyTest) ||
        Box2DTools::TestCollisionPoint(_world, pointB, true, &collisionPoint, &bodyTest)) &&
       (IsAnEnemy(bodyTest) || collisionPoint == 9))
    {
        _turning = 10;
        _animation.ReverseSprite(!_animation.GetReversed());
    }

    //Killing enemy if running out of life
    if(_life <= 0 && _body != NULL && _body->IsActive())
    {
        _position = _sprite.getPosition();
        _effectBox->AddEffect(Vector2i(_position.x, _position.y-8), 0.20, 25, 25, 6, 2, _animation.GetReversed(), SFX_EXPLOSION);
        _soundBox->PlaySound(SND_EXPLOSION2, false);
        _body->SetLinearVelocity(b2Vec2(0,0));
        _body->SetActive(false);
        _bodyClone->SetActive(false);
        _dead = true;
        KillYourself();
    }
}

void enemy::PerformEvents() //Override in children's code
{
}

void enemy::AnimeSprite()
{
    //Perform animation + position + drawing
    if(_hit != 0)
        _animation.SetAnimation(1, 0);
    else if(_turning > 0)
        _animation.SetAnimation(3, 0);
    else if(_attacking > 0)
        _animation.SetAnimation(2, 1);
    else
        _animation.SetAnimation(0, 0);

    if(!_isArriving)
        _animation.PerformAnimation(_deltaTime);
    _sprite.setPosition(_position.x, _position.y);
}

b2Body* enemy::GetBodyA()
{
    return _body != NULL ? _body : NULL;
}

b2Body* enemy::GetBodyB()
{
    return _bodyClone != NULL && _bodyClone->IsActive() ? _bodyClone : _body;
}

enemy* enemy::GetAnEnemy(b2Body* body)
{
    for(int i=0 ; i<_enemies->size() ; i++)
    {
        if((*_enemies)[i]->GetBodyA() == body || (*_enemies)[i]->GetBodyB() == body)
            return (*_enemies)[i];
    }
    return NULL;
}

bool enemy::IsAnEnemy(b2Body* body)
{
    return _body != NULL && _bodyClone != NULL && (body == _body || body == _bodyClone);
}

bool enemy::IsAnEnemy(b2Body* bodyA, b2Body* bodyB)
{
    return _body != NULL && _bodyClone != NULL &&
           (bodyA == _body || bodyA == _bodyClone || bodyB == _body || bodyB == _bodyClone);
}

bool enemy::IsActive()
{
    return _body != NULL && _bodyClone != NULL && (_body->IsActive() || _bodyClone->IsActive());
}

Vector2f enemy::GetPosition()
{
    if(_body != NULL)
        return Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
    else
        return Vector2f(-1,-1);
}

void enemy::SetDommages(float value, float stunRecoil)
{
    //Stun
    _body->ApplyForce(b2Vec2(stunRecoil, 0.f), _body->GetWorldCenter(), false);
    _life -= _life > 0 ? value : 0;
    _hit = 5;

    //If out of life :  destroying enemy
    if(_life <= 0 && _body != NULL && _body->IsActive())
    {
        int points = _pointsReward * _scoreManager->GetMultiplicatorLevel();
        stringstream stream;
        stream << points;
        _scoreManager->AddPoints(points);
        if(_type != ENEMY_BOSS)
            _scoreManager->AddEnemyKilled(1);
        _effectBox->AddText(stream.str(), Vector2f(_position.x, _position.y-10), Vector2f(0,-0.3), 60);
        _effectBox->ShakeScreen(3);

        //Adding power up : 25-33% of chances
        bool dropPowerup = _pointsReward == 142 ? rand()%3 == 0 : rand()%4 == 0;
        if(/*42 == 42*/dropPowerup && _type != ENEMY_BOSS)
            _powerups->AddRandomPowerup(_position);
    }
}

int enemy::getLife()
{
    return _life;
}

bool enemy::IsDead()
{
    return _dead;
}

void enemy::Step()
{
    Update();
    if(_life == 0) { return; } //enemy is dead
    PerformEvents();
    AnimeSprite();
}

void enemy::Display(RenderWindow* window, float deltaTime)
{
    _deltaTime = deltaTime;
    if(_life == 0) { return; } //enemy is dead
    window->draw(_sprite);
    if(_isArriving) { window->draw(_parachute); }
    if(_position.x-30 <= 0 || _position.x+30 >= NATIVE_WIDTH)
    {
        _spriteClone.setTextureRect(_sprite.getTextureRect());
        _spriteClone.setScale(_sprite.getScale());
        _spriteClone.setColor(_sprite.getColor());
        window->draw(_spriteClone);
    }
}
