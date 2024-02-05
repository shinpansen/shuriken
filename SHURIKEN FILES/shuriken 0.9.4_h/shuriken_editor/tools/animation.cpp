#include "animation.hpp"

using namespace std;
using namespace sf;

animation::animation()
{
}

animation::animation(Sprite* tile, float frameDelay, int width, int height, int nbFrames, int nbAnimations,
                     int staticAnimation, bool disableScale)
{
    //Variables
    _tile = tile;
    _width = width;
    _height = height;
    _nbFrames = nbFrames;
    _nbAnimations = nbAnimations;
    _animation = -1;
    _loop = 0;
    _frame = 0;
    _frameDelay = frameDelay;
    _reversed = false;
    _staticFrame = 0;
    _staticAnimation = staticAnimation;
    _disableScale = disableScale;

    //Animation of falling - crush sprite
    _scaleFall = new float[10];
    _frameScaleFall = 9;
    for(int i=0 ; i<5 ; i++)
        _scaleFall[i] = 1 - ((i+1)*0.05);
    for(int i=5 ; i<10 ; i++)
        _scaleFall[i] = 0.75 + ((i-4)*0.05);

    //Animation of jumping
    _scaleJump = new float[10];
    _frameScaleJump = 9;
    for(int i=0 ; i<5 ; i++)
        _scaleJump[i] = 1 + ((i+1)*0.1);
    for(int i=5 ; i<10 ; i++)
        _scaleJump[i] = 1.5 - ((i-4)*0.1);

    //IntRect informations
    _borderTopLeft = Vector2i(0,0);
    _borderBottomRight = Vector2i(width, height);

    PerformAnimation();
}

void animation::ReverseSprite(bool reversed)
{
    _reversed = reversed;
}
void animation::SetLoop(int loop)
{
    _loop = loop;
}

void animation::SetAnimation(int numAnimation, int loop)
{
    if(numAnimation != _animation && numAnimation >= 0 && numAnimation < _nbAnimations)
    {
        _frame = 0;

        if(numAnimation != _staticAnimation)
            _staticFrame = 3;
        else if(numAnimation == _staticAnimation && _staticFrame > 0)
        {
            _staticFrame--;
            return;
        }
    }
    else
        return;

    _loop = (loop >= 0 && loop < _nbAnimations) ? loop : 0;
    _animation = (numAnimation >= 0 && numAnimation < _nbAnimations) ? numAnimation : 0;
}

void animation::SetFrameDelay(float delay)
{
    if(delay >= 0)
        _frameDelay = delay;
}

int animation::GetAnimation()
{
    return _animation;
}

bool animation::GetReversed()
{
    return _reversed;
}

bool animation::isLastFrame()
{
    if((int)(_frame + _frameDelay) >= _nbFrames)
        return true;
    else
        return false;
}

void animation::PerformFallingScale()
{
    _frameScaleFall = 0;
}

void animation::PerformJumpingScale()
{
    _frameScaleJump = 0;
}

void animation::PerformAnimation()
{
    try
    {
         //Reverse
        if(_reversed && !_disableScale)
            _tile->setScale(-1.f, _tile->getScale().y);
        else if (!_disableScale)
            _tile->setScale(1.f, _tile->getScale().y);

        //Changing sprite rect
        if(_borderTopLeft.x + _borderBottomRight.x <= _tile->getTexture()->getSize().x &&
           _borderTopLeft.y + _borderBottomRight.y <= _tile->getTexture()->getSize().y)
            _tile->setTextureRect(IntRect(_borderTopLeft, _borderBottomRight));
        else
        {
            cout << "Unable to perform animation." << endl;
            return;
        }

        //Increasing frame position
        _frame += _frameDelay;
        if((int)_frame >= _nbFrames)
            _frame = _loop;

        //Changing border top left
        _borderTopLeft = Vector2i((int)_frame*_width, _animation*_height);

        //Animation of falling / Jumping - crush sprite
        if(!_disableScale)
        {
            _tile->setScale(_tile->getScale().x,
                _frameScaleFall != 9 ? _scaleFall[_frameScaleFall] : _scaleJump[_frameScaleJump]);
            if(_frameScaleFall < 9)
                _frameScaleFall ++;
            if(_frameScaleJump < 9)
                _frameScaleJump ++;
        }
    }
    catch(exception)
    {
        cout << "Unable to perform animation." << endl;
    }
}
