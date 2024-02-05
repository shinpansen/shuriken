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
    _backShape.setSize(Vector2f(840,30));
    _backShape.setFillColor(Color(0,0,0));
    _multiplicatorGauge.setSize(Vector2f(10,15));
    _multiplicatorGauge.setPosition(630,7);

    //Score
    _font.loadFromFile(_files.font2);
    _scoreText.setFont(_font);
    _multiplicatorText.setFont(_font);
    _scoreText.setCharacterSize(16);
    _multiplicatorText.setCharacterSize(16);
    stringstream stream;
    stream << xmlReader::getTranslationText("game", _language, "score");
    stream << " " << _score;
    _scoreText.setString(stream.str());
    _scoreText.setPosition(690, 5);
    _multiplicatorText.setPosition(645, 5);

    //Enemies killed
    _killedText.setFont(_font);
    _killedText.setCharacterSize(16);
    stringstream stream2;
    stream2 << xmlReader::getTranslationText("game", _language, "killed");
    stream2 << " 0/" << _enemiesToKill;
    _killedText.setString(stream2.str());
    _killedText.setPosition(350, 5);

    //Enemies missed
    _missedText.setFont(_font);
    _missedText.setCharacterSize(16);
    stringstream stream3;
    stream3 << xmlReader::getTranslationText("game", _language, "missed");
    stream3 << " 0/" << _enemiesToMiss;
    _missedText.setString(stream3.str());
    _missedText.setPosition(490, 5);
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
    stream << xmlReader::getTranslationText("game", _language, "score") ;
    stream << " " << _score;
    _scoreText.setString(stream.str());
}

void scoreManager::AddEnemyKilled(int value)
{
    _enemiesKilled = _enemiesKilled+value < _enemiesToKill ? _enemiesKilled+value : _enemiesToKill;

    stringstream stream;
    stream << xmlReader::getTranslationText("game", _language, "killed");
    stream << " " << _enemiesKilled << "/" << _enemiesToKill;
    _killedText.setString(stream.str());
}

void scoreManager::AddEnemyMissed(int value)
{
    _enemiesMissed = _enemiesMissed+value < _enemiesToMiss ? _enemiesMissed+value : _enemiesToMiss;

    stringstream stream;
    stringstream stream2;
    stream << xmlReader::getTranslationText("game", _language, "missed");
    stream << " " << _enemiesMissed << "/" << _enemiesToMiss;
    _missedText.setString(stream.str());
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
    stream << "x " << multiplicatorNumber;
    return stream.str();
}

void scoreManager::Display(RenderWindow* window)
{
    //Updating multiplicator
    _multiplicator -= _multiplicator > 0 ? 0.05 : 0;
    string multiplicatorString = GetMultiplicatorLevelString();
    _multiplicatorText.setString(multiplicatorString);

    //Displaying
    window->draw(_backShape);
    window->draw(_scoreText);
    window->draw(_multiplicatorText);
    window->draw(_killedText);
    window->draw(_missedText);

    //Multiplicator gauge
    float baseMultiplicator = _multiplicator-((GetMultiplicatorLevel()-1)*30);
    _multiplicatorGauge.setScale(1, baseMultiplicator/30.f);
    window->draw(_multiplicatorGauge);
}
