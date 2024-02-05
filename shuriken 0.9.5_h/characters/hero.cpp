#include "hero.hpp"

static const float SCALE = 30.f;
static const float WALKING_SPEED = 5.2f;
static const float FALLING_SPEED = 8.f;
static const float JUMPING_SPEED = -8.5f;
static const float SUPER_JUMPING_SPEED = -14.f;
static const float GRAVITY = 23.f;

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

hero::hero()
{
}

hero::~hero()
{
    _world->DestroyBody(_body);
    _world->DestroyBody(_bodyClone);
    _body = NULL;
    _bodyClone = NULL;
}

hero::hero(int type, Vector2i pos, b2World* world, input* input, effects* effectBox, projectile* projectileBox,
           sound* soundBox, scoreManager* scoreManager, int life, settings* settings)
{
    //Variable
    _onTheFloor = false;
    _lastOnTheFloor = false;
    _hasJumped = false;
    _hasAttacked = false;
    _isDown = false;
    _stunned = false;
    _dead = false;
    _specialAttack = false;
    _isCrushed = false;
    _doubleShuriken = false;
    _againstSpeedwalk = false;
    _canTeleport = false;
    _canSuperJump = false;
    _life = life;
    _lives = 3;
    _maxLife = life;
    _jumpTolerance = 0;
    _attacking = 0;
    _attackingAnimation = 0;
    _hooked = 0;
    _collisionType = -1;
    _stunned = 0;
    _invincibilityFrame = 0;
    _type = type;
    _specialAttackFrame = 0;
    _jumpAboveProjectileTimer = 0;
    _crushedTolerance = 0;
    _charging = 0;
    _spawningFrames = 0;
    _teleportingFrames = 0;
    _superJumpFrames = 0;
    _directionPressed = 0;
    _deltaTime = 1;
    _controlKey = HELP_NONE;

    //Vectors
    _initPos = pos;
    _speed = Vector2f(0.f, 0.f); //In Box2D unit
    _maxSpeed = Vector2f(WALKING_SPEED, FALLING_SPEED);
    _position = Vector2f(pos.x, pos.y); //In pixel unit
    _positionClone = Vector2f(pos.x, pos.y); //In pixel unit
    _hookedPoint = Vector2f(0,0);
    _stunDirection = Vector2f(0,0);
    _projectilePos = Vector2f(0,0);
    _teleportPos = Vector2f(0,0);

    //Objects
    _input = input;
    _effectBox = effectBox;
    _projectileBox = projectileBox;
    _soundBox = soundBox;
    _scoreManager = scoreManager;
    _settings = settings;
    _superJumpBuffer = NULL;

    //Box2D
    _world = world;
    _body = Box2DTools::CreateRectangleBox(_world, pos, 8, 12, 0.1, 0, 0, 3);
    _bodyClone = Box2DTools::CreateRectangleBox(_world, pos, 8, 12, 0.1, 0, 0, 3);
    _bodyClone->SetActive(false);

    //Sprites
    if (!_texture.loadFromFile(_files.purpleNinjaTile))
    {
        cout << "Unable to find ninja texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureLife.loadFromFile(_files.life))
    {
        cout << "Unable to find life texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureLives.loadFromFile(_files.lives))
    {
        cout << "Unable to find lives texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureBonusHud.loadFromFile(_files.bonusHud))
    {
        cout << "Unable to find bonus hud texture. Stopping execution." << endl;
        exit(1);
    }
    else if (!_textureControlArrows.loadFromFile(_files.controlArrows))
    {
        cout << "Unable to find control arrows texture. Stopping execution." << endl;
        exit(1);
    }
    else
    {
        //Hero sprite
        _sprite.setTexture(_texture);
        _sprite.setOrigin(14, 28);
        _spriteClone.setTexture(_texture);
        _spriteClone.setOrigin(14, 28);
        _animation = animation(&_sprite, 0.1, 28, 28, 6, 18, 0);

        //Life sprites
        for(int i=0 ; i<3 ; i++)
        {
            _lifeSprites[i].setTexture(_textureLife);
            _lifeSprites[i].setTextureRect(IntRect(0,0,7,7));
        }

        //Lives sprites
        for(int i=0 ; i<3 ; i++)
        {
            _livesSprites[i].setTexture(_textureLives);
            _livesSprites[i].setTextureRect(IntRect(0,0,10,7));
        }

        //Bonus hud
        _bonusHud.setTexture(_textureBonusHud);
        _bonusHud.setPosition(262,0);

        //Control_arrows
        _controlArrows.setTexture(_textureControlArrows);
        _controlArrows.setTextureRect(IntRect(0,0,7,7));
        _controlArrows.setOrigin(3.5,3.5);
    }

    //Black and white screen for special attack
    _blackWhiteScreen.setSize(Vector2f(NATIVE_WIDTH,NATIVE_HEIGHT));
    _bladeRectangle.setSize(Vector2f(NATIVE_WIDTH,4));
    _bladeRectangle.setFillColor(Color(255,255,255));
    _bladeRectangle.setOrigin(0,6);

    //Reverse sprite with initial position
    if(pos.x > NATIVE_WIDTH/2)
        _animation.ReverseSprite(true);

    //Texts
    _lifeText.SetString(xmlReader::getTranslationText("game", "English", "life"));
    _lifeText.SetPosition(-1, 3, ALIGNEMENT_LEFT);
}

void hero::UpdateVariables()
{
    //{ Respawn
    if(_life == 0 && _stunned == 0) //Waiting to respawn hero
    {
        if(_invincibilityFrame > 0)
            _invincibilityFrame --;
        else if(_spawningFrames == 30 && _lives >= 0)
        {
            _soundBox->PlaySound(SND_SPAWN, false);
            _effectBox->AddEffect(Vector2i(_initPos.x, _initPos.y-10), 0.2, 25, 25, 6, 5, false);
            _spawningFrames--;
        }
        else if(_spawningFrames > 0 && _lives >= 0)
            _spawningFrames --;
        else if(_lives >= 0)
        {
            _life = _maxLife;
            _invincibilityFrame = 120;
            _body->SetTransform(b2Vec2(_initPos.x/SCALE, _initPos.y/SCALE),0);
            _body->SetLinearVelocity(b2Vec2(0,0));
            _body->SetActive(true);
            _animation.ReverseSprite(_initPos.x > NATIVE_WIDTH/2);
            _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
            _speed = Vector2f(0,0);
            _sprite.setPosition(_position.x, _position.y+17);
        }
        else
            _dead = true;
        return;
    }
    //}

    //Body speed + position
    _speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
    _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
    _positionClone = Vector2f(_bodyClone->GetPosition().x*SCALE, _bodyClone->GetPosition().y*SCALE);

    //Testing if body is on the floor
    int colType;
    _lastOnTheFloor = _onTheFloor;
    _onTheFloor = false;
    _isCrushed = false;
    for(float i=_position.x-4.2 ; i<_position.x+4.2 ; i+=0.1)
    {
        b2Body* body;
        if(Box2DTools::TestCollisionPoint(_world, b2Vec2(i/SCALE, (_position.y+8.f)/SCALE), true, &_collisionType, &body))
            _onTheFloor = true;
        if(Box2DTools::TestCollisionPoint(_world, b2Vec2(i/SCALE, (_position.y-8.f)/SCALE), false, &colType, &body))
            _isCrushed = true;
    }
    if(!_onTheFloor)
        _collisionType = -1;

    //Killing hero if blocked between block and platform
    if(_stunned == 0 && (_collisionType == 1 || _collisionType == 2) && _isCrushed)
    {
        if(_crushedTolerance >= 20)
        {
            _life = _life > 0 ? 1 : 0;
            SetStunned(Vector2f(0,3.5), 5);
        }
        else
            _crushedTolerance ++;
    }
    else
        _crushedTolerance -= _crushedTolerance > 0 ? 1 : 0;

    //Jump tolerance - Permit the hero to jump during some frames after leaving the floor
    if(_onTheFloor)
        _jumpTolerance = 6/(_deltaTime != 0 ? _deltaTime :1);
    else if(_jumpTolerance > 0)
        _jumpTolerance --;

    //Speedwalk "STUCK IN THE AIR" bug correction
    if(Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x+5)/SCALE, (_position.y+5)/SCALE),
        true, &_collisionType) && _collisionType == 9)
        _body->SetLinearVelocity(b2Vec2(-3,_speed.y));
    else if(Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x-5)/SCALE, (_position.y+5)/SCALE),
        true, &_collisionType)  && _collisionType == 9)
        _body->SetLinearVelocity(b2Vec2(3,_speed.y));

    //Speedwalk animation bug correction
    if((Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x+5)/SCALE, (_position.y-5)/SCALE), true, &_collisionType) ||
        Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x-5)/SCALE, (_position.y-5)/SCALE), true, &_collisionType)) &&
        _collisionType == 9)
        _againstSpeedwalk = true;
    else
        _againstSpeedwalk = false;

    //Testing if body can hook to a wall
    if(!_onTheFloor && _speed.y >= -0.1)
    {
        b2Vec2 posLeft = b2Vec2(_body->GetPosition().x-(5/SCALE), _body->GetPosition().y-(6/SCALE));
        b2Vec2 posRight = b2Vec2(_body->GetPosition().x+(5/SCALE), _body->GetPosition().y-(6/SCALE));
        b2Vec2 posGround = b2Vec2(_body->GetPosition().x, _body->GetPosition().y+(9/SCALE));
        if(!Box2DTools::TestCollisionPoint(_world, posGround, true, &_collisionType))
        {
            //Getting hook coordinate if exist
            _hooked = Box2DTools::TestHookPoint(_world, posLeft, &_hookedPoint, _animation.GetReversed());
            if(_hooked == 0)
                _hooked = Box2DTools::TestHookPoint(_world, posRight, &_hookedPoint, _animation.GetReversed());

            //Testing if area above hook point is free
            b2Vec2 hook = b2Vec2(_hookedPoint.x/SCALE, _hookedPoint.y/SCALE);
            bool areaAboveFree = !Box2DTools::TestCollisionPoint(_world, hook, true, &_collisionType);

            //Performing hook
            if(_hooked != 0 && _speed.y >= 0)// && areaAboveFree)
            {
                _body->SetLinearVelocity(b2Vec2(0,0));
                _body->SetTransform(b2Vec2(_hookedPoint.x + ((_hooked > 0 ? 5 : -5)/SCALE), _hookedPoint.y+(6/SCALE)), 0);
                _speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
                _body->ApplyForce(b2Vec2(0.f,-GRAVITY*_body->GetMass()), _body->GetWorldCenter(), false);
            }
        }
    }
    else
    {
        _body->ApplyForce(b2Vec2(0.f,0*_body->GetMass()), _body->GetWorldCenter(), false);
        _hooked = 0;
    }

    //Attack duration
    if(_attacking != 0)
        _attacking += _attacking < 0 ? 1 : -1;

    //Jump above projectiles score limitation
    _jumpAboveProjectileTimer -= _jumpAboveProjectileTimer > 0 ? 1 : 0;

    //Direction if stunned
    if(_stunned > 0)
    {
        if(_hooked != 0 && fabs(_stunDirection.y) <= 2)
        {
            _stunDirection.x = 0;
            _stunDirection.y = 10;
            _body->ApplyForce(b2Vec2(0.f,42*_body->GetMass()), _body->GetWorldCenter(), false);
        }

        _hooked = 0;
        _body->SetLinearVelocity(b2Vec2(_stunDirection.x, _stunDirection.y));
    }

    //Stun duration
    _stunned -= _stunned > 0 ? 1 : 0;

    //Invincibility duration
    _invincibilityFrame -= _invincibilityFrame > 0 ? 1 : 0;

    //Teleporting animation
    if(_teleportingFrames == 1)
        _body->SetActive(true);
    _teleportingFrames -= _teleportingFrames > 0 ? 1 : 0;
    _superJumpFrames -= _superJumpFrames > 0 ? 1 : 0;

    //Limiting running speed
    if(_body->GetLinearVelocity().x > _maxSpeed.x)
        _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x-1.f*_deltaTime, _body->GetLinearVelocity().y));
    else if(_body->GetLinearVelocity().x < -_maxSpeed.x)
        _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x+1.f*_deltaTime, _body->GetLinearVelocity().y));

    //Limiting falling speed
    if(_body->GetLinearVelocity().y > _maxSpeed.y)
        _body->SetLinearVelocity(b2Vec2(_body->GetLinearVelocity().x, _maxSpeed.y));

    //Body speed + position (re-update)
    _speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
    _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);

    //Special attack frame
    _specialAttackFrame -= _specialAttackFrame > 0 ? 1 : 0;

    if(_life == 0 && _stunned == 1) //Killing hero if running out of life
    {
        _lives--;
        _position = _sprite.getPosition();
        _effectBox->AddEffect(Vector2i(_position.x, _position.y-8), 0.20, 25, 25, 6, 1, _animation.GetReversed());
        _soundBox->PlaySound(SND_EXPLOSION, false);
        _body->SetActive(false);
        _bodyClone->SetActive(false);
        _doubleShuriken = false;
        _stunned = 0;
        _spawningFrames = 30;
        _stunDirection = Vector2f(0,0);
        _scoreManager->DecreaseMultiplicator(2);
        _projectileBox->ChangeDommages(0);
        _effectBox->ShakeScreen(5);
    }
}

void hero::PerformLoop()
{
    //Clone of the sprite, if loop horizontal
    if(_position.x-15 <= 0)
    {
        _spriteClone.setPosition(NATIVE_WIDTH+_position.x+1, _position.y+8-(_isDown*3));
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2((NATIVE_WIDTH+_position.x)/SCALE, _position.y/SCALE),0);
    }
    else if(_position.x+15 >= NATIVE_WIDTH)
    {
        _spriteClone.setPosition(_position.x-NATIVE_WIDTH+1, _position.y+8-(_isDown*3));
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2((_position.x-NATIVE_WIDTH)/SCALE, _position.y/SCALE),0);
    }

    //Disabling collision of clone
    if(_position.x-15 > 0 && _position.x+15 < NATIVE_WIDTH)
    {
        _bodyClone->SetTransform(b2Vec2(0,0),0);
        _bodyClone->SetActive(false);
    }
}

void hero::PerformEnemyCollisions()
{
    /* To avoid contacts between enemies and hero just after a hit,
       Box2D contact detection is disable between those bodies.
       It is necessary to test collision "manually" */

    //Edge points of hero hit-box
    b2Vec2 posLeftTop = b2Vec2(_body->GetPosition().x-(4/SCALE), _body->GetPosition().y-(10/SCALE));
    b2Vec2 posLeftBottom = b2Vec2(_body->GetPosition().x-(4/SCALE), _body->GetPosition().y+(10/SCALE));
    b2Vec2 posRightTop = b2Vec2(_body->GetPosition().x+(4/SCALE), _body->GetPosition().y-(10/SCALE));
    b2Vec2 posRightBottom = b2Vec2(_body->GetPosition().x+(4/SCALE), _body->GetPosition().y+(10/SCALE));

    //Collision with enemies
    for(int i=0 ; i<_enemies->size() ; i++)
    {
        b2Body* enemyBodyA = (*_enemies)[i]->GetBodyA();
        b2Body* enemyBodyB = (*_enemies)[i]->GetBodyB();
        if((*_enemies)[i] == NULL || enemyBodyA == NULL || enemyBodyB == NULL || !enemyBodyA->IsActive())
            continue;

        if(enemyBodyA->GetFixtureList()[0].TestPoint(_body->GetPosition()) ||
           enemyBodyA->GetFixtureList()[0].TestPoint(posLeftTop) ||
           enemyBodyA->GetFixtureList()[0].TestPoint(posLeftBottom) ||
           enemyBodyA->GetFixtureList()[0].TestPoint(posRightTop) ||
           enemyBodyA->GetFixtureList()[0].TestPoint(posRightBottom))
        {
            int signe = _body->GetPosition().x > enemyBodyA->GetPosition().x > 0 ? 1 : -1;
            SetStunned(Vector2f(signe*5.f, -3.5f), 15);
            break;
        }
        else if(enemyBodyB->GetFixtureList()[0].TestPoint(_body->GetPosition()) ||
           enemyBodyB->GetFixtureList()[0].TestPoint(posLeftTop) ||
           enemyBodyB->GetFixtureList()[0].TestPoint(posLeftBottom) ||
           enemyBodyB->GetFixtureList()[0].TestPoint(posRightTop) ||
           enemyBodyB->GetFixtureList()[0].TestPoint(posRightBottom))
        {
            int signe = _body->GetPosition().x > enemyBodyA->GetPosition().x > 0 ? 1 : -1;
            SetStunned(Vector2f(signe*5.f, -3.5f), 15);
            break;
        }
    }

    //Contact with explosions
    Vector2f pt1 = Vector2f(posLeftTop.x*SCALE, posLeftTop.y*SCALE);
    Vector2f pt2 = Vector2f(posLeftBottom.x*SCALE, posLeftBottom.y*SCALE);
    Vector2f pt3 = Vector2f(posRightTop.x*SCALE, posRightTop.y*SCALE);
    Vector2f pt4 = Vector2f(posRightBottom.x*SCALE, posRightBottom.y*SCALE);
    Vector2f center = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
    int collisionWithAOE = _effectBox->InAOE(pt1, pt2, pt3, pt4, center);
    if(collisionWithAOE != 0)
        SetStunned(Vector2f(collisionWithAOE*5.f, -3.5f), 15);
}

void hero::Move()
{
    //Setting acceleration/deceleration
    float scalingSpeed = !_onTheFloor ? 0.5f : 0.8f;
    float scalingBrake = _attacking != 0 ? 0.8f : 0.5f;

    //Charging attack
    if(!_hasAttacked && _directionPressed < 4 && _input->isKeyPressed("ATTACK") &&
       (_input->isKeyPressed("LEFT") || _input->isKeyPressed("RIGHT") || _input->isKeyPressed("UP")))
       {
            if(_input->isKeyPressed("UP"))
                _charging = _superJumpBuffer == 0 ? _charging + 1 : 0;
            else
                _charging += 1;
            _body->SetLinearVelocity(b2Vec2(0.f, _speed.y));
       }
    else
        _charging = 0;

    if(_charging == 0 && (_input->isKeyPressed("LEFT") || _input->isKeyPressed("RIGHT") || _input->isKeyPressed("UP")))
        _directionPressed ++;
    else
        _directionPressed = 0;

    //{Attacking
    if(_input->isKeyPressed("ATTACK") && _attacking == 0 && _hooked == 0 && !_hasAttacked &&
       (_charging == 0 || _charging > 15))
    {
        int signe = _animation.GetReversed() ? -1 : 1;
        _attacking = signe*10;
        _attackingAnimation = _input->isKeyPressed("UP") ? 2 : 1;
        _hasAttacked = true;

        //Stop super jump sound
        _superJumpFrames = 0;
        if(_superJumpBuffer != NULL)
            _superJumpBuffer->stop();

        Vector2f position;
        if(_position.x-4 <= 0 && _animation.GetReversed())
            position = Vector2f(NATIVE_WIDTH+_position.x, _position.y);
        else if(_position.x+4 >= NATIVE_WIDTH && !_animation.GetReversed())
            position = Vector2f(_position.x-NATIVE_WIDTH, _position.y);
        else
            position = _position;

        //Projectile parameters
        float localSpeed = fabs(_speed.x) < 3.5 ? signe*4 : _speed.x + signe;
        int randY = rand()%3-1;
        Vector2i projectilePos;
        if(_input->isKeyPressed("UP"))
            projectilePos = Vector2i(position.x -signe*3, position.y-7);
        else
            projectilePos = Vector2i(position.x + localSpeed*1.3, position.y+randY+1);

        //Projectile speed
        int speed = _charging == 0 ? 7 : 10;
        Vector2f projectileSpeed = Vector2f(!_input->isKeyPressed("UP") ? speed*signe : 0,
                                             _input->isKeyPressed("UP") ? -speed : 0);

        //Creating projectile
        int projectileType = _charging == 0 ? -1 : 0;
        _projectileBox->AddProjectile(projectilePos, 0.3, 13, 13, 4, 0, 0, projectileSpeed, projectileType);
        if(_doubleShuriken && !_input->isKeyPressed("UP"))
            _projectileBox->AddProjectile(Vector2i(position.x - localSpeed, position.y+randY), 0.3, 13, 13, 4, 0, 0, -projectileSpeed, projectileType);
        _soundBox->PlaySound(_charging == 0 ? SND_SHOOT : SND_SHOOT3, false);
        _charging = 0;
        if(_speed.y >= 0)
        {
            _body->SetLinearVelocity(b2Vec2(_speed.x, 0));
            _speed.y = 0;
        }
    }
    if(!_input->isKeyPressed("ATTACK"))
        _hasAttacked = false;
    //}

    //Down - Teleporting !
    if(_canTeleport && !_isDown && _input->isKeyPressed("DOWN"))
        TeleportNinja();

    //Up - Super jump !
    if(_canSuperJump && _superJumpFrames == 0 && !_isDown && _input->isKeyPressed("UP"))
    {
        _hasJumped = true;
        _canSuperJump = false;
        _superJumpFrames = 70;
        _body->SetLinearVelocity(b2Vec2(_speed.x, SUPER_JUMPING_SPEED));
        _superJumpBuffer = _soundBox->PlaySound(SND_JUMP2, false);
        _effectBox->AddEffect(Vector2i(_teleportPos.x, _teleportPos.y-20), 0.34, 46, 27, 6, 1, false, SFX_EXPLOSION2);
        return;
    }

    //Down - Duck
    if(_onTheFloor && _hooked == 0 && _attacking == 0 && _charging == 0 && _superJumpFrames == 0 &&
       fabs(_speed.x) < 0.05 && _input->isKeyPressed("DOWN") && _teleportingFrames == 0)
    {
        if(!_isDown)
        {
            _world->DestroyBody(_body);
            _body = Box2DTools::CreateRectangleBox(_world, Vector2i(_position.x, _position.y+7), 13, 7, 0.1, 0, 0, 3);
        }
        _isDown = true;
    }
    else if(_teleportingFrames == 0 && _superJumpFrames == 0)
    {
        if(_isDown)
        {
            _world->DestroyBody(_body);
            _body = Box2DTools::CreateRectangleBox(_world, Vector2i(_position.x,_position.y-7), 8, 12, 0.1, 0, 0, 3);
            _body->SetTransform(b2Vec2(_position.x/SCALE, (_position.y+1)/SCALE), 0);
        }
        _isDown = false;
    }

    //Unhook
    if((_hooked < 0 && _input->isKeyPressed("LEFT")) ||
       (_hooked > 0 && _input->isKeyPressed("RIGHT")) ||
       (_hooked != 0 && _input->isKeyPressed("DOWN")))
    {
        _hooked = 0;
        _body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y+3.5/SCALE), 0);
        _body->ApplyForce(b2Vec2(0.f,4*_body->GetMass()), _body->GetWorldCenter(), false);
        if(!_input->isKeyPressed("LEFT") && !_input->isKeyPressed("RIGHT"))
            _body->SetLinearVelocity(b2Vec2((_hooked < 0 ? -2.f : 2.f), _speed.y));
    }

    //Moving
    if(_charging == 0)
    {
        if(_input->isKeyPressed("LEFT") || _input->isKeyPressed("RIGHT") && _hooked == 0)
            _body->ApplyForce(b2Vec2(0.f,0*_body->GetMass()), _body->GetWorldCenter(), false);

        if(_input->isKeyPressed("LEFT") && _speed.x - scalingSpeed > -_maxSpeed.x && _attacking == 0 && _hooked == 0)
            _body->SetLinearVelocity(b2Vec2(_speed.x - scalingSpeed, _speed.y));
        else if(_input->isKeyPressed("RIGHT") && _speed.x + scalingSpeed < _maxSpeed.x && _attacking == 0 && _hooked == 0)
            _body->SetLinearVelocity(b2Vec2(_speed.x + scalingSpeed, _speed.y));
        else if((!_input->isKeyPressed("LEFT") || _attacking != 0) && _speed.x <= -scalingBrake && _hooked == 0)
            _body->SetLinearVelocity(b2Vec2(_speed.x+scalingBrake, _speed.y));
        else if((!_input->isKeyPressed("RIGHT") || _attacking != 0) && _speed.x >= scalingBrake && _hooked == 0)
            _body->SetLinearVelocity(b2Vec2(_speed.x-scalingBrake, _speed.y));
        else if((!_input->isKeyPressed("LEFT") || !_input->isKeyPressed("RIGHT") || _attacking != 0) &&
                fabs(_speed.x) < scalingBrake && _hooked == 0)
            _body->SetLinearVelocity(b2Vec2(0, _speed.y));
    }

    //{Jumping
    if(!_hasJumped && !_isDown && _charging == 0 && _superJumpFrames == 0)
    {
        //Jumping - leaving platform
        if(_onTheFloor && /*(_collisionType == PLATFORM_ENTITY || _collisionType == PLATFORM2_ENTITY) &&*/
           _input->isKeyPressed("JUMP") && _input->isKeyPressed("DOWN"))
        {
            _onTheFloor = _lastOnTheFloor = false;
            _body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y+0.6), 0);
        }
        else if((_onTheFloor || _jumpTolerance > 0) && _input->isKeyPressed("JUMP"))
        {
            _body->ApplyForce(b2Vec2(0.f,0*_body->GetMass()), _body->GetWorldCenter(), false);
            _body->SetLinearVelocity(b2Vec2(_speed.x, JUMPING_SPEED));
            _soundBox->PlaySound(SND_JUMP, false);
        }
        else if(!_onTheFloor && !_input->isKeyPressed("JUMP") && _speed.y < -4)
            _body->SetLinearVelocity(b2Vec2(_speed.x, -0.5f));
        else if(_hooked != 0 && _input->isKeyPressed("JUMP"))
        {
            _body->ApplyForce(b2Vec2(0.f,0*_body->GetMass()), _body->GetWorldCenter(), false);
            _hooked = 0;
            _body->SetLinearVelocity(b2Vec2(_speed.x, JUMPING_SPEED));
            _soundBox->PlaySound(SND_JUMP, false);
        }
    }

    //Test if jumping above projectile
    if(!_onTheFloor && _stunned == 0 && _jumpAboveProjectileTimer == 0 &&
       _projectileBox->AboveAProjectile(_position, &_projectilePos))
        _jumpAboveProjectileTimer = 10; //Above projectile
    if(_jumpAboveProjectileTimer == 5)
    {
        //Waiting 15 frames to avoid giving points just before colliding projectile
        int points = 25 * _scoreManager->GetMultiplicatorLevel();
        stringstream stream;
        stream << points;
        _scoreManager->AddPoints(points);
        _effectBox->AddText(stream.str(), _projectilePos, Vector2f(0,-0.3), 60);
        _soundBox->PlaySound(SND_COIN, false);
    }
    //}

    //Special Attack !
    if(_input->isKeyPressed("SPECIAL") && _specialAttack && _specialAttackFrame == 0 &&
       _hooked == 0 && _attacking == 0)
    {
        _specialAttack = false;
        _specialAttackFrame = 60;
        _superJumpFrames = 0;
    }

    //Locking jumping key if pressed once
    if(_input->isKeyPressed("JUMP"))
        _hasJumped = true;
    else
        _hasJumped = false;

    _maxSpeed.y = FALLING_SPEED;
}

void hero::AnimeSprite()
{
    //Animation speed
    if(_attacking != 0 || _isCrushed)
        _animation.SetFrameDelay(0.45);
    else if(_animation.GetAnimation() == 0)
        _animation.SetFrameDelay(0.1);
    else
        _animation.SetFrameDelay(0.25);

    //Reverse sprite
    if(_attacking == 0 && _hooked == 0 && _specialAttackFrame == 0)
    {
        if(_input->isKeyPressed("LEFT"))
            _animation.ReverseSprite(true);
        else if(_input->isKeyPressed("RIGHT"))
            _animation.ReverseSprite(false);
    }

    //Changing opacity if character invincible
    if(_stunned == 0 && _invincibilityFrame > 0)
        _sprite.setColor(Color(255,255,255,(_invincibilityFrame%5 <= 2 ? 200 : 100)));
    else
        _sprite.setColor(Color(255,255,255));

    //{All animation
    if(_stunned != 0)
    {
        if(_stunDirection.x > 0.5)
            _animation.ReverseSprite(true);
        else if(_stunDirection.x < -0.5)
            _animation.ReverseSprite(false);
        _animation.SetAnimation(6,0);
    }
    else if(_hooked != 0)
    {
        _animation.SetAnimation(7, 4);
        _animation.ReverseSprite(_hooked > 0);
    }
    else if(_attacking != 0)
    {
        if(_attackingAnimation == 1)
        {
            _animation.SetAnimation(_doubleShuriken ? 14 : 5, 5);
            _animation.ReverseSprite(_attacking < 0 && !_doubleShuriken);
        }
        else
            _animation.SetAnimation(12, 5);
    }
    else if(_superJumpFrames > 0)
        _animation.SetAnimation(15, 0, false);
    else if(_charging > 0)
        _animation.SetAnimation(13, 0, false);
    else if(_specialAttackFrame > 35)
        _animation.SetAnimation(9, 5, false);
    else if(_specialAttackFrame > 30)
        _animation.SetAnimation(10, 5, false);
    else if(_isDown)
        _animation.SetAnimation(8, 5, false);
    else if(_isCrushed && !_onTheFloor)
        _animation.SetAnimation(11, 5, false);
    else if(!_onTheFloor && _speed.y < 0)
        _animation.SetAnimation(2, 2);
    else if(!_onTheFloor && _speed.y >= 0)
        _animation.SetAnimation(3, 3);
    else if(_onTheFloor && _input->isKeyPressed("LEFT") && _speed.x > 0.1)
        _animation.SetAnimation(_againstSpeedwalk ? 0 : 4, 0);
    else if(_onTheFloor && _input->isKeyPressed("RIGHT") && _speed.x < -0.1)
        _animation.SetAnimation(_againstSpeedwalk ? 0 : 4, 0);
    else if(fabs(_speed.x) > 0.5)
        _animation.SetAnimation(fabs(_speed.x) > WALKING_SPEED+1 ? 7 : _againstSpeedwalk ? 0 : 1, 0);
    else
        _animation.SetAnimation(0, 0);
    //}

    //Perform animation + position + drawing
    _animation.PerformAnimation(_deltaTime);
    _sprite.setPosition(_position.x, _position.y+8-(_isDown*3));

    //Special attack animation
    if(_animation.GetReversed())
        _bladeRectangle.setPosition(36*_specialAttackFrame-650, _position.y);
    else
        _bladeRectangle.setPosition(-NATIVE_WIDTH+36*(25-_specialAttackFrame), _position.y);

    if(_specialAttackFrame > 25 && _specialAttackFrame <= 30)
        _blackWhiteScreen.setFillColor(Color(255,255,255));
    else
        _blackWhiteScreen.setFillColor(Color(0,0,0));

    //Special attack sounds
    if(_specialAttackFrame == 60)
        _soundBox->PlaySound(SND_SPECIAL, false);
    else if(_specialAttackFrame == 25)
        _soundBox->PlaySound(SND_SPECIAL2, false);

    //Destroying enemies on last frame
    if(_specialAttackFrame == 1)
        _projectileBox->DestroyLine(_position.y);
}

b2Body* hero::GetBodyA()
{
    return _body;
}

b2Body* hero::GetBodyB()
{
    return _body != NULL && _bodyClone != NULL && _bodyClone->IsActive() ? _bodyClone : _body;
}

Vector2f hero::GetPosition()
{
    return Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
}

void hero::SetStunned(Vector2f stunDirection, int stunDuration)
{
    if(_stunned == 0 && _invincibilityFrame == 0 && _life > 0)
    {
        //Stop super jump sound
        if(_superJumpBuffer != NULL)
            _superJumpBuffer->stop();

        //Stunned
        _life -= _life > 0 ? 1 : 0;
        _stunned = stunDuration;
        _charging = 0;
        _stunDirection = stunDirection;
        _invincibilityFrame = _life > 0 ? stunDuration*10 : 60;
        _jumpAboveProjectileTimer = 0;
        _scoreManager->DecreaseMultiplicator(1);
        _soundBox->PlaySound(SND_HIT, false);
        _effectBox->ShakeScreen(6);
        _superJumpFrames = 0;

        if(_projectileBox->GetDommagesRatio() > 1)
        {
            _projectileBox->ChangeDommages(0.5);
            stringstream stream;
            stream << _projectileBox->GetDommagesRatio();
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->language, "powerup2") + stream.str(),
                                Vector2f(_position.x, _position.y-8), Vector2f(0,-0.3), 60);
        }
    }
}

int hero::GetType()
{
    return _type;
}

int hero::isAHero(b2Body* body)
{
    for(int i=0 ; i<_ninjas.size() ; i++)
    {
        if(_ninjas[i]->GetBodyA() == body || _ninjas[i]->GetBodyB() == body)
            return i;
    }
    return -1;
}

int hero::isAHero(b2Body* bodyA, b2Body* bodyB)
{
    return _body != NULL && _bodyClone != NULL &&
           (bodyA == _body || bodyA == _bodyClone || bodyB == _body || bodyB == _bodyClone);
}

bool hero::IsOnTheFloor()
{
    return _onTheFloor;
}

int hero::GetLife()
{
    return _life;
}

bool hero::AddLife(bool testOnly)
{
    if(_life < _maxLife)
    {
        if (!testOnly)
        {
            _life++;
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->language, "powerup1"),
                                Vector2f(_position.x, _position.y-8), Vector2f(0,-0.3), 60);
        }
        return true;
    }
    return false;
}

bool hero::AddLive(bool testOnly)
{
    if(_lives < 3)
    {
        if (!testOnly)
        {
            _lives++;
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->language, "powerup4"),
                                Vector2f(_position.x, _position.y-8), Vector2f(0,-0.3), 60);
        }
        return true;
    }
    return false;
}

bool hero::DoubleDommages(bool testOnly)
{
    if(_projectileBox->GetDommagesRatio() < 4)
    {
        if (!testOnly)
        {
            _projectileBox->ChangeDommages(1);
            stringstream stream;
            stream << _projectileBox->GetDommagesRatio();
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->language, "powerup2") + stream.str(),
                                Vector2f(_position.x, _position.y-8), Vector2f(0,-0.3), 60);
        }
        return true;
    }
    return false;
}

bool hero::DoubleShuriken(bool testOnly)
{
    if(!_doubleShuriken)
    {
        if (!testOnly)
        {
            _doubleShuriken = true;
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->language, "powerup5"),
                                Vector2f(_position.x, _position.y-8), Vector2f(0,-0.3), 60);
        }
        return true;
    }
    return false;
}

bool hero::AddSpecialAttack(bool testOnly)
{
    if(!_specialAttack)
    {
        if (!testOnly)
        {
            _specialAttack = true;
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->language, "powerup3"),
                                Vector2f(_position.x, _position.y-8), Vector2f(0,-0.3), 60);
        }
        return true;
    }
    return false;
}

bool hero::isUsingSpecialAttack()
{
    return _specialAttackFrame > 0;
}

bool hero::IsSpawningOrTeleporting()
{
    return _spawningFrames > 0 || _teleportingFrames > 0;
}

bool hero::IsDead()
{
    return _dead;
}

void hero::TeleportNinja()
{
    if(_teleportingFrames > 0)
        return;

    _soundBox->PlaySound(SND_TELEPORT, false);
    _effectBox->AddEffect(Vector2i(_teleportPos.x, _teleportPos.y), 0.34, 25, 50, 12, 0, _animation.GetReversed(), SFX_TELEPORT);
    _body->SetLinearVelocity(b2Vec2(0,0));
    _body->SetTransform(b2Vec2(_teleportPos.x/SCALE, (_teleportPos.y+18.f)/SCALE), 0);
    _body->SetActive(false);
    _teleportingFrames = 36;
    _invincibilityFrame = 60;
}

void hero::Step()
{
    UpdateVariables();
    if(_life == 0 && _stunned == 0) { return; } //Hero is dead
    if(_stunned == 0 && _life > 0 && _specialAttackFrame == 0) { Move(); }
    PerformLoop();
    if(_stunned == 0 && _life > 0) { PerformEnemyCollisions(); }
    AnimeSprite();
}

void hero::ResetHero(int numColor, int life, Vector2i initPos)
{
    //Variables
    _type = numColor;
    _attacking = 0;
    _stunned = 0;
    _invincibilityFrame = 0;
    _specialAttackFrame = 0;
    _spawningFrames = 30;
    _stunDirection = Vector2f(0,0);
    _initPos = initPos;
    _hasJumped = true;
    _dead = false;
    _specialAttack = false;
    _doubleShuriken = false;
    _life = 0;
    _lives = 3;
    _maxLife = life;

    //Box2D - objects
    _body->SetActive(true);
    _body->SetTransform(b2Vec2(_initPos.x/SCALE, _initPos.y/SCALE),0);
    _bodyClone->SetActive(false);
    _animation.ReverseSprite(_initPos.x > NATIVE_WIDTH/2);
}

void hero::ResetLifeText(string text)
{
    _lifeText.SetString(text);
}

void hero::DisableBody()
{
    _body->SetActive(false);
    _bodyClone->SetActive(false);
}

void hero::DrawLife(RenderWindow* window)
{
    //Life
    _lifeText.Display(window);
    for(int i=0 ; i<3 ; i++)
    {
        Color color = i+1 <= _life ? Color(255,255,255) : Color(0,0,0);
        IntRect rect = i+1 <= _life ? IntRect(0,0,7,7) : IntRect(7,0,7,7);
        _lifeSprites[i].setTextureRect(rect);
        _lifeSprites[i].setPosition(_lifeText.GetLocalBounds().width+4+i*8,1);
        window->draw(_lifeSprites[i]);
    }

    //Lives
    for(int i=0 ; i<3 ; i++)
    {
        _livesSprites[i].setPosition(_lifeText.GetLocalBounds().width+31 + i*12, 1);
        if(i+1 <= _lives)
            window->draw(_livesSprites[i]);
    }

    //Bonus hud
    if(!_specialAttack && _projectileBox->GetDommagesRatio() > 1)
        _bonusHud.setTextureRect(IntRect(0,0,9,8));
    else if(_specialAttack && _projectileBox->GetDommagesRatio() <= 1)
        _bonusHud.setTextureRect(IntRect(9,0,9,8));
    else
        _bonusHud.setTextureRect(IntRect(0,0,18,8));

    if(_specialAttack || _projectileBox->GetDommagesRatio() > 1)
        window->draw(_bonusHud);
}

void hero::Display(RenderWindow* window, float deltaTime)
{
    _deltaTime = deltaTime;
    if(_life == 0 && _stunned == 0) { return; } //Hero is dead

    //Hero sprite
    if(_teleportingFrames <= 2)
        window->draw(_sprite);
    if((_position.x-15 <= 0 || _position.x+15 >= NATIVE_WIDTH) && _teleportingFrames <= 2)
    {
        _spriteClone.setTextureRect(_sprite.getTextureRect());
        _spriteClone.setScale(_sprite.getScale());
        _spriteClone.setColor(_sprite.getColor());
        window->draw(_spriteClone);
    }

    //Special attack
    if(_specialAttackFrame > 10 && _specialAttackFrame <= 30)
        window->draw(_blackWhiteScreen);
    if(_specialAttackFrame > 10 && _specialAttackFrame <= 25)
        window->draw(_bladeRectangle);

    //Control keys
    _controlArrows.setPosition(_position.x, _position.y-20);
    _controlArrows.setTextureRect(IntRect((_controlKey == HELP_DOWN ? 0 : 7),0,7,7));
    if(_controlKey != HELP_NONE)
        window->draw(_controlArrows);
}
