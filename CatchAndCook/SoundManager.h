#pragma once

#include "Fmod/fmod.hpp"

enum class SoundState
{
	NONE,
	PLAYING,
	STOP,
	PAUSED,
};

struct SoundData
{
	string name;
	SoundState state = SoundState::NONE;
	FMOD::Channel* channel = nullptr;
	FMOD::Sound* sound = nullptr;
	bool _isPlaying = false;
};


class Sound
{
public:
	void Init();
	void Update();
	void Play(const string& name, float volume =1.0f, bool overlapped=false);
	void Stop(const string& name);
	void Pause(const string& name);


private:
	void LoadSound(const string& path , const string& name, bool loop=false);
	void ERRCHECK(FMOD_RESULT result);
private:
	FMOD::System* _system;

private:
	std::unordered_map<string, SoundData> _soundDatas;

public:
	static std::unique_ptr<Sound> main;
};

