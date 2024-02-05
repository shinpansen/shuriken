#ifndef sound_HPP
#define sound_HPP

#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

class sound
{
public:
    sound();
    Sound* PlaySound(int type, bool loop, int volume = NULL);
    void PlayRandomMusic();
    void PerformSounds();
private:
    Music _music;
    SoundBuffer _jumpBuffer;
    SoundBuffer _fallingBuffer;
    SoundBuffer _spell1Buffer;
    SoundBuffer _spell2Buffer;
    SoundBuffer _spell3Buffer;
    SoundBuffer _spell4Buffer;
    SoundBuffer _spell5Buffer;
    SoundBuffer _magicExplosionBuffer;
    SoundBuffer _rollingBuffer;
    SoundBuffer _counteringBuffer;
    SoundBuffer _wallSlidingBuffer;
    SoundBuffer _dyingBuffer;
    vector<Sound*> _sounds;
};
#endif
