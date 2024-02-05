#include "pixelFont.hpp"

using namespace std;
using namespace sf;

pixelFont::pixelFont(string text, Vector2f pos, ALIGNEMENT align)
{
    //Variables
    _text = text;
    _highlighted = false;
    _waveMode = false;
    _colorFlashingMode = false;
    _waveTransitionFrame = 0;
    _waveForce = 16;
    _colorFrame = 0;
    _sizeRatio = 1;
    _position = pos;
    _align = align;

    //Texture
    if(!_textureFont.loadFromFile(_files.pixelfont))
    {
        cout << "unable to find font file." << endl;
        exit(1);
    }
    _textureFont.setSmooth(false);

    //Transition wave
    for(int i=0 ; i<56 ; i++)
        _waveTransitions[i] = (int)((float)i/8.f) - 3;
    for(int i=56 ; i<96 ; i++)
        _waveTransitions[i] = 2 - (int)((float)(i-56)/8.f);

    //Color flashing effect
    _flashColors[0] = Color(23,93,240);
    _flashColors[1] = Color(23,93,240);
    _flashColors[2] = Color(0,202,41);
    _flashColors[3] = Color(0,202,41);
    _flashColors[4] = Color(0,168,241);
    _flashColors[5] = Color(0,168,241);
    _flashColors[6] = Color(235,127,35);
    _flashColors[7] = Color(235,127,35);

    LoadTextSprites();
}

void pixelFont::SetPosition(int x, int y, ALIGNEMENT align)
{
    SetPosition(Vector2f(x,y), align);
}

void pixelFont::SetPosition(Vector2f pos, ALIGNEMENT align)
{
    _position = pos;
    _align = align;
    int width = 6*_sizeRatio*_text.length();
    int posY = _position.y - 4 - (_sizeRatio-1)*6;
    switch(align)
    {
        case ALIGNEMENT_LEFT:
            for(int i=0 ; i<_letters.size() ; i++)
                _letters[i]->setPosition(Vector2f(_position.x+3*_sizeRatio+6*_sizeRatio*i, posY));
            for(int i=0 ; i<_lettersHighLight.size() ; i++)
                _lettersHighLight[i]->setPosition(Vector2f(_position.x+3*_sizeRatio+6*_sizeRatio*i, posY-1));
            break;
        case ALIGNEMENT_RIGHT:
            for(int i=0 ; i<_letters.size() ; i++)
                _letters[i]->setPosition(Vector2f(_position.x-width-3*_sizeRatio+6*_sizeRatio*i, posY));
            for(int i=0 ; i<_lettersHighLight.size() ; i++)
                _lettersHighLight[i]->setPosition(Vector2f(_position.x-width-3*_sizeRatio+6*_sizeRatio*i, posY-1));
            break;
        case ALIGNEMENT_CENTER:
            for(int i=0 ; i<_letters.size() ; i++)
                _letters[i]->setPosition(Vector2f(_position.x-width/2+6*_sizeRatio*i ,posY));
            for(int i=0 ; i<_lettersHighLight.size() ; i++)
                _lettersHighLight[i]->setPosition(Vector2f(_position.x-width/2+6*_sizeRatio*i, posY-1));
            break;
    }

    //Adjusting y if ç or Ç
    for(int i=0 ; i<_text.length() ; i++)
        if ((int)_text[i] == -25 || (int)_text[i] == -57)
            _letters[i]->setPosition(Vector2f(_letters[i]->getPosition().x, posY+2));
}

void pixelFont::Move(Vector2f pos, ALIGNEMENT align)
{
    SetPosition(_position.x+pos.x, _position.y+pos.y, align);
}

void pixelFont::SetString(string text)
{
    _text = text;
    LoadTextSprites();
}

string pixelFont::GetString()
{
    return _text;
}

Vector2f pixelFont::GetPosition()
{
    return _position;
}

void pixelFont::SetColor(Color color)
{
    _highlighted = false;
    for(int i=0 ; i<_letters.size() ; i++)
        _letters[i]->setColor(color);
}

Color pixelFont::GetColor()
{
    return _letters.size() > 0 ? _letters[0]->getColor() : Color(255,255,255);
}

void pixelFont::SetCharacterSizeRatio(int sizeRatio)
{
    _sizeRatio = sizeRatio >= 1 && sizeRatio <= 10 ? sizeRatio : 1;
    for(int i=0 ; i<_letters.size() ; i++)
        _letters[i]->setScale(_sizeRatio, _sizeRatio);
    for(int i=0 ; i<_lettersHighLight.size() ; i++)
        _lettersHighLight[i]->setScale(_sizeRatio, _sizeRatio);

    SetPosition(_position, _align);
}

void pixelFont::Highlight(Color foreColor, Color backColor)
{
    _highlighted = true;
    for(int i=0 ; i<_letters.size() ; i++)
        _letters[i]->setColor(backColor);
    for(int i=0 ; i<_lettersHighLight.size() ; i++)
        _lettersHighLight[i]->setColor(foreColor);
}

void pixelFont::ActivateWaveMode(bool activate, int waveForce)
{
    _waveForce = waveForce >= 2 && waveForce < 56 ? waveForce : 16;
    _waveMode = activate;
    if(!activate)
        SetPosition(_position);
}

void pixelFont::ActivateColorFlashing(bool activate, Color oldColor)
{
    _colorFlashingMode = activate;
    if(!activate)
        SetColor(oldColor);
}

FloatRect pixelFont::GetLocalBounds()
{
    return FloatRect(0,0,6*_sizeRatio*_text.length(),8*_sizeRatio);
}

FloatRect pixelFont::GetGlobalBounds()
{
    int width = 6*_sizeRatio*_text.length();
    switch(_align)
    {
        case ALIGNEMENT_LEFT:
            return FloatRect(_position.x,_position.y-4,width,8*_sizeRatio);
        case ALIGNEMENT_RIGHT:
            return FloatRect(_position.x-width,_position.y-4,width,8*_sizeRatio);
        case ALIGNEMENT_CENTER:
            return FloatRect(_position.x-width/2,_position.y-4,width,8*_sizeRatio);
        default:
            return FloatRect(_position.x-width/2,_position.y-4,width,8*_sizeRatio);
    }
}

int pixelFont::Lenght()
{
    return _text.length();
}

void pixelFont::Display(RenderWindow* window)
{
    //Letters
    for(int i=0 ; i<_letters.size() ; i++)
        window->draw(*_letters[i]);

    //Highlighted letters
    if(_highlighted)
        for(int i=0 ; i<_lettersHighLight.size() ; i++)
            window->draw(*_lettersHighLight[i]);

    //Wave transition
    if(_waveMode)
        PerformWaveEffect();

    //Color flashing effect
    if(_colorFlashingMode)
    {
        _colorFrame = _colorFrame < 7 ? _colorFrame + 1 : 0;
        SetColor(_flashColors[_colorFrame]);
    }
}

void pixelFont::LoadTextSprites()
{
    //Save last color
    Color lastColor = _letters.size() > 0 ? _letters[0]->getColor() : Color(255,255,255);

    //Deleting all letters sprites
    while(_letters.size() > 0)
    {
        delete _letters[0];
        _letters.erase(_letters.begin());
    }

    //Deleting all letters highlight sprites
    while(_lettersHighLight.size() > 0)
    {
        delete _lettersHighLight[0];
        _lettersHighLight.erase(_lettersHighLight.begin());
    }

    //Adding letters
    int width = 6*_text.length();
    for(int i=0 ; i<_text.length() ; i++)
    {
        Vector2i letterRect = GetRectPos((int)_text[i]);
        _letters.push_back(new Sprite());
        _letters[i]->setTexture(_textureFont);
        _letters[i]->setTextureRect(IntRect(letterRect.x,letterRect.y,6,8));
        _letters[i]->setColor(lastColor);
        _lettersHighLight.push_back(new Sprite());
        _lettersHighLight[i]->setTexture(_textureFont);
        _lettersHighLight[i]->setTextureRect(IntRect(letterRect.x,letterRect.y,6,8));
    }
    //SetPosition(_position, _align);
    SetCharacterSizeRatio(_sizeRatio);
}

void pixelFont::PerformWaveEffect()
{
    //Position of first letter
    _waveTransitionFrame = _waveTransitionFrame < 95 ? _waveTransitionFrame + 1 : 0;
    _letters[0]->setPosition(_letters[0]->getPosition().x, _position.y-4+_waveTransitions[_waveTransitionFrame]);
    _lettersHighLight[0]->setPosition(_letters[0]->getPosition().x, _position.y-5+_waveTransitions[_waveTransitionFrame]);

    // + 16 frames between each letter
    for(int i=1 ; i<_letters.size() ; i++)
    {
        //Getting next frame
        int nextFrame = _waveTransitionFrame;
        int cpt = i*_waveForce;
        while(cpt > 0)
        {
            nextFrame++;
            cpt--;
            if(nextFrame == 96)
                nextFrame = 0;
        }

        //Changing position
        _letters[i]->setPosition(_letters[i]->getPosition().x, _position.y-4+_waveTransitions[nextFrame]);
        _lettersHighLight[i]->setPosition(_letters[i]->getPosition().x, _position.y-5+_waveTransitions[nextFrame]);
    }
}

Vector2i pixelFont::GetRectPos(int character)
{
    if (character >= 65 && character <= 90) // A to Z
        return Vector2i((character-65)*6, 0);
    else if (character >= 97 && character <= 122) // a to z
        return Vector2i((character-97)*6, 0);
    else if (character >= 48 && character <= 57) // 0 to 9
        return Vector2i((character-48)*6, 16);
    else if (character == -32 || character == -64) // À and à
        return Vector2i(0,8);
    else if (character == -23 || character == -55) // É and é
        return Vector2i(6,8);
    else if (character == -24 || character == -56) // È and è
        return Vector2i(12,8);
    else if (character == -15 || character == -47) // Ñ and ñ
        return Vector2i(18,8);
    else if (character == -13 || character == -45) // Ó and ó
        return Vector2i(24,8);
    else if (character == -14 || character == -46) // Ò and ò
        return Vector2i(30,8);
    else if (character == 58) // :
        return Vector2i(36,8);
    else if (character == 46) // . point
        return Vector2i(42,8);
    else if (character == 32) // space
        return Vector2i(54,8);
    else if (character == 95) // _ underscore
        return Vector2i(60,8);
    else if (character == 47) // / slash
        return Vector2i(66,8);
    else if (character == 59) // ... in place of ;
        return Vector2i(72,8);
    else if (character == 45) // -
        return Vector2i(78,8);
    else if (character == 62) // <
        return Vector2i(84,8);
    else if (character == 60) // >
        return Vector2i(90,8);
    else if (character == -25 || character == -57) // ç or Ç
        return Vector2i(96,8);
    else if (character == 43) // +
        return Vector2i(102,8);
    else
        return Vector2i(48,8);
}
