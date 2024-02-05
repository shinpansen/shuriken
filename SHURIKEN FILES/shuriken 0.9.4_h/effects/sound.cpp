#include "sound.hpp"

using namespace std;
using namespace sf;

sound::sound()
{
    //Volumes
    _volume = 10;

    //Sounds resources
    if(!_jumpBuffer.loadFromFile("resources//sounds//Jump.ogg") ||
       !_jump2Buffer.loadFromFile("resources//sounds//Jump2.ogg") ||
       !_hitBuffer.loadFromFile("resources//sounds//Hit.ogg") ||
       !_explosionBuffer.loadFromFile("resources//sounds//Explosion.ogg") ||
       !_explosion2Buffer.loadFromFile("resources//sounds//Explosion2.ogg") ||
       !_shootBuffer.loadFromFile("resources//sounds//Shoot.ogg") ||
       !_shoot2Buffer.loadFromFile("resources//sounds//Shoot2.ogg") ||
       !_shoot3Buffer.loadFromFile("resources//sounds//Shoot3.ogg") ||
       !_powerupBuffer.loadFromFile("resources//sounds//powerup.ogg") ||
       !_powerup2Buffer.loadFromFile("resources//sounds//powerup2.ogg") ||
       !_powerup3Buffer.loadFromFile("resources//sounds//powerup3.ogg") ||
       !_powerup4Buffer.loadFromFile("resources//sounds//powerup4.ogg") ||
       !_powerup5Buffer.loadFromFile("resources//sounds//powerup5.ogg") ||
       !_powerup6Buffer.loadFromFile("resources//sounds//powerup6.ogg") ||
       !_powerup7Buffer.loadFromFile("resources//sounds//powerup7.ogg") ||
       !_blipBuffer.loadFromFile("resources//sounds//Blip.ogg") ||
       !_blip2Buffer.loadFromFile("resources//sounds//Blip2.ogg") ||
       !_specialBuffer.loadFromFile("resources//sounds//special.ogg") ||
       !_special2Buffer.loadFromFile("resources//sounds//special2.ogg") ||
       !_loadBuffer.loadFromFile("resources//sounds//load.ogg") ||
       !_gameOverBuffer.loadFromFile("resources//sounds//gameOver.ogg") ||
       !_victoryBuffer.loadFromFile("resources//sounds//victory.ogg") ||
       !_coinBuffer.loadFromFile("resources//sounds//Coin.ogg") ||
       !_bounceBuffer.loadFromFile("resources//sounds//bounce.ogg") ||
       !_engineBuffer.loadFromFile("resources//sounds//engine.ogg") ||
       !_levelBuffer.loadFromFile("resources//sounds//level.ogg") ||
       !_level1Buffer.loadFromFile("resources//sounds//level1.ogg") ||
       !_level2Buffer.loadFromFile("resources//sounds//level2.ogg") ||
       !_level3Buffer.loadFromFile("resources//sounds//level3.ogg") ||
       !_fallBuffer.loadFromFile("resources//sounds//fall.ogg") ||
       !_spawnBuffer.loadFromFile("resources//sounds//spawn.ogg") ||
       !_switchBuffer.loadFromFile("resources//sounds//switch.ogg") ||
       !_outOfOrderBuffer.loadFromFile("resources//sounds//outOfOrder.ogg") ||
       !_teleportBuffer.loadFromFile("resources//sounds//Teleport.ogg") ||
       !_pushBuffer.loadFromFile("resources//sounds//push.ogg") ||
       !_releaseBuffer.loadFromFile("resources//sounds//release.ogg"))
       {
           cout << "Some sounds files are missing." << endl;
           exit(1);
       }
}

Sound* sound::PlaySound(SND type, bool loop, int volume)
{
    volume = volume != -1 ? volume*((float)_volume/10.f) : volume;
    _sounds.push_back(new Sound());
    switch(type)
    {
        case SND_JUMP:// 0:
            _sounds[_sounds.size()-1]->setBuffer(_jumpBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 40.f*((float)_volume/10.f));
            break;
        case SND_JUMP2:
            _sounds[_sounds.size()-1]->setBuffer(_jump2Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 30.f*((float)_volume/10.f));
            break;
        case SND_HIT:// 1:
            _sounds[_sounds.size()-1]->setBuffer(_hitBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 40.f*((float)_volume/10.f));
            break;
        case SND_EXPLOSION:// 2:
            _sounds[_sounds.size()-1]->setBuffer(_explosionBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 40.f*((float)_volume/10.f));
            break;
        case SND_EXPLOSION2:// 3:
            _sounds[_sounds.size()-1]->setBuffer(_explosion2Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 40.f*((float)_volume/10.f));
            break;
        case SND_SHOOT:// 4:
            _sounds[_sounds.size()-1]->setBuffer(_shootBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 40.f*((float)_volume/10.f));
            break;
        case SND_SHOOT2: //5:
            _sounds[_sounds.size()-1]->setBuffer(_shoot2Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 40.f*((float)_volume/10.f));
            break;
        case SND_SHOOT3:
            _sounds[_sounds.size()-1]->setBuffer(_shoot3Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 40.f*((float)_volume/10.f));
            break;
        case SND_POWERUP: //6:
            _sounds[_sounds.size()-1]->setBuffer(_powerupBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_POWERUP2: //7:
            _sounds[_sounds.size()-1]->setBuffer(_powerup2Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_POWERUP3:
            _sounds[_sounds.size()-1]->setBuffer(_powerup3Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_POWERUP4:
            _sounds[_sounds.size()-1]->setBuffer(_powerup4Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_POWERUP5:
            _sounds[_sounds.size()-1]->setBuffer(_powerup5Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_POWERUP6:
            _sounds[_sounds.size()-1]->setBuffer(_powerup6Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_POWERUP7:
            _sounds[_sounds.size()-1]->setBuffer(_powerup7Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_BLIP: //8:
            _sounds[_sounds.size()-1]->setBuffer(_blipBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_BLIP2: //9:
            _sounds[_sounds.size()-1]->setBuffer(_blip2Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_SPECIAL: //10:
            _sounds[_sounds.size()-1]->setBuffer(_specialBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_SPECIAL2: //11:
            _sounds[_sounds.size()-1]->setBuffer(_special2Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_LOAD: //12:
            _sounds[_sounds.size()-1]->setBuffer(_loadBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 40.f*((float)_volume/10.f));
            break;
        case SND_GAMEOVER: //13:
            _sounds[_sounds.size()-1]->setBuffer(_gameOverBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 50.f*((float)_volume/10.f));
            break;
        case SND_VICTORY: //14:
            _sounds[_sounds.size()-1]->setBuffer(_victoryBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 50.f*((float)_volume/10.f));
            break;
        case SND_COIN: //15:
            _sounds[_sounds.size()-1]->setBuffer(_coinBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 50.f*((float)_volume/10.f));
            break;
        case SND_BOUNCE: //16:
            _sounds[_sounds.size()-1]->setBuffer(_bounceBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 50.f*((float)_volume/10.f));
            break;
        case SND_ENGINE: //17:
            _sounds[_sounds.size()-1]->setBuffer(_engineBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 20.f*((float)_volume/10.f));
            break;
        case SND_LEVEL: //18:
            _sounds[_sounds.size()-1]->setBuffer(_levelBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 50.f*((float)_volume/10.f));
            break;
        case SND_LEVEL_01: //20
            _sounds[_sounds.size()-1]->setBuffer(_level1Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_LEVEL_02:
            _sounds[_sounds.size()-1]->setBuffer(_level2Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_LEVEL_03:
            _sounds[_sounds.size()-1]->setBuffer(_level3Buffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_FALL: //19:
            _sounds[_sounds.size()-1]->setBuffer(_fallBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 30.f*((float)_volume/10.f));
            break;
        case SND_SPAWN:
            _sounds[_sounds.size()-1]->setBuffer(_spawnBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 60.f*((float)_volume/10.f));
            break;
        case SND_SWITCH:
            _sounds[_sounds.size()-1]->setBuffer(_switchBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 50.f*((float)_volume/10.f));
            break;
        case SND_OUT_OF_ORDER:
            _sounds[_sounds.size()-1]->setBuffer(_outOfOrderBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_TELEPORT:
            _sounds[_sounds.size()-1]->setBuffer(_teleportBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 70.f*((float)_volume/10.f));
            break;
        case SND_PUSH:
            _sounds[_sounds.size()-1]->setBuffer(_pushBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 50.f*((float)_volume/10.f));
            break;
        case SND_RELEASE:
            _sounds[_sounds.size()-1]->setBuffer(_releaseBuffer);
            _sounds[_sounds.size()-1]->setVolume(volume != -1 ? volume : 30.f*((float)_volume/10.f));
            break;
    }
    //_sounds[_sounds.size()-1]->setVolume(0);
    _sounds[_sounds.size()-1]->setLoop(loop);
    _sounds[_sounds.size()-1]->play();

    return _sounds[_sounds.size()-1];
}

void sound::PerformSounds()
{
    for(int i=0 ; i<_sounds.size() ; i++)
    {
        if(_sounds[i] == NULL)
            continue;
        if(_sounds[i]->getStatus() == SoundSource::Stopped)
        {
            //delete _sounds[i];
            _sounds.erase(_sounds.begin()+i);
        }
        else if(_sounds[i]->getLoop() && _sounds[i]->getPlayingOffset() == _sounds[i]->getBuffer()->getDuration())
        {
            _sounds.push_back(new Sound());
            _sounds[_sounds.size()-1]->setBuffer(*_sounds[i]->getBuffer());
            _sounds[_sounds.size()-1]->setVolume(_sounds[i]->getVolume());
            _sounds[_sounds.size()-1]->setLoop(true);
            _sounds[_sounds.size()-1]->play();
            //delete _sounds[i];
            _sounds.erase(_sounds.begin()+i);
        }
    }
}

void sound::UpdateVolumes(int volume)
{
    _volume = volume >= 0 && volume <= 10 ? volume : _volume;
}
