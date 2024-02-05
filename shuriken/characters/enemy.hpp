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

class projectile;
class scoreManager;
class powerups;
class enemy
{
public:
    enemy();
    ~enemy();
    enemy(int type, Vector2i pos, b2World* world, effects* effectBox, projectile* projectileBox,
          sound* soundBox, scoreManager* scoreManager, powerups* powerups, Texture* textureEnemy,
          Texture* textureParachute);
    void KillYourself();
    void UpdateVariables();
    void PerformEffects();
    void AnimeSprite();
    b2Body* GetBodyA();
    b2Body* GetBodyB();
    bool IsAnEnemy(b2Body* body);
    bool IsAnEnemy(b2Body* bodyA, b2Body* bodyB);
    bool IsActive();
    bool IsAMovingTank();
    Vector2f GetPosition();
    void SetDommages(float value);
    int GetType();
    int getLife();
    void Step();
    void DisableBody();
    void Display(RenderWindow* window, float deltaTime);
    vector<enemy*>* _enemies;
private:
    //Variables
    bool _isArriving;
    bool _onTheFloor;
    bool _lastOnTheFloor;
    bool _ignorePlatform;

    bool _isdead;

    int _type;
    int _hitboxWidth;
    int _hitboxHeight;
    int _tileWidth;
    int _tileHeight;
    int _life;
    int _maxLife;
    int _attacking;
    int _attackingCooldown;
    int _turning;
    int _hit;
    int _jumpingTimer;
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
