#ifndef sound_HPP
#define sound_HPP

#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <SFML/Audio.hpp>
#include "../tools/settings.hpp"

using namespace std;
using namespace sf;

enum SND
{
    SND_JUMP,
    SND_HIT,
    SND_EXPLOSION,
    SND_EXPLOSION2,
    SND_SHOOT,
    SND_SHOOT2,
    SND_SHOOT3,
    SND_POWERUP,
    SND_POWERUP2,
    SND_POWERUP3,
    SND_POWERUP4,
    SND_POWERUP5,
    SND_POWERUP6,
    SND_POWERUP7,
    SND_BLIP,
    SND_BLIP2,
    SND_SPECIAL,
    SND_SPECIAL2,
    SND_LOAD,
    SND_GAMEOVER,
    SND_VICTORY,
    SND_COIN,
    SND_BOUNCE,
    SND_ENGINE,
    SND_LEVEL,
    SND_LEVEL_01,
    SND_LEVEL_02,
    SND_LEVEL_03,
    SND_FALL,
    SND_SPAWN,
    SND_SWITCH,
    SND_OUT_OF_ORDER
};

class sound
{
public:
    sound();
    Sound* PlaySound(SND type, bool loop, int volume = -1);
    void PerformSounds();
    void UpdateVolumes(settings* settings);
private:
    //Variables
    int _volume;

    //Music - Sounds
    SoundBuffer _jumpBuffer;
    SoundBuffer _hitBuffer;
    SoundBuffer _explosionBuffer;
    SoundBuffer _explosion2Buffer;
    SoundBuffer _shootBuffer;
    SoundBuffer _shoot2Buffer;
    SoundBuffer _shoot3Buffer;
    SoundBuffer _powerupBuffer;
    SoundBuffer _powerup2Buffer;
    SoundBuffer _powerup3Buffer;
    SoundBuffer _powerup4Buffer;
    SoundBuffer _powerup5Buffer;
    SoundBuffer _powerup6Buffer;
    SoundBuffer _powerup7Buffer;
    SoundBuffer _blipBuffer;
    SoundBuffer _blip2Buffer;
    SoundBuffer _specialBuffer;
    SoundBuffer _special2Buffer;
    SoundBuffer _loadBuffer;
    SoundBuffer _gameOverBuffer;
    SoundBuffer _victoryBuffer;
    SoundBuffer _coinBuffer;
    SoundBuffer _bounceBuffer;
    SoundBuffer _engineBuffer;
    SoundBuffer _levelBuffer;
    SoundBuffer _level1Buffer;
    SoundBuffer _level2Buffer;
    SoundBuffer _level3Buffer;
    SoundBuffer _fallBuffer;
    SoundBuffer _spawnBuffer;
    SoundBuffer _switchBuffer;
    SoundBuffer _outOfOrderBuffer;
    vector<Sound*> _sounds;
};
#endif
