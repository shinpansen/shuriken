#ifndef pixelFont_HPP
#define pixelFont_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

#include "files.hpp"

using namespace std;
using namespace sf;

enum ALIGNEMENT
{
    ALIGNEMENT_LEFT,
    ALIGNEMENT_RIGHT,
    ALIGNEMENT_CENTER
};


class pixelFont
{
public:
    pixelFont() : pixelFont("", Vector2f(0,0), ALIGNEMENT_CENTER) {};
    pixelFont(string text, Vector2f pos, ALIGNEMENT align = ALIGNEMENT_CENTER);
    void SetString(string text);
    string GetString();
    void SetPosition(int x, int y, ALIGNEMENT align = ALIGNEMENT_CENTER);
    void SetPosition(Vector2f pos, ALIGNEMENT align = ALIGNEMENT_CENTER);
    void Move(Vector2f pos, ALIGNEMENT align = ALIGNEMENT_CENTER);
    Vector2f GetPosition();
    void SetColor(Color color);
    Color GetColor();
    void SetCharacterSizeRatio(int sizeRatio);
    void Highlight(Color foreColor, Color backColor);
    void ActivateWaveMode(bool activate, int waveForce = 16);
    void ActivateColorFlashing(bool activate, Color oldColor = Color(255,255,255));
    FloatRect GetLocalBounds();
    FloatRect GetGlobalBounds();
    int Lenght();
    void Display(RenderWindow* window);
private:
    //Methods
    void LoadTextSprites();
    void PerformWaveEffect();
    Vector2i GetRectPos(int character);

    //Variables - Objects
    string _text;
    bool _highlighted;
    bool _waveMode;
    bool _colorFlashingMode;
    int _waveTransitionFrame;
    int _waveTransitions[96];
    int _waveForce;
    int _colorFrame;
    int _sizeRatio;
    Vector2f _position;
    files _files;
    ALIGNEMENT _align;
    Color _flashColors[8];

    //Graphics
    Texture _textureFont;
    vector<Sprite*> _letters;
    vector<Sprite*> _lettersHighLight;
};
#endif
