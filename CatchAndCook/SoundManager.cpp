#include "pch.h"
#include "SoundManager.h"
#include <Fmod/fmod_errors.h>
std::unique_ptr<Sound> Sound::main = nullptr;



Sound::~Sound()
{
    for (auto& [key, data] : _soundDatas)
    {

        if (data.sound)
        {
            data.sound->release();
            data.sound = nullptr;
        }
    }

    _soundDatas.clear();

    if (_system)
    {
        _system->close();
        _system->release();
        _system = nullptr;
    }
}

void Sound::Init()
{
    auto result = FMOD::System_Create(&_system);

    ERRCHECK(result);

    result = _system->init(128, FMOD_INIT_NORMAL, nullptr);

    ERRCHECK(result);

    {
        std::string path = "../Resources/Sound/noneRoop/";

        for (const auto& entry : fs::directory_iterator(path))
        {
            std::string filename = entry.path().filename().string();        
            std::string key = fs::path(filename).stem().string();           

            LoadSound(path + filename, key, false);
        }
    }

    {
        std::string path = "../Resources/Sound/roop/";

        for (const auto& entry : fs::directory_iterator(path))
        {
            std::string filename = entry.path().filename().string();
            std::string key = fs::path(filename).stem().string();

            LoadSound(path + filename, key, true);
        }
    }

}

void Sound::Update()
{
    _system->update();

    for (auto& [name,soundData] : _soundDatas)
    {
        if (soundData.channel)
        {
            soundData.channel->isPlaying(&soundData._isPlaying);
        }
    }
}

void Sound::SetVolume(const string& name , float volume)
{
    auto it = _soundDatas.find(name);

    if (it == _soundDatas.end())
    {
        cerr << "Sound \"" << name << "\" not loaded!" << endl;
        return;
    }

    SoundData& data = it->second;

    if (data.channel)
    {
        data.channel->setVolume(_masterVolume * volume);
    }

}

void Sound::SetMasterVolume(float volume)
{
    _masterVolume = volume;
}

void Sound::Play(const string& name, float volume, bool overlapped)
{
    auto it = _soundDatas.find(name);

    if (it == _soundDatas.end())
    {
        cerr << "Sound \"" << name << "\" not loaded!" << endl;
        return;
    }

    SoundData& data = it->second;

    if (overlapped == false && data._isPlaying)
    {
        return;
    }

    FMOD_RESULT result = _system->playSound(data.sound, nullptr, false, &data.channel);
    ERRCHECK(result);
    
    if (data.channel)
        data.channel->setVolume(_masterVolume * volume);

}

void Sound::Stop(const string& name)
{
    auto it = _soundDatas.find(name);

    if (it != _soundDatas.end())
    {
        FMOD::Channel* channel = it->second.channel;

        if (channel)
        {
            channel->stop();
        }
    }
    _soundDatas[name]._isPlaying = false;
}

void Sound::StopAll()
{
   
    for (auto& ele : _soundDatas)
    {
        FMOD::Channel* channel = ele.second.channel;

        if (channel)
        {
            channel->stop();
        }
    }
    for (auto& [name, soundData] : _soundDatas)
    {
        if (soundData.channel)
        {
            soundData.channel->isPlaying(&soundData._isPlaying);
        }
    }
}

void Sound::Pause(const string& name)
{
    auto it = _soundDatas.find(name);
    if (it != _soundDatas.end())
    {
        FMOD::Channel* channel = it->second.channel;

        if (channel)
        {
            bool paused = false;
            channel->getPaused(&paused);
            channel->setPaused(!paused);

			paused ? _soundDatas[name]._isPlaying = false : _soundDatas[name]._isPlaying = true;
        }
    }
    
};


void Sound::LoadSound(const string& path, const string& name, bool loop)
{
  
    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result;

     if (loop)
     {
         result = _system->createSound(path.c_str(), FMOD_LOOP_NORMAL, 0, &sound);
     }
     else
     {
         result = _system->createSound(path.c_str(), FMOD_LOOP_OFF, 0, &sound);
     }

     ERRCHECK(result);

     _soundDatas.emplace(name, SoundData{ nullptr, sound });

}

void Sound::ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        cerr << "FMOD Error: " << FMOD_ErrorString(result) << endl;
        exit(-1);
    };
}
