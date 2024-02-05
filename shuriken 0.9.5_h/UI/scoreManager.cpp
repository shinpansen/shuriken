#include "scoreManager.hpp"

using namespace std;
using namespace sf;

scoreManager::scoreManager()
{
    _score = 0;
    _multiplicator = 0;
    _language = "";
    _enemiesKilled = 0;
    _enemiesToKill = 0;
    _enemiesMissed = 0;
    _enemiesToMiss = 0;
}

void scoreManager::Reset(int enemiesToKill, int enemiesToMiss, string language, bool resetGraphics, bool resetScore)
{
    //Variables
    if(resetScore)
        _score = 0;
    _multiplicator = 0;
    _language = language;
    _enemiesKilled = 0;
    _enemiesToKill = enemiesToKill;
    _enemiesMissed = 0;
    _enemiesToMiss = enemiesToMiss;

    AddPoints(0);
    if(!resetGraphics)
        return;

    //Graphics
    _backShape.setSize(Vector2f(280,9));
    _backShape.setFillColor(Color(0,0,0));
    _backShape.setOutlineColor(Color(146,146,146));
    _backShape.setOutlineThickness(1);
    _multiplicatorGauge.setSize(Vector2f(10,15));
    _multiplicatorGauge.setPosition(630,7);

    //Hud
    if(!_textureHud.loadFromFile(_files.gameHud))
    {
        cout << "Error. Unable to find HUD texture !" << endl;
        exit(1);
    }
    _killedIcon.setTexture(_textureHud);
    _missedIcon.setTexture(_textureHud);
    _scoreIcon.setTexture(_textureHud);
    _killedIcon.setTextureRect(IntRect(14,0,11,5));
    _missedIcon.setTextureRect(IntRect(0,0,14,5));
    _scoreIcon.setTextureRect(IntRect(25,0,5,5));
    _killedIcon.setPosition(99,2);
    _missedIcon.setPosition(143,2);
    _scoreIcon.setPosition(201,2);

    //Score
    stringstream stream;
    //stream << xmlReader::getTranslationText("game", _language, "score");
    stream << _score;
    _scoreText.SetString(stream.str());
    _scoreText.SetPosition(205, 3, ALIGNEMENT_LEFT);
    _multiplicatorText.SetPosition(185, 3, ALIGNEMENT_LEFT);

    //Enemies killed
    stringstream stream2;
    //stream2 << xmlReader::getTranslationText("game", _language, "killed");
    stream2 << "0/" << _enemiesToKill;
    _killedText.SetString(stream2.str());
    _killedText.SetPosition(111, 3, ALIGNEMENT_LEFT);

    //Enemies missed
    stringstream stream3;
    //stream3 << xmlReader::getTranslationText("game", _language, "missed");
    stream3 << "0/" << _enemiesToMiss;
    _missedText.SetString(stream3.str());
    _missedText.SetPosition(158, 3, ALIGNEMENT_LEFT);
}

void scoreManager::AddPoints(int points)
{
    //Increasing multiplicator
    int multiplicatorLevel = GetMultiplicatorLevel();
    _multiplicator += points > 0 ? 15 : 0;
    _multiplicator = _multiplicator > 269 ? 269 : _multiplicator;
    if(GetMultiplicatorLevel() > multiplicatorLevel)
        _multiplicator = 30*GetMultiplicatorLevel();

    //Adding points
    _score += points;
    stringstream stream;
    //stream << xmlReader::getTranslationText("game", _language, "score") ;
    stream << _score;
    _scoreText.SetString(stream.str());
}

void scoreManager::AddEnemyKilled(int value)
{
    _enemiesKilled = _enemiesKilled+value < _enemiesToKill ? _enemiesKilled+value : _enemiesToKill;

    stringstream stream;
    //stream << xmlReader::getTranslationText("game", _language, "killed");
    stream << _enemiesKilled << "/" << _enemiesToKill;
    _killedText.SetString(stream.str());
}

void scoreManager::AddEnemyMissed(int value)
{
    _enemiesMissed = _enemiesMissed+value < _enemiesToMiss ? _enemiesMissed+value : _enemiesToMiss;

    stringstream stream;
    stringstream stream2;
    //stream << xmlReader::getTranslationText("game", _language, "missed");
    stream << _enemiesMissed << "/" << _enemiesToMiss;
    _missedText.SetString(stream.str());
}

int scoreManager::GetObjectivesState()
{
    /* TRICKY
     * 0 : game must continue
     * 1 : All enemies are dead
     * 2 : Too many enemies escaped
     */
    if(_enemiesKilled >= _enemiesToKill)
        return 1;
    else if(_enemiesMissed >= _enemiesToMiss)
        return 2;
    else
        return 0;
}

int scoreManager::GetEnemiesKilled()
{
    return _enemiesKilled;
}

void scoreManager::DecreaseMultiplicator(int value)
{
    _multiplicator -= _multiplicator >= value*30 ? value*30 : _multiplicator;
}

int scoreManager::GetScore()
{
    return _score;
}

float scoreManager::GetMultiplicator()
{
    return _multiplicator;
}

int scoreManager::GetMultiplicatorLevel()
{
    return (int)(_multiplicator/30) + 1;
}

string scoreManager::GetMultiplicatorLevelString()
{
    int multiplicatorNumber = (int)(_multiplicator/30) + 1;
    stringstream stream;
    stream << "x" << multiplicatorNumber;
    return stream.str();
}

void scoreManager::Display(RenderWindow* window, bool fullDisplay)
{
    //Updating multiplicator
    _multiplicator -= _multiplicator > 0 ? 0.05 : 0;
    string multiplicatorString = GetMultiplicatorLevelString();
    _multiplicatorText.SetString(multiplicatorString);

    //Displaying
    window->draw(_backShape);
    window->draw(_scoreIcon);
    _scoreText.Display(window);
    if(fullDisplay)
    {
        window->draw(_killedIcon);
        window->draw(_missedIcon);
        _multiplicatorText.Display(window);
        _killedText.Display(window);
        _missedText.Display(window);
    }

    //Multiplicator gauge
    float baseMultiplicator = _multiplicator-((GetMultiplicatorLevel()-1)*30);
    _multiplicatorGauge.setScale(1, baseMultiplicator/30.f);
    window->draw(_multiplicatorGauge);
}
