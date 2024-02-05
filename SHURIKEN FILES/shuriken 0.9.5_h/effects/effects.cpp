#include "effects.hpp"

using namespace std;
using namespace sf;

static const float SCALE = 30.f;
static const double PI = 3.14159265359;
static const float NATIVE_WIDTH = 280;
static const float NATIVE_HEIGHT = 192;

effects::effects()
{
    //Variables
    _shakeFrame = 0;
    _shakePositions[0] = Vector2f(NATIVE_WIDTH/2,NATIVE_HEIGHT/2);

    //Textures
    if (!_explosionsTexture.loadFromFile(_files.explosionTile))
        cout << "Unable to find explosion texture '" << _files.explosionTile << "'" << endl;
    if (!_explosions2Texture.loadFromFile(_files.explosion2Tile))
        cout << "Unable to find explosion2 texture '" << _files.explosionTile << "'" << endl;
    if (!_teleportEffectTexture.loadFromFile(_files.teleportEffect))
        cout << "Unable to find effect texture '" << _files.teleportEffect << "'" << endl;

}

Sprite* effects::AddEffect(Vector2i pos, float frameDelay, int width, int height, int nbFrames,
                           int numAnimation, bool reversed, SFX textureId)
{
    try
    {
        _tiles.push_back(new Sprite());
        _animations.push_back(new animation());
        if(textureId == SFX_EXPLOSION)
            _tiles[_tiles.size()-1]->setTexture(_explosionsTexture);
        else if(textureId == SFX_EXPLOSION2)
            _tiles[_tiles.size()-1]->setTexture(_explosions2Texture);
        else if(textureId == SFX_TELEPORT)
            _tiles[_tiles.size()-1]->setTexture(_teleportEffectTexture);
        _tiles[_tiles.size()-1]->setOrigin(width/2, height/2);
        _tiles[_tiles.size()-1]->setPosition(pos.x, pos.y);
        *_animations[_animations.size()-1] = animation(_tiles[_tiles.size()-1], frameDelay, width, height, nbFrames, numAnimation+1, -1);
        _animations[_animations.size()-1]->SetAnimation(numAnimation, nbFrames);
        _animations[_animations.size()-1]->ReverseSprite(reversed);

        return _tiles[_tiles.size()-1];
    }
    catch(exception e)
    {
        cout << "An error occurred while adding effect : " << e.what() << endl;
    }
}

void effects::AddText(string text, Vector2f pos, Vector2f speed, int frames)
{
    try
    {
        //Adding text
        _textSpeed.push_back(speed);
        _textFrames.push_back(frames);
        _texts.push_back(new pixelFont());
        _texts[_texts.size()-1]->SetString(text);
        _texts[_texts.size()-1]->SetPosition(pos);
    }
    catch(exception e)
    {
        cout << "An error occurred while adding text : " << e.what() << endl;
    }
}

void effects::ShakeScreen(float force)
{
    _shakeFrame = 13;
    _shakePositions[0] = Vector2f(NATIVE_WIDTH/2.0,NATIVE_HEIGHT/2.0);
    _shakePositions[1] = Vector2f(NATIVE_WIDTH/2.0-force/2.0,NATIVE_HEIGHT/2.0);
    _shakePositions[2] = Vector2f(NATIVE_WIDTH/2.0-force,NATIVE_HEIGHT/2.0);
    _shakePositions[3] = Vector2f(NATIVE_WIDTH/2.0-force/2.0,NATIVE_HEIGHT/2.0);
    _shakePositions[4] = Vector2f(NATIVE_WIDTH/2.0,NATIVE_HEIGHT/2.0);
    _shakePositions[5] = Vector2f(NATIVE_WIDTH/2.0+force/2.0,NATIVE_HEIGHT/2.0);
    _shakePositions[6] = Vector2f(NATIVE_WIDTH/2.0+force,NATIVE_HEIGHT/2.0);
    _shakePositions[7] = Vector2f(NATIVE_WIDTH/2.0+force/2.0,NATIVE_HEIGHT/2.0+force/2.0);
    _shakePositions[8] = Vector2f(NATIVE_WIDTH/2.0,NATIVE_HEIGHT/2.0+force);
    _shakePositions[9] = Vector2f(NATIVE_WIDTH/2.0,NATIVE_HEIGHT/2.0+force/2.0);
    _shakePositions[10] = Vector2f(NATIVE_WIDTH/2.0-force/2.0,NATIVE_HEIGHT/2.0);
    _shakePositions[11] = Vector2f(NATIVE_WIDTH/2.0-force,NATIVE_HEIGHT/2.0-force/2.0);
    _shakePositions[12] = Vector2f(NATIVE_WIDTH/2.0-force/2.0,NATIVE_HEIGHT/2.0-force);
    _shakePositions[13] = Vector2f(NATIVE_WIDTH/2.0,NATIVE_HEIGHT/2.0-force/2.0);
}

void effects::RenderEffects(b2World* world, RenderWindow* window, View* view)
{
    try
    {
        //Removing finished effects
        bool elementDeleted = false;
        for(int i=0 ; i<_animations.size() ; i++)
        {
            if(elementDeleted)
            {
                elementDeleted = false;
                i--;
            }

            if(_animations[i]->isLastFrame())
            {
                delete _animations[i];
                delete _tiles[i];
                _animations.erase(_animations.begin()+i);
                _tiles.erase(_tiles.begin()+i);
                elementDeleted = true;
            }
            else
                _animations[i]->PerformAnimation(1);
        }

        //Moving/removing and drawing texts
        for(int i=0 ; i<_texts.size() ; i++)
        {
            int mod = _textSpeed[i].y != 0 ? (int)(1.f/_textSpeed[i].y) : 1;
            if((_textSpeed[i].y < 1 && _textFrames[i]%mod == 0) || _textSpeed[i].y >= 1)
                _texts[i]->Move(_textSpeed[i]);
            _textFrames[i] --;
            if(_textFrames[i] <= 0)
            {
                delete _texts[i];
                _texts.erase(_texts.begin()+i);
                _textSpeed.erase(_textSpeed.begin()+i);
                _textFrames.erase(_textFrames.begin()+i);
            }
            else
                _texts[i]->Display(window);
        }

        //Drawing effects
        for(int i=0 ; i<_tiles.size() ; i++)
            window->draw(*_tiles[i]);

        //Shake screen
        view->setCenter(_shakePositions[_shakeFrame]);
        _shakeFrame -= _shakeFrame > 0 ? 1 : 0;
    }
    catch(exception e)
    {
        cout << e.what() << endl;
    }
}

void effects::Display(RenderWindow* window)
{
    //Drawing effects
    for(int i=0 ; i<_tiles.size() ; i++)
        window->draw(*_tiles[i]);

    /*Drawing particles
    for(int i=0 ; i<_particles.size() ; i++)
        window->draw(*_particles[i]);

    //Drawing stars
    for(int i=0 ; i<_stars.size() ; i++)
        window->draw((*_stars[i]));*/
}
