#include "pch.h"
#include "Profiler.h"


void BlockInfo::UpdateSmoothTime()
{
	double delta = (1 / Profiler::main->_frameRate); //Time::main->GetDeltaTime() * 20
	smoothIntervalTime = std::lerp(smoothIntervalTime, GetIntervalTime(), std::min(delta, 1.0));
}

Profiler::Profiler()
{
}
std::unique_ptr<Profiler> Profiler::main = nullptr;

void Profiler::Init(HWND hParent, HINSTANCE hinstance)
{
	hwnd = WinMain(hParent, hinstance);
}


void Profiler::Set(const string& key, BlockTag tag)
{
	if (main == nullptr)
		return;

	auto [it, tf] = main->blockInfoTable.try_emplace({key,{}});
	auto& info = it->second;
	info.tag = tag;
	info._startTime = Time::main->GetClockNow();
	info.depth = main->blockStack.size();

	main->blockOrder.push_back(key);
	main->blockStack.push(key);
}

void Profiler::Fin()
{
	if (main == nullptr)
		return;

	auto& info = main->blockInfoTable[main->blockStack.top()];
	info._endTime = Time::main->GetClockNow();

	main->blockStack.pop();
}

void Profiler::Reset()
{
	if (main == nullptr)
		return;

	total._endTime = Time::main->GetClockNow();
	
	if (currentTotal >= (1 / _frameRate))
	{

		InvalidateRect(hwnd, nullptr, TRUE);
		UpdateWindow(hwnd);
		currentTotal -= (1 / _frameRate);

		for (auto& data : blockInfoTable)
			data.second.UpdateSmoothTime();
		total.UpdateSmoothTime();
	}
	currentTotal += Time::main->GetDeltaTimeNow();
	blockOrder.clear();

	total._startTime = Time::main->GetClockNow();
}

LRESULT Profiler::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (main == nullptr)
		return {};


	if(msg == WM_PAINT) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd,&ps);


		vec2 padding = vec2(10,10);

		TextOutA(hdc, padding.x, padding.y,"CPU / GPU Performance Profiler", strlen("CPU / GPU Performance Profiler"));

		double totalDelta = std::round(main->total.GetIntervalSmoothTime() * 100000) / 100;
		double totalDeltaFPS = std::round(1 / main->total.GetIntervalSmoothTime());
		string text = std::format("{0:.2f} ms  /  {1:.0f} fps           \0", totalDelta, totalDeltaFPS);
		TextOutA(hdc, padding.x, padding.y + 20, text.c_str(), text.size());

		vec2 progressOffset = vec2(padding.x,max(10, 50));
		vec2 progressSize = vec2(((main->_winSize.x - padding.x) - progressOffset.x), 15);


		float total = 0;
		for(auto& data : main->blockOrder)
		{
			auto& info = main->blockInfoTable[data];
			total += info.GetIntervalSmoothTime();
		}
		RECT rect = {
			0,(int)progressOffset.y,
			(int)main->_winSize.x,(int)(progressOffset.y +  progressSize.y)
		};
		FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

		int currentX = progressOffset.x;
		int gaugeWidth = static_cast<int>(progressSize.x);
		size_t blockCount = main->blockOrder.size();

		// blockOrder의 모든 구간을 순회합니다.
		for (size_t i = 0; i < blockCount; i++)
		{
			auto& data = main->blockOrder[i];
			auto& info = main->blockInfoTable[data];
			int barWidth = 0;

			// 마지막 구간이라면 남은 픽셀을 모두 할당해 전체 게이지 길이가 정확히 gaugeWidth가 되도록 함
			if (i == blockCount - 1)
			{
				barWidth = gaugeWidth - (currentX - progressOffset.x);
			}
			else
			{
				// 각 구간의 길이는 비율에 맞게 계산 (반올림 오차가 있을 수 있음)
				barWidth = static_cast<int>(info.GetIntervalSmoothTime() / total * gaugeWidth);
			}

			// 사각형 영역 계산
			RECT rect = {
				currentX,
				static_cast<int>(progressOffset.y),
				currentX + barWidth,
				static_cast<int>(progressOffset.y + progressSize.y)
			};

			// 블록 태그에 따라 색상을 지정
			if (info.tag == BlockTag::CPU)
				SetDCBrushColor(hdc, RGB(150, 190, 230 - 5 * info.depth));
			else if (info.tag == BlockTag::GPU)
				SetDCBrushColor(hdc, RGB(180, 240, 150));

			// 사각형 영역을 채워서 게이지를 그립니다.
			FillRect(hdc, &rect, (HBRUSH)GetStockObject(DC_BRUSH));

			currentX += barWidth;
		}


		int dataCount = 10;
		auto totalData = main->total.GetIntervalSmoothTime() * 20;
		float dataOffsetY = progressOffset.y + 20;
		for (int i = 0; i < dataCount; i++)
		{
			rect = { static_cast<LONG>(progressOffset.x), static_cast<LONG>(dataOffsetY + 6 * i),
					static_cast<LONG>(progressOffset.x + progressSize.x), static_cast<LONG>(dataOffsetY + 6 * i + 5) };

			SetDCBrushColor(hdc, RGB(230, 150, 150));
			FillRect(hdc, &rect, (HBRUSH)GetStockObject(DC_BRUSH));

			double segmentValue = totalData * dataCount - i;
			segmentValue = std::clamp(segmentValue, 0.0, 1.0);

			// rect는 각 세그먼트의 게이지를 그리기 위한 사각형입니다.
			rect = {
				static_cast<LONG>(progressOffset.x),
				static_cast<LONG>(dataOffsetY + 6 * i),
				static_cast<LONG>(progressOffset.x + progressSize.x * segmentValue),
				static_cast<LONG>(dataOffsetY + 6 * i + 5)
			};
			SetDCBrushColor(hdc, RGB(150, 230, 150));
			FillRect(hdc, &rect, (HBRUSH)GetStockObject(DC_BRUSH));
		}


		int index = 0;
		for(auto& data : main->blockOrder)
		{
			auto& info = main->blockInfoTable[data];
			double delta = std::round(info.GetIntervalSmoothTime() * 100000) / 100;
			double deltaFPS = 1.0 / info.GetIntervalSmoothTime();
			string text = std::format("{0} : {1:.2f}ms          \0",data, delta);
			TextOutA(hdc,padding.x + 20 * info.depth,150 + (index * 20),text .c_str(),text.size());
			++index;
		}

		EndPaint(hWnd,&ps);
	}
	else if (msg == WM_DESTROY) {
		PostQuitMessage(0);
	}
	else if (msg == WM_SIZE){
		RECT	rect;
		GetClientRect(hWnd, &rect);
		main->_winSize = vec2(rect.right - rect.left, rect.bottom - rect.top);
	} else {
		return DefWindowProcA(hWnd,msg,wParam,lParam);
	}
	return 0;
}

HWND Profiler::WinMain(HWND hParent,HINSTANCE hInst)
{
	WNDCLASSA wc = {0};
	wc.lpfnWndProc = Profiler::WndProc; // static 함수여야 함
	wc.hInstance = hInst;
	wc.lpszClassName = "MySubWndClass";
	RegisterClassA(&wc);

	int desiredClientWidth  = static_cast<int>(main->_winSize.x);
	int desiredClientHeight = static_cast<int>(main->_winSize.y);

	// 클라이언트 영역 크기를 담을 RECT 구조체 초기화
	RECT rc = {0,0,desiredClientWidth,desiredClientHeight};

	// 지정한 창 스타일에 맞게 전체 창 크기를 조정합니다.
	// WS_OVERLAPPEDWINDOW 스타일을 사용하고, 메뉴가 없으므로 FALSE를 전달합니다.
	AdjustWindowRect(&rc,WS_OVERLAPPEDWINDOW,FALSE);

	// 조정된 전체 창 크기는 다음과 같습니다.
	int windowWidth  = rc.right - rc.left;
	int windowHeight = rc.bottom - rc.top;

	HWND hwnd = CreateWindowA(
		"MySubWndClass",          // 등록한 클래스 이름
		"Sub Window",             // 창 제목
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, // 창 스타일
		10,10,                   // 창의 좌측 상단 좌표
		windowWidth,windowHeight,// 전체 창 크기 (비클라이언트 영역 + 테두리, 타이틀 바 등)
		hParent,                  // 부모 창 핸들 (없다면 nullptr)
		nullptr,
		hInst,
		nullptr
	);

	if(!hwnd) {
		MessageBoxA(nullptr,"CreateWindowA failed!","Error",MB_OK);
		return nullptr;
	}

	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);
	SetFocus(hParent);
	return hwnd;
}
