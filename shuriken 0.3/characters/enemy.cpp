#include "enemy.hpp"

static const float SCALE = 30.f;
static const float RUNNING_SPEED = 5.f;
static const float FALLING_SPEED = 7.f;
static const int JUMPING_TIMER = 120;

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
    delete &_sprite;
    delete &_parachute;
    delete &_spriteClone;
}

void enemy::KillYourself() // it'll only take a minute
{
    //Removing enemy from enemy list
    for(int i=0 ; i<_enemies->size() ; i++)
    {
        if((*_enemies)[i]->GetBodyA() == _body)
        {
            _enemies->erase(_enemies->begin()+i);
            break;
        }
    }

    delete this;
}

enemy::enemy(int type, Vector2i pos, b2World* world, effects* effectBox, projectile* projectileBox,
          sound* soundBox, scoreManager* scoreManager, powerups* powerups, Texture* textureEnemy,
          Texture* textureParachute)
{
    _isdead = false;

    //Variable
    _type = type;
    _isArriving = _type != 2;
    _onTheFloor = false;
    _lastOnTheFloor = false;
    _ignorePlatform = _type != 2 ? false : rand()%3 > 0 && pos.x < NATIVE_WIDTH/2 ? true : false;
    _attacking = 0;
    _attackingCooldown = 360;
    _turning = 0;
    _hit = 0;
    _jumpingTimer = JUMPING_TIMER;
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

    //Type of enemy
    int maskType = 4;
    switch(_type)
    {
        case 0: //Big green tank
            _hitboxWidth = 26;
            _hitboxHeight = 12;
            _tileWidth = 32;
            _tileHeight = 18;
            _maxLife = 10;
            break;
        case 1: //Small blue tank
            _hitboxWidth = 17;
            _hitboxHeight = 10;
            _tileWidth = 32;
            _tileHeight = 18;
            _maxLife = 6;
            break;
        case 2: //Clown robot
            _hitboxWidth = 8;
            _hitboxHeight = 23;
            _tileWidth = 32;
            _tileHeight = 26;
            _maxLife = 10;
            maskType = 10;
            break;
        case 3: //Clown robot
            _hitboxWidth = 32;
            _hitboxHeight = 15;
            _tileWidth = 38;
            _tileHeight = 23;
            _maxLife = 15;
            break;
        default:
            cout << "Not a valid enemy type !" << endl;
            exit(1);
            break;
    }

    //Life
    _life = _maxLife;

    //Box2D
    _world = world;
    _body = Box2DTools::CreateRectangleBox(_world, pos, _hitboxWidth, _hitboxHeight, 0.1, 0.f, 0, maskType);
    _body->SetTransform(_body->GetPosition(), 0);
    _bodyClone = Box2DTools::CreateRectangleBox(_world, pos, _hitboxWidth, _hitboxHeight, 0.1, 0.f, 0, maskType);
    _bodyClone->SetActive(false);

    //Sprites
    _texture = textureEnemy;
    _textureParachute = textureParachute;
    _sprite.setTexture(*_texture);
    if(_type < 2)
        _sprite.setOrigin(13, 12);
    else if(_type == 2)
        _sprite.setOrigin(13, 15);
    else
        _sprite.setOrigin(17, 15);
    _spriteClone.setTexture(*_texture);
    _spriteClone.setOrigin(_sprite.getOrigin());
    _parachute.setTexture(*_textureParachute);
    _parachute.setOrigin(_parachute.getLocalBounds().width/2,_parachute.getLocalBounds().height);
    _animation = animation(&_sprite, 0.1, _tileWidth, _tileHeight, 2, 4, 0);
    _animation.ReverseSprite(pos.x > NATIVE_WIDTH/2);
}

void enemy::UpdateVariables()
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
    int collisionType = -1;
    for(double i=_position.x-middleWidth ; i<_position.x+middleWidth ; i+=0.1)
    {
        b2Body* body;
        if(Box2DTools::TestCollisionPoint(_world, b2Vec2(i/SCALE, (_position.y+37)/SCALE), true,
                                          &collisionType, &body))
        {
            _onTheFloor = true;
            break;
        }
    }
    if(!_lastOnTheFloor && _onTheFloor)
        _soundBox->PlaySound(SND_FALL, false);

    //TODO : Adapter nouvelle resolution native !
    //Bug correction : stuck between box2D bodies
    if(_type == 0 && (int)_position.y == 125 && ((int)_position.x == 519 || (int)_position.x == 423))
        _body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y-(1.f/SCALE)), 0);
    else if(_type == 3 && (int)_position.y == 121 && ((int)_position.x == 431 || (int)_position.x == 527))
        _body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y-(1.f/SCALE)), 0);

    if(_type == 2)
    {
        //Jumping out of platform (LEVEL 2)
        if(collisionType == 2 && (_position.y < 50 || (_ignorePlatform && !_animation.GetReversed() && _position.y > 150)))
            _jumpingTimer = 0;

        //Jumping timer
        _jumpingTimer -= _jumpingTimer > 0 ? 1 : 0;
        if(_jumpingTimer == 0 && (collisionType != 2 || _position.y < 50 || (_ignorePlatform && !_animation.GetReversed() && _position.y > 150)))
        {
            _jumpingTimer = JUMPING_TIMER;
            _body->SetLinearVelocity(b2Vec2(0, -17));
            _soundBox->PlaySound(SND_BOUNCE, false);
        }
    }

    //Arriving
    if(_isArriving && !_onTheFloor)
    {
        //Low gravity with parachute
        _body->ApplyForce(b2Vec2(0.f,-65.f*_body->GetMass()), _body->GetWorldCenter(), false);
        _parachute.setPosition(_position.x, _position.y-20);
    }
    else
        _isArriving = false;

    if(_life <= 0 && _body != NULL && _body->IsActive()) //Killing enemy if running out of life
    {
        _position = _sprite.getPosition();
        _effectBox->AddEffect(Vector2i(_position.x, _position.y-8), 0.20, 25, 25, 6, 2, _animation.GetReversed(), 1);
        _soundBox->PlaySound(SND_EXPLOSION2, false);
        _body->SetLinearVelocity(b2Vec2(0,0));
        _body->SetActive(false);
        _bodyClone->SetActive(false);
        KillYourself();
    }
}

void enemy::PerformEffects()
{
    if(_body == NULL || _bodyClone == NULL)
        return;

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

    //Disabling collision of clone
    if(_position.x-_hitboxWidth > 0 && _position.x+_hitboxWidth < NATIVE_WIDTH && _position.y-30 > 0 && _position.y+30 < NATIVE_HEIGHT)
    {
        _bodyClone->SetTransform(b2Vec2(0,0),0);
        _bodyClone->SetActive(false);
    }

    //Enemy type parameters
    float enemySpeed, projectileSpeed;
    int projectileType;
    Vector2i projectilePos;
    switch(_type)
    {
        case 0:
            enemySpeed = 0.7;
            projectileSpeed = 1.6;
            projectilePos = Vector2i(18,4);
            projectileType = 1;
            break;
        case 1:
            enemySpeed = 0.8;
            projectileSpeed = 1.6;
            projectilePos = Vector2i(18,3);
            projectileType = 2;
            break;
        case 2:
            enemySpeed = !_onTheFloor ? 2.5 : 0;
            break;
        case 3:
            enemySpeed = 0.7;
            projectileSpeed = 1.6;
            projectilePos = Vector2i(27,10);
            projectileType = 3;
            break;
    }

    //Moving
    int movingSpeed = !_animation.GetReversed() ? enemySpeed : -enemySpeed;
    _body->SetLinearVelocity(b2Vec2((!_isArriving && _turning == 0)*movingSpeed, _body->GetLinearVelocity().y));

    //Attacking
    int signe = !_animation.GetReversed() ? 1 : -1;
    if(_type != 2 && _attackingCooldown == 0 && _turning == 0)
    {
        _attacking = 20;
        _attackingCooldown = 360;
        int angle = _animation.GetReversed()*180;
        Vector2i projectilePosFinal = Vector2i(_position.x+(signe*projectilePos.x), _position.y-projectilePos.y);
        _projectileBox->AddProjectile(projectilePosFinal, 0.2, 13, 13, 4, 2, angle, Vector2f(signe*projectileSpeed,0.f), projectileType);
        _soundBox->PlaySound(SND_SHOOT2, false);
    }

    //Changing direction if something in front
    int spaceInFront = _hitboxWidth/2 + 2;// _type != 2 ? 45 : 25;
    b2Vec2 pointA = b2Vec2(_body->GetPosition().x+(signe*spaceInFront/SCALE), _body->GetPosition().y-(5/SCALE));
    b2Vec2 pointB = b2Vec2(_body->GetPosition().x+(signe*spaceInFront/SCALE), _body->GetPosition().y-(2/SCALE));
    int collisionPoint;
    b2Body* bodyTest;
    if((Box2DTools::TestCollisionPoint(_world, pointA, true, &collisionPoint, &bodyTest) ||
        Box2DTools::TestCollisionPoint(_world, pointB, true, &collisionPoint, &bodyTest)) &&
       (IsAnEnemy(bodyTest) || collisionPoint == 9))
    {
        _turning = 10;
        _animation.ReverseSprite(!_animation.GetReversed());
    }
}

void enemy::AnimeSprite()
{
    //Perform animation + position + drawing
    if(_hit != 0)
        _animation.SetAnimation(_type != 2 ? 1 : _onTheFloor ? 2 : 3, 0);
    else if(_type == 2 && !_onTheFloor)
         _animation.SetAnimation(1, 0);
    else if(_type != 2 && _turning > 0)
        _animation.SetAnimation(3, 0);
    else if(_type != 2 && _attacking > 0)
        _animation.SetAnimation(2, 1);
    else
        _animation.SetAnimation(0, 0);
    _animation.PerformAnimation(_deltaTime);
    _sprite.setPosition(_position.x, _position.y);//_type != 2 ? _position.y+20 : _position.y+10);
}

b2Body* enemy::GetBodyA()
{
    return _body != NULL ? _body : NULL;
}

b2Body* enemy::GetBodyB()
{
    return _bodyClone != NULL && _bodyClone->IsActive() ? _bodyClone : _body;
}

bool enemy::IsAnEnemy(b2Body* body)
{
    for(int i=0 ; i<_enemies->size() ; i++)
    {
        if((*_enemies)[i]->GetBodyA() == body || (*_enemies)[i]->GetBodyB() == body)
            return true;
    }
    return false;
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

bool enemy::IsAMovingTank()
{
    return !_isArriving && _turning == 0 && _type != 2;
}

Vector2f enemy::GetPosition()
{
    if(_body != NULL)
        return Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
    else
        return Vector2f(-1,-1);
}

void enemy::SetDommages(float value)
{
    //Stun
    _life -= _life > 0 ? value : 0;
    _hit = 5;

    //If out of life :  destroying enemy
    if(_life <= 0 && _body != NULL && _body->IsActive())
    {
        int points = (_type < 2 ? 100/(_type+1) : 142)  * _scoreManager->GetMultiplicatorLevel();
        stringstream stream;
        stream << points;
        _scoreManager->AddPoints(points);
        _scoreManager->AddEnemyKilled(1);
        _effectBox->AddText(stream.str(), Vector2f(_position.x, _position.y-10), Vector2f(0,-1), 60);
        _effectBox->ShakeScreen(10);

        //Adding power up : 25% of chances
        bool dropPowerup = _type == 2 ? rand()%3 == 0 : rand()%4 == 0;
        if(/*42 == 42*/dropPowerup)
            _powerups->AddRandomPowerup(_position);
    }
}

int enemy::GetType()
{
    return _type;
}

int enemy::getLife()
{
    return _life;
}

void enemy::Step()
{
    UpdateVariables();
    if(_life == 0) { return; } //enemy is dead
    PerformEffects();
    AnimeSprite();
}

void enemy::DisableBody()
{
    if(_isdead)
        return;
    if(_body != NULL && _bodyClone != NULL)
    {
        _body->SetActive(false);
        _bodyClone->SetActive(false);
    }
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
