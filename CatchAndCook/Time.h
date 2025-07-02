#pragma once

class Time
{
public:
	static unique_ptr<Time> main;

	void Init();
	void Update();

	uint32 GetFps() const { return _fps; }
	double GetTime() const { return _time; }
	double GetDeltaTime() const { return std::min(_deltaTime, 0.333); }
	double GetClockNow() const;
	double GetDeltaTimeNow() const;


private:
	uint64	_frequency = 0;
	uint64	_prevCount = 0;
	double	_deltaTime = 0.f;

private:

	HWND	_hwnd;
	std::array<double, 10> _frameCount;
	std::array<double, 10> _frameTime;
	uint32	_fps = 0;

	double _time = 0;

};

