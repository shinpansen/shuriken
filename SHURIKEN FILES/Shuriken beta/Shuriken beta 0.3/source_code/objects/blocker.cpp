#include "blocker.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

blocker::~blocker()
{
    _world->DestroyBody(_blueBody);
    _world->DestroyBody(_pinkBody);
    _blueBody = NULL;
    _pinkBody = NULL;
}

blocker::blocker(vector<hero*>* ninjas, effects* effectBox, sound* soundBox, b2World* world,
                 Vector2f pinkSwitchPos, Vector2f blueSwitchPos, Vector2f pinkBlockerPos, Vector2f blueBlockerPos,
                 Texture* textureSwitch, Texture* textureBlocker, Texture* textureWarning)
{
    _pinkPressed = false;
    _bluePressed = false;
    _warningTimer = 0;
    _switchDisabled = 0;
    _ninjas = ninjas;
    _world = world;
    _effectBox = effectBox;
    _soundBox = soundBox;

    //Box2D bodies
    _pinkBlockerPos = Vector2i(pinkBlockerPos.x, pinkBlockerPos.y);
    _blueBlockerPos = Vector2i(blueBlockerPos.x, blueBlockerPos.y);
    _pinkBody = Box2DTools::CreateRectangleBox(_world, _pinkBlockerPos, 36, 36, 0.1, 0, 0, 2);
    _blueBody = Box2DTools::CreateRectangleBox(_world, _blueBlockerPos, 36, 36, 0.1, 0, 0, 2);

    //Graphics
    _textureSwitch = textureSwitch;
    _textureBlocker = textureBlocker;
    _textureWarning = textureWarning;

    //Blockers
    _pinkBlocker.setTexture(*_textureBlocker);
    _blueBlocker.setTexture(*_textureBlocker);
    _pinkBlocker.setTextureRect(IntRect(0,0,36,36));
    _blueBlocker.setTextureRect(IntRect(36,0,36,36));
    _pinkBlocker.setOrigin(_pinkBlocker.getLocalBounds().width/2, _pinkBlocker.getLocalBounds().height/2);
    _blueBlocker.setOrigin(_blueBlocker.getLocalBounds().width/2, _blueBlocker.getLocalBounds().height/2);

    //Switches
    _pinkSwitch.setTexture(*_textureSwitch);
    _blueSwitch.setTexture(*_textureSwitch);
    _pinkSwitch.setTextureRect(IntRect(0,21,33,21));
    _blueSwitch.setTextureRect(IntRect(0,0,33,21));
    _pinkSwitch.setOrigin(_pinkSwitch.getLocalBounds().width/2, _pinkSwitch.getLocalBounds().height/2);
    _blueSwitch.setOrigin(_blueSwitch.getLocalBounds().width/2, _blueSwitch.getLocalBounds().height/2);
    _pinkSwitch.setPosition(pinkSwitchPos);
    _blueSwitch.setPosition(blueSwitchPos);

    //Warning bubbles
    _pinkWarning.setTexture(*_textureWarning);
    _blueWarning.setTexture(*_textureWarning);
    _pinkWarning.setTextureRect(IntRect(0,0,57,48));
    _blueWarning.setTextureRect(IntRect(58,0,57,48));
    _pinkWarning.setOrigin(_pinkWarning.getLocalBounds().width/2, _pinkWarning.getLocalBounds().height);
    _blueWarning.setOrigin(_blueWarning.getLocalBounds().width/2, _blueWarning.getLocalBounds().height);

    //Bubble warning transition
    _bubbleTransitionFrame = 0;
    _bubbleTransitions = new int[20];
    for(int i=0 ; i<10 ; i++)
        _bubbleTransitions[i] = i;
    for(int i=10 ; i<20 ; i++)
        _bubbleTransitions[i] = 20-i;
}

void blocker::Render()
{
    PerformContact();

    //Testing if enemy against blocker
    int collisionType;
    b2Vec2 pinkTestC = b2Vec2((_pinkBlocker.getPosition().x-35)/SCALE, (_pinkBlocker.getPosition().y)/SCALE);
    b2Vec2 blueTestC = b2Vec2((_blueBlocker.getPosition().x+35)/SCALE, (_blueBlocker.getPosition().y)/SCALE);
    if(_pinkPressed && Box2DTools::TestCollisionPoint(_world, pinkTestC, true, &collisionType) && collisionType == 4)
        _warningTimer++;
    else if(_bluePressed && Box2DTools::TestCollisionPoint(_world, blueTestC, true, &collisionType) && collisionType == 4)
        _warningTimer++;
    else
        _warningTimer = 0;

    //Disabling switches if warning timer above 8sec
    if(_warningTimer > 600)
    {
        _soundBox->PlaySound(SND_OUT_OF_ORDER, false);
        if(_pinkPressed)
            _effectBox->AddEffect(Vector2i(_pinkBlocker.getPosition().x, _pinkBlocker.getPosition().y), 0.3, 75, 75, 6, 7, false, 0);
        if(_bluePressed)
            _effectBox->AddEffect(Vector2i(_blueBlocker.getPosition().x, _blueBlocker.getPosition().y), 0.3, 75, 75, 6, 7, false, 0);
        _pinkPressed = false;
        _bluePressed = false;
        _warningTimer = 0;
        _switchDisabled = 600;
    }
    _switchDisabled -= _switchDisabled > 0 ? 1 : 0;

    //Blockers positions
    _pinkBody->SetTransform(b2Vec2(_pinkBlockerPos.x/SCALE, (_pinkBlockerPos.y-(36*_pinkPressed))/SCALE), 0);
    _blueBody->SetTransform(b2Vec2(_blueBlockerPos.x/SCALE, (_blueBlockerPos.y-(36*_bluePressed))/SCALE), 0);

    //Changing switches tiles
    _pinkSwitch.setTextureRect(IntRect(_switchDisabled > 0 ? 67 : _pinkPressed ? 34 : 0, 21, 33, 21));
    _blueSwitch.setTextureRect(IntRect(_switchDisabled > 0 ? 67 : _bluePressed ? 34 : 0, 0, 33, 21));

    //Blockers sprites positions
    Vector2f pinkPos = Vector2f(_pinkBody->GetPosition().x*SCALE, _pinkBody->GetPosition().y*SCALE);
    Vector2f bluePos = Vector2f(_blueBody->GetPosition().x*SCALE, _blueBody->GetPosition().y*SCALE);
    _pinkBlocker.setPosition(pinkPos);
    _blueBlocker.setPosition(bluePos);

    //Warning bubble
    int moving = _bubbleTransitions[_bubbleTransitionFrame];
    _bubbleTransitionFrame -= _bubbleTransitionFrame > 0 ? 1 : 0;
    _bubbleTransitionFrame = _bubbleTransitionFrame == 0 ? 19 : _bubbleTransitionFrame;
    _pinkWarning.setPosition(_pinkBlocker.getPosition().x, _pinkBlocker.getPosition().y-20-moving);
    _blueWarning.setPosition(_blueBlocker.getPosition().x, _blueBlocker.getPosition().y-20-moving);
}

void blocker::PerformContact()
{
    if(_switchDisabled > 0)
        return;

    //Perform contact with hero
    int collisionType;
    b2Vec2 pinkTestA = b2Vec2((_pinkBlocker.getPosition().x-16)/SCALE, (_pinkBlocker.getPosition().y-25)/SCALE);
    b2Vec2 pinkTestB = b2Vec2((_pinkBlocker.getPosition().x+16)/SCALE, (_pinkBlocker.getPosition().y-25)/SCALE);
    b2Vec2 blueTestA = b2Vec2((_blueBlocker.getPosition().x-16)/SCALE, (_blueBlocker.getPosition().y-25)/SCALE);
    b2Vec2 blueTestB = b2Vec2((_blueBlocker.getPosition().x+16)/SCALE, (_blueBlocker.getPosition().y-25)/SCALE);
    for(int i=0 ; i<_ninjas->size() ; i++)
    {
        b2Body* ninjaBody = (*_ninjas)[i]->GetBodyA();
        if(ninjaBody == NULL)
            continue;

        Vector2f ninjaPos = Vector2f(ninjaBody->GetPosition().x*SCALE, ninjaBody->GetPosition().y*SCALE);
        Vector2f pinkPos = _pinkSwitch.getPosition();
        Vector2f bluePos = _blueSwitch.getPosition();

        if(!Box2DTools::TestCollisionPoint(_world, pinkTestA, true, &collisionType) &&
           !Box2DTools::TestCollisionPoint(_world, pinkTestB, true, &collisionType) &&
           fabs(ninjaPos.x-pinkPos.x) < 25 && fabs(ninjaPos.y-pinkPos.y) < 25)
        {
            //TODO - PLAY SOUND
            if(!_pinkPressed)
            {
                _warningTimer = 0;
                _soundBox->PlaySound(SND_SWITCH, false);
                _effectBox->AddEffect(Vector2i(_pinkBlocker.getPosition().x, _pinkBlocker.getPosition().y-36), 0.3, 75, 75, 6, 6, false, 0);
                if(_bluePressed)
                    _effectBox->AddEffect(Vector2i(_blueBlocker.getPosition().x, _blueBlocker.getPosition().y), 0.3, 75, 75, 6, 7, false, 0);
            }
            _pinkPressed = true;
            _bluePressed = false;
        }
        else if(!Box2DTools::TestCollisionPoint(_world, blueTestA, true, &collisionType) &&
                !Box2DTools::TestCollisionPoint(_world, blueTestB, true, &collisionType) &&
                fabs(ninjaPos.x-bluePos.x) < 25 && fabs(ninjaPos.y-bluePos.y) < 25)
        {
            if(!_bluePressed)
            {
                _warningTimer = 0;
                _soundBox->PlaySound(SND_SWITCH, false);
                _effectBox->AddEffect(Vector2i(_blueBlocker.getPosition().x, _blueBlocker.getPosition().y-36), 0.3, 75, 75, 6, 6, false, 0);
                if(_pinkPressed)
                    _effectBox->AddEffect(Vector2i(_pinkBlocker.getPosition().x, _pinkBlocker.getPosition().y), 0.3, 75, 75, 6, 7, false, 0);
            }
            _pinkPressed = false;
            _bluePressed = true;
        }
    }
}

void blocker::Display(RenderWindow* window)
{
    if(_pinkBody != NULL && _blueBody != NULL)
        Render();

    //Switches
    window->draw(_pinkSwitch);
    window->draw(_blueSwitch);

    //Blockers
    if(_pinkPressed)
        window->draw(_pinkBlocker);
    if(_bluePressed)
        window->draw(_blueBlocker);

    //Warning bubbles
    if(_warningTimer > 360)
    {
        if(_pinkPressed)
            window->draw(_pinkWarning);
        if(_bluePressed)
            window->draw(_blueWarning);
    }
}
