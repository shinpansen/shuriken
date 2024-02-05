#ifndef hero_hpp
#define hero_hpp

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
#include "../tools/xmlReader.hpp"
#include "../tools/settings.hpp"
#include "../effects/effects.hpp"
#include "../effects/projectile.hpp"
#include "../effects/sound.hpp"
#include "../UI/scoreManager.hpp"
#include "../objects/platform.hpp"
#include "enemy.hpp"

using namespace std;
using namespace sf;

class projectile;
class scoreManager;
class enemy;
class hero
{
public:
    hero();
    ~hero();
    hero(int type, Vector2i pos, b2World* world, input* input, effects* effectBox, projectile* projectileBox,
         sound* soundBox, scoreManager* scoreManager, int life, settings* settings);
    void LoadWizardTexture(int wizardNumber);
    void UpdateVariables();
    void Move();
    void PerformEffects();
    void PerformEnemyCollisions();
    void AnimeSprite();
    b2Body* GetBodyA();
    b2Body* GetBodyB();
    Vector2f GetPosition();
    void SetStunned(Vector2f stunDirection, int stunDuration);
    int GetType();
    int isAHero(b2Body* body);
    int isAHero(b2Body* bodyA, b2Body* bodyB);
    int GetLife();
    bool IsOnTheFloor();
    bool AddLife(bool testOnly = false);
    bool AddLive(bool testOnly = false);
    bool DoubleDommages(bool testOnly = false);
    bool DoubleShuriken(bool testOnly = false);
    bool AddSpecialAttack(bool testOnly = false);
    bool isUsingSpecialAttack();
    bool IsSpawning();
    bool IsDead();
    void Step();
    void ResetHero(int numColor, int life, Vector2i initPos);
    void ResetLifeText(string text);
    void DisableBody();
    void DrawLife(RenderWindow* window);
    void Display(RenderWindow* window, float deltaTime);
    vector<hero*> _ninjas;
    vector<enemy*>* _enemies;
    bool _hasJumped;
private:
    //Variables
    bool _onTheFloor;
    bool _lastOnTheFloor;
    bool _hasAttacked;
    bool _isDown;
    bool _dead;
    bool _specialAttack;
    bool _isCrushed;
    bool _doubleShuriken;
    bool _againstSpeedwalk;
    int _life;
    int _lives;
    int _jumpTolerance; //Permit the hero to jump during some frames after leaving the floor
    int _hasCounteractedCoolDown;
    int _attacking;
    int _attackingAnimation;
    int _hooked;
    int _stunned;
    int _invincibilityFrame;
    int _userData;
    int _type;
    int _collisionType;
    int _maxLife;
    int _specialAttackFrame;
    int _jumpAboveProjectileTimer;
    int _crushedTolerance;
    int _charging;
    int _spawningFrames;
    int _directionPressed = 0;
    float _deltaTime;

    //Vectors
    Vector2i _initPos;
    Vector2f _speed;
    Vector2f _maxSpeed;
    Vector2f _position;
    Vector2f _positionClone;
    Vector2f _hookedPoint;
    Vector2f _stunDirection;
    Vector2f _projectilePos;

    //Objects
    input* _input;
    animation _animation;
    effects* _effectBox;
    projectile* _projectileBox;
    files _files;
    sound* _soundBox;
    scoreManager* _scoreManager;
    settings* _settings;

    //Box2D
    b2World* _world;
    b2Body* _body;
    b2Body* _bodyClone;

    //Graphics
    Sprite _sprite;
    Sprite _spriteClone;
    Sprite _livesSprites[3];
    Sprite* _spellEffect;
    Sprite _bonusHud;
    vector<Sprite*> _lifeSprite;
    vector<Sprite*> _lifeSpriteClone;
    Texture _texture;
    Texture _textureLives;
    Texture _textureBonusHud;
    RectangleShape _blackWhiteScreen;
    RectangleShape _bladeRectangle;

    //Texts
    Font _font;
    Text _lifeText;
};
#endif
