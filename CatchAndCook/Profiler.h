

#pragma once

enum class BlockTag
{
	CPU,
	GPU,
};
class Profiler;
struct BlockInfo
{
	BlockTag tag = BlockTag::CPU;
	double _startTime = 0;
	double _endTime = 0;

	int depth = 0;

	double GetIntervalTime() const {
		return _endTime - _startTime;
	};

	double smoothIntervalTime = 0;
	double GetIntervalSmoothTime() const {
		return smoothIntervalTime;
	};
	void UpdateSmoothTime();
};

class Profiler
{
	unordered_map<string, BlockInfo> blockInfoTable;

	BlockInfo* lastInfo;
	vector<string> blockOrder;
	stack<string> blockStack;

	double currentTotal = 0;

public:
	static std::unique_ptr<Profiler> main;

	double _frameRate = 20;
	HWND hwnd;
	BlockInfo total;

	Profiler();
	void Init(HWND hParent, HINSTANCE hinstance);
	static void Set(const string& key,BlockTag tag = BlockTag::CPU);
	static void Fin();

	void Reset();

	Vector2 _winSize = Vector2(400,550);
    static LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static HWND WINAPI WinMain(HWND hParent, HINSTANCE hInst);
};

