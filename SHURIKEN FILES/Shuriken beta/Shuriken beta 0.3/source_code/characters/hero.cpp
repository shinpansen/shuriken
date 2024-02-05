#include "hero.hpp"

static const float SCALE = 30.f;
static const float WALKING_SPEED = 15.f;
static const float RUNNING_SPEED = 15.f;
static const float FALLING_SPEED = 20.f;

static const uint16 GROUND_ENTITY = 0x0001;
static const uint16 PLATFORM_ENTITY = 0x0002;
static const uint16 HERO_ENTITY = 0x0004;
static const uint16 ENEMY_ENTITY = 0x0008;
static const uint16 SHURIKEN_ENTITY = 0x0016;
static const uint16 PROJECTILE_ENTITY = 0x0032;

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
    _directionPressed = 0;
    _deltaTime = 1;

    //Vectors
    _initPos = pos;
    _speed = Vector2f(0.f, 0.f); //In Box2D unit
    _maxSpeed = Vector2f(RUNNING_SPEED, FALLING_SPEED);
    _position = Vector2f(pos.x, pos.y); //In pixel unit
    _positionClone = Vector2f(pos.x, pos.y); //In pixel unit
    _hookedPoint = Vector2f(0,0);
    _stunDirection = Vector2f(0,0);
    _projectilePos = Vector2f(0,0);

    //Objects
    _input = input;
    _effectBox = effectBox;
    _projectileBox = projectileBox;
    _soundBox = soundBox;
    _scoreManager = scoreManager;
    _settings = settings;

    //Box2D
    _world = world;
    _body = Box2DTools::CreateRectangleBox(_world, pos, 20, 35, 0.1, 0, 0, 3);
    _bodyClone = Box2DTools::CreateRectangleBox(_world, pos, 20, 35, 0.1, 0, 0, 3);
    _bodyClone->SetActive(false);

    //Sprites
    if (!_texture.loadFromFile(_files.purpleNinjaTile))
    {
        cout << "Unable to find ninja texture. Stopping execution." << endl;
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
    else
    {
        //Hero sprite
        _sprite.setTexture(_texture);
        _sprite.setOrigin(40, 84);
        _spriteClone.setTexture(_texture);
        _spriteClone.setOrigin(40, 84);
        _animation = animation(&_sprite, 0.1, 84, 84, 6, 18, 0);

        //Lives sprites
        for(int i=0 ; i<3 ; i++)
        {
            _livesSprites[i].setTexture(_textureLives);
            _livesSprites[i].setTextureRect(IntRect(0,0,31,20));
        }

        //Bonus hud
        _bonusHud.setTexture(_textureBonusHud);
        _bonusHud.setPosition(270,0);
    }

    //Black and white screen for special attack
    _blackWhiteScreen.setSize(Vector2f(840,576));
    _bladeRectangle.setSize(Vector2f(840,12));
    _bladeRectangle.setFillColor(Color(255,255,255));
    _bladeRectangle.setOrigin(0,6);

    //Reverse sprite with initial position
    if(pos.x > 450)
        _animation.ReverseSprite(true);

    //Texts
    _font.loadFromFile(_files.font2);
    _lifeText.setFont(_font);
    _lifeText.setCharacterSize(16);
    _lifeText.setString(xmlReader::getTranslationText("game", "English", "life"));
    _lifeText.setOrigin(_lifeText.getLocalBounds().width, 0);
    _lifeText.setPosition(_lifeText.getLocalBounds().width+5, 5);
}

void hero::UpdateVariables()
{
    //Respawn
    if(_life == 0 && _stunned == 0) //Waiting to respawn hero
    {
        if(_invincibilityFrame > 0)
            _invincibilityFrame --;
        else if(_spawningFrames == 30 && _lives >= 0)
        {
            _soundBox->PlaySound(SND_SPAWN, false);
            _effectBox->AddEffect(Vector2i(_initPos.x, _initPos.y-10), 0.2, 75, 75, 6, 5, false, 0);
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
            _animation.ReverseSprite(_initPos.x > 450);
            _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
            _speed = Vector2f(0,0);
            _sprite.setPosition(_position.x, _position.y+17);
        }
        else
            _dead = true;
        return;
    }

    //Body speed + position
    _speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
    _position = Vector2f(_body->GetPosition().x*SCALE, _body->GetPosition().y*SCALE);
    _positionClone = Vector2f(_bodyClone->GetPosition().x*SCALE, _bodyClone->GetPosition().y*SCALE);

    //Testing if body is on the floor
    int colType;
    _lastOnTheFloor = _onTheFloor;
    _onTheFloor = false;
    _isCrushed = false;
    for(double i=_position.x-10.3 ; i<_position.x+10.3 ; i+=0.1)
    {
        b2Body* body;
        if(Box2DTools::TestCollisionPoint(_world, b2Vec2(i/SCALE, (_position.y+18)/SCALE), true, &_collisionType, &body))
            _onTheFloor = true;
        if(Box2DTools::TestCollisionPoint(_world, b2Vec2(i/SCALE, (_position.y-18)/SCALE), false, &colType, &body))
            _isCrushed = true;
    }
    if(!_onTheFloor)
        _collisionType = -1;

    //Killing hero if blocked between block and platform
    if(_stunned == 0 && (_collisionType == 1 || _collisionType == 2) && _isCrushed)
    {
        if(_crushedTolerance >= 10)
        {
            _life = _life > 0 ? 1 : 0;
            SetStunned(Vector2f(0,10), 5);
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
    if(Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x+15)/SCALE, (_position.y+15)/SCALE),
        true, &_collisionType) && _collisionType == 9)
        _body->SetLinearVelocity(b2Vec2(-3,_speed.y));
    else if(Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x-15)/SCALE, (_position.y+15)/SCALE),
        true, &_collisionType)  && _collisionType == 9)
        _body->SetLinearVelocity(b2Vec2(3,_speed.y));

    //Speedwalk animation bug correction
    if((Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x+15)/SCALE, (_position.y-15)/SCALE), true, &_collisionType) ||
        Box2DTools::TestCollisionPoint(_world, b2Vec2((_position.x-15)/SCALE, (_position.y-15)/SCALE), true, &_collisionType)) &&
        _collisionType == 9)
        _againstSpeedwalk = true;
    else
        _againstSpeedwalk = false;

    //Testing if body can hook to a wall
    if(!_onTheFloor && _speed.y >= -0.1)
    {
        b2Vec2 posLeft = b2Vec2(_body->GetPosition().x-(11/SCALE), _body->GetPosition().y-(14/SCALE));
        b2Vec2 posRight = b2Vec2(_body->GetPosition().x+(11/SCALE), _body->GetPosition().y-(14/SCALE));
        b2Vec2 posGround = b2Vec2(_body->GetPosition().x, _body->GetPosition().y+(26/SCALE));
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
                _body->SetTransform(b2Vec2(_hookedPoint.x + ((_hooked > 0 ? 11 : -11)/SCALE), _hookedPoint.y+(14/SCALE)), 0);
                _speed = Vector2f(_body->GetLinearVelocity().x, _body->GetLinearVelocity().y);
                _body->ApplyForce(b2Vec2(0.f,-70.f*_body->GetMass()), _body->GetWorldCenter(), false);
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
    if(_stunned > 0)
        _stunned--;

    //Invincibility duration
    if(_invincibilityFrame > 0)
        _invincibilityFrame --;

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
        _effectBox->AddEffect(Vector2i(_position.x, _position.y-25), 0.20, 75, 75, 6, 1, _animation.GetReversed(), 1);
        _soundBox->PlaySound(SND_EXPLOSION, false);
        _body->SetActive(false);
        _bodyClone->SetActive(false);
        _doubleShuriken = false;
        _stunned = 0;
        _spawningFrames = 30;
        _stunDirection = Vector2f(0,0);
        _scoreManager->DecreaseMultiplicator(2);
        _projectileBox->ChangeDommages(0);
        _effectBox->ShakeScreen(20);
    }
}

void hero::PerformEffects()
{
    //Clone of the sprite, if loop horizontal
    if(_position.x-13 <= 0)
    {
        _spriteClone.setPosition(840+_position.x, _position.y+17);
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2((840+_position.x)/SCALE, _position.y/SCALE),0);
    }
    else if(_position.x+13 >= 840)
    {
        _spriteClone.setPosition(_position.x-840, _position.y+17);
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2((_position.x-840)/SCALE, _position.y/SCALE),0);
    }
    else if(_position.y-30 <= 0)
    {
        _spriteClone.setPosition(_position.x, _position.y+617);
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2(_position.x/SCALE, (_position.y+576)/SCALE),0);
    }
    else if(_position.y+30 >= 576)
    {
        _spriteClone.setPosition(_position.x, _position.y-583);
        _bodyClone->SetActive(true);
        _bodyClone->SetTransform(b2Vec2(_position.x/SCALE, (_position.y-576)/SCALE),0);
    }

    //Disabling collision of clone
    if(_position.x-13 > 0 && _position.x+13 < 840 && _position.y-30 > 0 && _position.y+30 < 576)
    {
        _bodyClone->SetTransform(b2Vec2(0,0),0);
        _bodyClone->SetActive(false);
    }
}

void hero::PerformEnemyCollisions()
{
    b2Vec2 posLeft = b2Vec2(_body->GetPosition().x-(13/SCALE), _body->GetPosition().y);
    b2Vec2 posRight = b2Vec2(_body->GetPosition().x+(13/SCALE), _body->GetPosition().y);
    b2Vec2 posTop = b2Vec2(_body->GetPosition().x, _body->GetPosition().y-(30/SCALE));
    b2Vec2 posBottom = b2Vec2(_body->GetPosition().x, _body->GetPosition().y+(30/SCALE));
    for(int i=0 ; i<_enemies->size() ; i++)
    {
        b2Body* enemyBodyA = (*_enemies)[i]->GetBodyA();
        b2Body* enemyBodyB = (*_enemies)[i]->GetBodyB();
        if((*_enemies)[i] == NULL || enemyBodyA == NULL || enemyBodyB == NULL || !enemyBodyA->IsActive())
            continue;

        if(enemyBodyA->GetFixtureList()[0].TestPoint(_body->GetPosition()) ||
           enemyBodyA->GetFixtureList()[0].TestPoint(posLeft) ||
           enemyBodyA->GetFixtureList()[0].TestPoint(posRight) ||
           enemyBodyA->GetFixtureList()[0].TestPoint(posTop) ||
           enemyBodyA->GetFixtureList()[0].TestPoint(posBottom))
        {
            int signe = _body->GetPosition().x > enemyBodyA->GetPosition().x > 0 ? 1 : -1;
            SetStunned(Vector2f(signe*15.f, -10.f), 15);
            break;
        }
        else if(enemyBodyB->GetFixtureList()[0].TestPoint(_body->GetPosition()) ||
           enemyBodyB->GetFixtureList()[0].TestPoint(posLeft) ||
           enemyBodyB->GetFixtureList()[0].TestPoint(posRight) ||
           enemyBodyB->GetFixtureList()[0].TestPoint(posTop) ||
           enemyBodyB->GetFixtureList()[0].TestPoint(posBottom))
        {
            int signe = _body->GetPosition().x > enemyBodyA->GetPosition().x > 0 ? 1 : -1;
            SetStunned(Vector2f(signe*15.f, -10.f), 15);
            break;
        }
    }
}

void hero::Move()
{
    //Setting acceleration/deceleration
    float deltaAdjust = fabs(_deltaTime - 1) < 0.1 ? 1 : _deltaTime;
    int scalingSpeed = !_onTheFloor ? 1.5f*deltaAdjust : 2*deltaAdjust;
    float scalingBrake = _attacking != 0 ? 1*deltaAdjust: 1.5f*deltaAdjust;

    //Charging attack
    if(!_hasAttacked && _directionPressed < 4 && _input->isKeyPressed("ATTACK") &&
       (_input->isKeyPressed("LEFT") || _input->isKeyPressed("RIGHT") || _input->isKeyPressed("UP")))
       {
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

        Vector2f position;
        if(_position.x-13 <= 0 && _animation.GetReversed())
            position = Vector2f(840+_position.x, _position.y);
        else if(_position.x+13 >= 840 && !_animation.GetReversed())
            position = Vector2f(_position.x-840, _position.y);
        else
            position = _position;

        //Projectile parameters
        int localSpeed = fabs(_speed.x) < 10 ? signe*12 : _speed.x + (2*signe);
        int randY = rand()%10-5;
        Vector2i projectilePos;
        if(_input->isKeyPressed("UP"))
            projectilePos = Vector2i(position.x -signe*10, position.y-20);
        else
            projectilePos = Vector2i(position.x + localSpeed, position.y-2-randY);

        //Projectile speed
        int speed = _charging == 0 ? 20 : 30;
        Vector2f projectileSpeed = Vector2f(!_input->isKeyPressed("UP") ? speed*signe : 0,
                                             _input->isKeyPressed("UP") ? -speed : 0);

        //Creating projectile
        int projectileType = _charging == 0 ? -1 : 0;
        _projectileBox->AddProjectile(projectilePos, 0.3, 39, 39, 2, 0, 0, projectileSpeed, projectileType);
        if(_doubleShuriken && !_input->isKeyPressed("UP"))
            _projectileBox->AddProjectile(Vector2i(position.x - localSpeed, position.y-2-randY), 0.3, 39, 39, 2, 0, 0, -projectileSpeed, projectileType);
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

    //Down - Changing hitbox
    if(_onTheFloor && _hooked == 0 && _attacking == 0 && _charging == 0 &&
       fabs(_speed.x) < 0.05 && _input->isKeyPressed("DOWN"))
    {
        if(!_isDown)
        {
            _world->DestroyBody(_body);
            _body = Box2DTools::CreateRectangleBox(_world, Vector2i(_position.x, _position.y+7), 40, 20, 0.1, 0, 0, 3);
        }
        _isDown = true;
    }
    else
    {
        if(_isDown)
        {
            _world->DestroyBody(_body);
            _body = Box2DTools::CreateRectangleBox(_world, Vector2i(_position.x,_position.y-7), 20, 35, 0.1, 0, 0, 3);
        }
        _isDown = false;
    }

    //Unhook
    if((_hooked < 0 && _input->isKeyPressed("LEFT")) ||
       (_hooked > 0 && _input->isKeyPressed("RIGHT")) ||
       (_hooked != 0 && _input->isKeyPressed("DOWN")))
    {
        _hooked = 0;
        _body->SetTransform(b2Vec2(_body->GetPosition().x, _body->GetPosition().y+10/SCALE), 0);
        _body->ApplyForce(b2Vec2(0.f,10*_body->GetMass()), _body->GetWorldCenter(), false);
        if(!_input->isKeyPressed("LEFT") && !_input->isKeyPressed("RIGHT"))
            _body->SetLinearVelocity(b2Vec2((_hooked < 0 ? -5.f : 5.f), _speed.y));
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
    if(!_hasJumped && !_isDown && _charging == 0)
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
            _body->SetLinearVelocity(b2Vec2(_speed.x, -25));
            _soundBox->PlaySound(SND_JUMP, false);
        }
        else if(!_onTheFloor && !_input->isKeyPressed("JUMP") && _speed.y < -7)
            _body->SetLinearVelocity(b2Vec2(_speed.x, -3));
        else if(_hooked != 0 && _input->isKeyPressed("JUMP"))
        {
            _body->ApplyForce(b2Vec2(0.f,0*_body->GetMass()), _body->GetWorldCenter(), false);
            _hooked = 0;
            _body->SetLinearVelocity(b2Vec2(_speed.x, -25));
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
        _effectBox->AddText(stream.str(), _projectilePos, Vector2f(0,-1), 60);
        _soundBox->PlaySound(SND_COIN, false);
    }
    //}

    //Special Attack !
    if(_input->isKeyPressed("SPECIAL") && _specialAttack && _specialAttackFrame == 0 &&
       _hooked == 0 && _attacking == 0)
    {
        _specialAttack = false;
        _specialAttackFrame = 60;
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
    if(_attacking != 0)
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
    _sprite.setPosition(_position.x, _position.y+17-(_isDown*9));

    //Special attack animation
    if(_animation.GetReversed())
        _bladeRectangle.setPosition(112*_specialAttackFrame-1960, _position.y);
    else
        _bladeRectangle.setPosition(-840+112*(25-_specialAttackFrame), _position.y);

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
        //Stunned
        _life -= _life > 0 ? 1 : 0;
        _stunned = stunDuration;
        _charging = 0;
        _stunDirection = stunDirection;
        _invincibilityFrame = _life > 0 ? stunDuration*10 : 60;
        _jumpAboveProjectileTimer = 0;
        _scoreManager->DecreaseMultiplicator(1);
        _soundBox->PlaySound(SND_HIT, false);
        _effectBox->ShakeScreen(15);

        if(_projectileBox->GetDommagesRatio() > 1)
        {
            _projectileBox->ChangeDommages(0.5);
            stringstream stream;
            stream << _projectileBox->GetDommagesRatio();
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->_language, "powerup2") + stream.str(),
                                Vector2f(_position.x, _position.y-25), Vector2f(0,-1), 60);
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
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->_language, "powerup1"),
                                Vector2f(_position.x, _position.y-25), Vector2f(0,-1), 60);
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
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->_language, "powerup4"),
                                Vector2f(_position.x, _position.y-25), Vector2f(0,-1), 60);
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
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->_language, "powerup2") + stream.str(),
                                Vector2f(_position.x, _position.y-25), Vector2f(0,-1), 60);
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
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->_language, "powerup5"),
                                Vector2f(_position.x, _position.y-25), Vector2f(0,-1), 60);
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
            _effectBox->AddText(xmlReader::getTranslationText("other", _settings->_language, "powerup3"),
                                Vector2f(_position.x, _position.y-25), Vector2f(0,-1), 60);
        }
        return true;
    }
    return false;
}

bool hero::isUsingSpecialAttack()
{
    return _specialAttackFrame > 0;
}

bool hero::IsSpawning()
{
    return _spawningFrames > 0;
}

bool hero::IsDead()
{
    return _dead;
}

void hero::Step()
{
    UpdateVariables();
    if(_life == 0 && _stunned == 0) { return; } //Hero is dead
    if(_stunned == 0 && _life > 0 && _specialAttackFrame == 0) { Move(); }
    PerformEffects();
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
    _animation.ReverseSprite(_initPos.x > 450);
}

void hero::ResetLifeText(string text)
{
    _lifeText.setString(text);
}

void hero::DisableBody()
{
    _body->SetActive(false);
    _bodyClone->SetActive(false);
}

void hero::DrawLife(RenderWindow* window)
{
    //Life
    window->draw(_lifeText);
    for(int i=0 ; i<_maxLife ; i++)
    {
        Color color = i+1 <= _life ? Color(255,255,255) : Color(0,0,0);
        RectangleShape shape(Vector2f(12,12));
        shape.setFillColor(color);
        shape.setOutlineColor(Color(255,255,255));
        shape.setOutlineThickness(2);
        shape.setPosition(_lifeText.getLocalBounds().width+15+i*25,8);
        window->draw(shape);
    }

    //Lives
    for(int i=0 ; i<3 ; i++)
    {
        _livesSprites[i].setPosition(_lifeText.getLocalBounds().width+100 + i*35, 4);
        if(i+1 <= _lives)
            window->draw(_livesSprites[i]);
    }

    //Bonus hud
    if(!_specialAttack && _projectileBox->GetDommagesRatio() > 1)
        _bonusHud.setTextureRect(IntRect(0,0,33,33));
    else if(_specialAttack && _projectileBox->GetDommagesRatio() <= 1)
        _bonusHud.setTextureRect(IntRect(33,0,33,33));
    else
        _bonusHud.setTextureRect(IntRect(0,0,66,33));

    if(_specialAttack || _projectileBox->GetDommagesRatio() > 1)
        window->draw(_bonusHud);
}

void hero::Display(RenderWindow* window, float deltaTime)
{
    _deltaTime = deltaTime;
    if(_life == 0 && _stunned == 0) { return; } //Hero is dead

    //Hero sprite
    window->draw(_sprite);
    if(_position.x-13 <= 0 || _position.x+13 >= 840)
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
}
