#pragma once

#include "Fmod/fmod.hpp"



struct SoundData
{
	FMOD::Channel* channel = nullptr;
	FMOD::Sound* sound = nullptr;
	bool _isPlaying = false;
};


class Sound
{
public:
	Sound() = default;
	~Sound();
	void Init();
	void Update();
	void SetVolume(const string& name, float volume);
	void SetMasterVolume(float volume);
	void Play(const string& name, float volume =1.0f, bool overlapped=false);
	void Stop(const string& name);
	void StopAll();
	void Pause(const string& name);


private:
	void LoadSound(const string& path , const string& name, bool loop=false);
	void ERRCHECK(FMOD_RESULT result);
private:
	FMOD::System* _system;

private:
	std::unordered_map<string, SoundData> _soundDatas;
	float _masterVolume = 1.0f;

public:
	static std::unique_ptr<Sound> main;
};

