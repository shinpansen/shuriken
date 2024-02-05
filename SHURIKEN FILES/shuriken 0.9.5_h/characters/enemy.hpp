#ifndef enemy_hpp
#define enemy_hpp

#include <math.h>
#include <string.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <Box2D/Box2D.h>
#include "../tools/input.hpp"
#include "../tools/Box2DTools.hpp"
#include "../tools/animation.hpp"
#include "../tools/files.hpp"
#include "../effects/effects.hpp"
#include "../effects/projectile.hpp"
#include "../effects/sound.hpp"
#include "../UI/scoreManager.hpp"
#include "../objects/powerups.hpp"

using namespace std;
using namespace sf;

enum ENEMY_TYPE
{
    ENEMY_BLUE_TANK,
    ENEMY_GREEN_TANK,
    ENEMY_RED_TANK,
    ENEMY_CLOWN,
    ENEMY_BOSS
};

class projectile;
class scoreManager;
class powerups;
class enemy
{
public:
    enemy();
    ~enemy();
    enemy(Vector2i hitbox, Vector2i tile, Vector2i origin, int maxLife, int maskType,
          ENEMY_TYPE type, Vector2i pos, b2World* world, effects* effectBox, projectile* projectileBox,
          sound* soundBox, scoreManager* scoreManager, powerups* powerups, Texture* textureEnemy,
          Texture* textureParachute);
    virtual void KillYourself();
    void Update();
    virtual void PerformEvents();
    virtual void AnimeSprite();
    b2Body* GetBodyA();
    b2Body* GetBodyB();
    enemy* GetAnEnemy(b2Body* body);
    bool IsAnEnemy(b2Body* body);
    virtual bool IsAnEnemy(b2Body* bodyA, b2Body* bodyB);
    bool IsActive();
    Vector2f GetPosition();
    void SetDommages(float value, float stunRecoil = 0);
    int getLife();
    bool IsDead();
    void Step();
    virtual void Display(RenderWindow* window, float deltaTime);
    vector<enemy*>* _enemies;

protected:
    //Variables
    ENEMY_TYPE _type;
    bool _dead;
    bool _isArriving;
    bool _onTheFloor;
    bool _lastOnTheFloor;
    bool _ignorePlatform;
    int _hitboxWidth;
    int _hitboxHeight;
    int _tileWidth;
    int _tileHeight;
    int _maskType;
    int _originX;
    int _originY;
    int _life;
    int _maxLife;
    int _attacking;
    int _attackingCooldown;
    int _turning;
    int _hit;
    int _jumpingTimer;
    int _collisionType;
    int _pointsReward;
    float _deltaTime;

    //Vectors
    Vector2i _initPos;
    Vector2f _speed;
    Vector2f _maxSpeed;
    Vector2f _position;
    Vector2f _positionClone;

    //Objects
    animation _animation;
    effects* _effectBox;
    projectile* _projectileBox;
    files _files;
    sound* _soundBox;
    scoreManager* _scoreManager;
    powerups* _powerups;

    //Box2D
    b2World* _world;
    b2Body* _body;
    b2Body* _bodyClone;

    //Sprite
    Sprite _sprite;
    Sprite _parachute;
    Sprite _spriteClone;
    Texture* _texture;
    Texture* _textureParachute;
};
#endif
