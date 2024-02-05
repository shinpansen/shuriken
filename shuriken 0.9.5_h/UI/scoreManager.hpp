#ifndef scoreManager_HPP
#define scoreManager_HPP

#include <iostream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "../characters/hero.hpp"
#include "../effects/sound.hpp"
#include "../tools/files.hpp"
#include "../tools/xmlReader.hpp"
#include "../tools/pixelFont.hpp"

using namespace std;
using namespace sf;

class hero;
class scoreManager
{
public:
    scoreManager();
    void Reset(int enemiesToKill, int enemiesToMiss, string language = "", bool resetGraphics = true, bool resetScore = true);
    void AddPoints(int points);
    void AddEnemyKilled(int value);
    void AddEnemyMissed(int value);
    int GetObjectivesState();
    int GetEnemiesKilled();
    void DecreaseMultiplicator(int value);
    int GetScore();
    float GetMultiplicator();
    int GetMultiplicatorLevel();
    string GetMultiplicatorLevelString();
    void Display(RenderWindow* window, bool fullDisplay);
private:
    //Objects - variables
    string _language;
    files _files;
    int _score;
    float _multiplicator;
    int _enemiesKilled;
    int _enemiesToKill;
    int _enemiesMissed;
    int _enemiesToMiss;

    //Graphics
    RectangleShape _backShape;
    RectangleShape _multiplicatorGauge;
    Texture _textureHud;
    Sprite _killedIcon;
    Sprite _missedIcon;
    Sprite _scoreIcon;

    //Texts
    pixelFont _scoreText;
    pixelFont _multiplicatorText;
    pixelFont _killedText;
    pixelFont _missedText;
};
#endif
