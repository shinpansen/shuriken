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
    if (!_font.loadFromFile(_files.font2))
        cout << "Unable to find font file" << endl;
}

Sprite* effects::AddEffect(Vector2i pos, float frameDelay, int width, int height, int nbFrames,
                           int numAnimation, bool reversed, int textureId)
{
    try
    {
        _tiles.push_back(new Sprite());
        _animations.push_back(new animation());
        _tiles[_tiles.size()-1]->setTexture(_explosionsTexture);
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

void effects::AddParticles(b2World* world, Color color, Vector2i pos, int minRand, int maxRand)
{
    try
    {
        srand(time(NULL));
        int random = rand()%(maxRand+minRand)+minRand;
        for(int i=0 ; i<random ; i++)
        {
            _particlesBodies.push_back(Box2DTools::CreateRectangleBox(world, pos, 10, 5, 0.5, 1, 0, 5));
            _particles.push_back(new RectangleShape());
            _particles[_particles.size()-1]->setSize(Vector2f(6,3));
            _particles[_particles.size()-1]->setFillColor(color);
            if(rand()%2 == 0)
                _particles[_particles.size()-1]->setScale(-1,1);
        }
    }
    catch(exception e)
    {
        cout << "An error occurred while adding particles : " << e.what() << endl;
    }
}

void effects::AddCircleLight(Color color, Vector2f pos, int number)
{
    try
    {
        for(int i=0 ; i<number ; i++)
        {
            int radius = 10;
            _stars.push_back(new CircleShape());
            _stars[_stars.size()-1]->setRadius(radius);
            _stars[_stars.size()-1]->setFillColor(Color(color.r, color.g, color.b, 50));
            _stars[_stars.size()-1]->setOrigin(radius, radius);
            _stars[_stars.size()-1]->setPosition(pos);
        }
    }
    catch(exception e)
    {
        cout << "An error occurred while adding star(s) : " << e.what() << endl;
    }
}

void effects::AddDust(Vector2f pos, bool toLeft, bool isStatic)
{
    try
    {
        //Random size and rotation speed
        int width = rand()%4+2;
        int rotationSpeed = rand()%15+5;
        int posX = rand()%6-3;
        int posY = rand()%6-3;
        float speedX = toLeft ? -fabs((float)posX*2.0) : fabs((float)posX*2.0);

        //Adding dust
        _dust.push_back(new RectangleShape());
        _dust[_dust.size()-1]->setFillColor(Color(235,235,200,230));
        _dust[_dust.size()-1]->setSize(Vector2f(width, width));
        _dust[_dust.size()-1]->setOrigin(Vector2f(width/2,width/2));
        _dust[_dust.size()-1]->setPosition(Vector2f(pos.x+posX, pos.y+posY));
        _dustRotation.push_back(toLeft ? rotationSpeed : -rotationSpeed);
        _dustSpeed.push_back(isStatic ? Vector2f(0,0) : Vector2f(speedX, -fabs((float)posY)));
    }
    catch(exception e)
    {
        cout << "An error occurred while adding dust : " << e.what() << endl;
    }
}

void effects::AddText(string text, Vector2f pos, Vector2f speed, int frames)
{
    try
    {
        //Adding text
        _textSpeed.push_back(speed);
        _textFrames.push_back(frames);
        _texts.push_back(new Text());
        _texts[_texts.size()-1]->setFont(_font);
        _texts[_texts.size()-1]->setCharacterSize(16);
        _texts[_texts.size()-1]->setString(text);
        _texts[_texts.size()-1]->setOrigin(_texts[_texts.size()-1]->getLocalBounds().width/2, 0);
        _texts[_texts.size()-1]->setPosition(pos);
    }
    catch(exception e)
    {
        cout << "An error occurred while adding text : " << e.what() << endl;
    }
}

void effects::ShakeScreen(int force)
{
    _shakeFrame = 7;
    _shakePositions[0] = Vector2f(NATIVE_WIDTH/2,NATIVE_HEIGHT/2);
    _shakePositions[1] = Vector2f(NATIVE_WIDTH/2-force,NATIVE_HEIGHT/2);
    _shakePositions[2] = Vector2f(NATIVE_WIDTH/2+force,NATIVE_HEIGHT/2);
    _shakePositions[3] = Vector2f(NATIVE_WIDTH/2,NATIVE_HEIGHT/2-force);
    _shakePositions[4] = Vector2f(NATIVE_WIDTH/2,NATIVE_HEIGHT/2+force);
    _shakePositions[5] = Vector2f(NATIVE_WIDTH/2-force,NATIVE_HEIGHT/2-force);
    _shakePositions[6] = Vector2f(NATIVE_WIDTH/2-force,NATIVE_HEIGHT/2+force);
    _shakePositions[7] = Vector2f(NATIVE_WIDTH/2+force,NATIVE_HEIGHT/2-force);
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
            _texts[i]->move(_textSpeed[i]);
            _textFrames[i] --;
            if(_textFrames[i] <= 0)
            {
                delete _texts[i];
                _texts.erase(_texts.begin()+i);
                _textSpeed.erase(_textSpeed.begin()+i);
                _textFrames.erase(_textFrames.begin()+i);
            }
            else
                window->draw(*_texts[i]);
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
