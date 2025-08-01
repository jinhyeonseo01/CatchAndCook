#include "pch.h"
#include "Time.h"


unique_ptr<Time> Time::main=nullptr;

void Time::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount)); // CPU 클럭
	_deltaTime = 0.33f;

	int i = 0;
	for (auto& ele : _frameCount)
		ele = 0;
	for (auto& ele : _frameTime)
		ele = static_cast<double>(i++) / _frameTime.size();
}

void Time::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<double>(_frequency);
	_prevCount = currentCount;

	for (auto& ele : _frameCount)
		ele += 1;
	for (auto& ele : _frameTime)
		ele += _deltaTime;

	float delta = 1.0f;
	for (int i = 0; i < _frameTime.size(); i++)
	{
		if (_frameTime[i] > delta)
		{
			_fps = static_cast<uint32>(std::round(_frameCount[i] / _frameTime[i]));
			_frameCount[i] -= _frameCount[i] / _frameTime[i];
			_frameTime[i] -= delta;
		}
	}

    _time += _deltaTime;



}

double Time::GetClockNow() const
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));
	return static_cast<double>(currentCount) / static_cast<double>(_frequency);
}

double Time::GetDeltaTimeNow() const
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));
	return (currentCount - _prevCount) / static_cast<double>(_frequency);
}


