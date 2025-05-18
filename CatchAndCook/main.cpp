#include "pch.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "Game.h"
#include "Shader.h"

unique_ptr<Game> game;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	HINSTANCE hInst = GetModuleHandle(NULL);
	WNDCLASSEX wc = { sizeof(WNDCLASSEX),
				CS_CLASSDC,
				WndProc,
				0L,
				0L,
		hInst,
				NULL,
				NULL,
				NULL,
				NULL,
				L"Game", // lpszClassName, L-string
				NULL };

	// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerclassa?redirectedfrom=MSDN
	if (!RegisterClassEx(&wc)) {
		cout << "RegisterClassEx() failed." << endl;
	}

	RECT wr = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };


	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	// 윈도우를 만들때 위에서 계산한 wr 사용
	HWND hwnd = CreateWindow(wc.lpszClassName, L"Game",
		WS_OVERLAPPEDWINDOW,
		100, // 윈도우 좌측 상단의 x 좌표
		100, // 윈도우 좌측 상단의 y 좌표
		wr.right - wr.left, // 윈도우 가로 방향 해상도
		wr.bottom - wr.top, // 윈도우 세로 방향 해상도
		NULL, NULL, wc.hInstance, NULL);

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	game = make_unique<Game>();
	game->SetHandle(hwnd, hInst);
	game->Init(hwnd);

	MSG msg = {};


	while (WM_QUIT != msg.message) 
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
		/*	 auto frameStart = std::chrono::high_resolution_clock::now();*/

			game->Run(); // 게임 프레임 실행

			//auto frameEnd = std::chrono::high_resolution_clock::now();
			//std::chrono::duration<float> frameDuration = frameEnd - frameStart;

			//// 10 FPS → 프레임당 100ms 간격 유지
			//float targetFrameTime = 1.0f / 10.0f;
			//float sleepTime = targetFrameTime - frameDuration.count();

			//if (sleepTime > 0.0f) {
   //         std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
			//}
		}
	}


	game->Release();
	game.reset(nullptr);

	DestroyWindow(hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return 0;
};

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	    InputEvent eventDesc;
	    std::memset(&eventDesc, 0, sizeof(InputEvent));
	    eventDesc.type = InputType::Event;
		
		switch (msg)
		{
	
		case WM_SIZE:
		{
			if (Initalize)
			{
				RECT	rect;
				GetClientRect(hWnd, &rect);
				WINDOW_WIDTH = rect.right - rect.left;
				WINDOW_HEIGHT = rect.bottom - rect.top;

				if (WINDOW_WIDTH <= 0 || WINDOW_HEIGHT <= 0)
					break;

				Core::main->ResizeWindowSize();
			}
			break;
		}
		
	    case WM_LBUTTONUP:
	    case WM_RBUTTONUP:
	    case WM_MBUTTONUP:
	    case WM_XBUTTONUP:
	    {
	        eventDesc.type = InputType::Mouse;
	        eventDesc.mouse.isCtrl = wParam & MK_CONTROL;
	        eventDesc.mouse.isShift = wParam & MK_SHIFT;
	        eventDesc.mouse.isUp = true;
	        eventDesc.mouse.isInClient = true;
	        eventDesc.mouse.posX = static_cast<float>(LOWORD(lParam));
	        eventDesc.mouse.posY = static_cast<float>(HIWORD(lParam));
	        switch (msg)
	        {
	        case WM_LBUTTONUP: eventDesc.keyCode = KeyCode::LeftMouse;
	            break;
	        case WM_RBUTTONUP: eventDesc.keyCode = KeyCode::RightMouse;
	            break;
	        case WM_MBUTTONUP: eventDesc.keyCode = KeyCode::CenterMouse;
	            break;
	        default:
	            if (wParam & MK_XBUTTON1) eventDesc.keyCode = KeyCode::X1Mouse;
	            if (wParam & MK_XBUTTON1) eventDesc.keyCode = KeyCode::X2Mouse;
	            break;
	        }
	        Input::main->_eventQueue.push(eventDesc);
	        break;
	    }
	    case WM_LBUTTONDOWN:
	    case WM_RBUTTONDOWN:
	    case WM_MBUTTONDOWN:
	    case WM_XBUTTONDOWN:
	    {
	        eventDesc.type = InputType::Mouse;
	        eventDesc.mouse.isCtrl = wParam & MK_CONTROL;
	        eventDesc.mouse.isShift = wParam & MK_SHIFT;
	        eventDesc.mouse.isDown = true;
	        eventDesc.mouse.isInClient = true;
	        eventDesc.mouse.posX = static_cast<float>(LOWORD(lParam));
	        eventDesc.mouse.posY = static_cast<float>(HIWORD(lParam));
	        switch (msg)
	        {
	        case WM_LBUTTONDOWN: eventDesc.keyCode = KeyCode::LeftMouse;
	            break;
	        case WM_RBUTTONDOWN: eventDesc.keyCode = KeyCode::RightMouse;
	            break;
	        case WM_MBUTTONDOWN: eventDesc.keyCode = KeyCode::CenterMouse;
	            break;
	        default:
	            if (wParam & MK_XBUTTON1) eventDesc.keyCode = KeyCode::X1Mouse;
	            if (wParam & MK_XBUTTON1) eventDesc.keyCode = KeyCode::X2Mouse;
	            break;
	        }
	        Input::main->_eventQueue.push(eventDesc);
	        break;
	    }
	    case WM_NCMOUSEMOVE:
	    {
	        eventDesc.type = InputType::Mouse;
	        eventDesc.mouse.posX = static_cast<float>(LOWORD(lParam));
	        eventDesc.mouse.posY = static_cast<float>(HIWORD(lParam));
	        eventDesc.mouse.isInClient = false;
	        Input::main->_eventQueue.push(eventDesc);
	        break;
	    }
	    case WM_MOUSEMOVE:
	    {
	        eventDesc.type = InputType::Mouse;
	        eventDesc.mouse.posX = static_cast<float>(LOWORD(lParam));
	        eventDesc.mouse.posY = static_cast<float>(HIWORD(lParam));
	        eventDesc.mouse.isInClient = true;
	        Input::main->_eventQueue.push(eventDesc);
	        break;
	    }
	    case WM_MOUSEWHEEL:
	    {
	        short deltaZ = (wParam >> 16) & 0xffff;
	        char sign = std::sign(deltaZ);
	        eventDesc.type = InputType::Mouse;
	        eventDesc.mouse.posX = 0;
	        eventDesc.mouse.posY = deltaZ;
			eventDesc.mouse.isDown = true;
			eventDesc.mouse.isUp = true;
			eventDesc.mouse.isCtrl = wParam & MK_CONTROL;
			eventDesc.mouse.isShift = wParam & MK_SHIFT;
			eventDesc.keyCode = (sign > 0) ? KeyCode::WheelUpMouse : KeyCode::WheelDownMouse;
			eventDesc.mouse.wheelDelta= GET_WHEEL_DELTA_WPARAM(wParam);
			eventDesc.mouse.wheelDeltaH = GET_WHEEL_DELTA_WPARAM(wParam);
	        Input::main->_eventQueue.push(eventDesc);
	        break;
	    }
	  
	    case WM_KEYUP:
		{
			short repeat = lParam & 0xFFFF;
			short scanCode = (lParam >> 16) & 0xFF;
			short extendedKey = (lParam >> 16) & 0xFF;
			bool isAlt = (lParam >> 29) & 0x1;
			bool isFirst = !((lParam >> 30) & 0x1);
			bool isDown = !((lParam >> 31) & 0x1);
			bool isUp = !isDown;

			std::memset(&eventDesc, 0, sizeof(InputEvent));
			eventDesc.type = InputType::Keyboard;
			eventDesc.keyCode = wParam;
			eventDesc.keyboard.repeat = repeat;
			eventDesc.keyboard.scanCode = scanCode;
			eventDesc.keyboard.extendedKey = extendedKey;
			eventDesc.keyboard.isFirst = isFirst;
			eventDesc.keyboard.isAlt = isAlt;
			eventDesc.keyboard.isDown = isDown;
			eventDesc.keyboard.isUp = isUp;
			Input::main->_eventQueue.push(eventDesc);
			break;
		}
	    case WM_KEYDOWN:
	    {
	        short repeat = lParam & 0xFFFF;
	        short scanCode = (lParam >> 16) & 0xFF;
	        short extendedKey = (lParam >> 16) & 0xFF;
	        bool isAlt = (lParam >> 29) & 0x1;
	        bool isFirst = !((lParam >> 30) & 0x1);
	        bool isDown = !((lParam >> 31) & 0x1);
	        bool isUp = !isDown;

	        std::memset(&eventDesc, 0, sizeof(InputEvent));
	        eventDesc.type = InputType::Keyboard;
	        eventDesc.keyCode = wParam;
	        eventDesc.keyboard.repeat = repeat;
	        eventDesc.keyboard.scanCode = scanCode;
	        eventDesc.keyboard.extendedKey = extendedKey;
	        eventDesc.keyboard.isFirst = isFirst;
	        eventDesc.keyboard.isAlt = isAlt;
	        eventDesc.keyboard.isDown = isDown;
	        eventDesc.keyboard.isUp = isUp;
	        Input::main->_eventQueue.push(eventDesc);
	        break;
	    }
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;

	    case WM_SYSKEYDOWN:
	    case WM_SYSKEYUP:
	    {
	        if (wParam != 18)
	            break;
	        short repeat = lParam & 0xFFFF;
	        short scanCode = (lParam >> 16) & 0xFF;
	        short extendedKey = (lParam >> 16) & 0xFF;
	        bool isAlt = (lParam >> 29) & 0x1;
	        bool isFirst = !((lParam >> 30) & 0x1);
	            bool isDown = !((lParam >> 31) & 0x1);
	            bool isUp = !isDown;

	            std::memset(&eventDesc, 0, sizeof(InputEvent));
	            eventDesc.type = InputType::Keyboard;
	            eventDesc.keyCode = wParam;
	            eventDesc.keyboard.repeat = repeat;
	            eventDesc.keyboard.scanCode = scanCode;
	            eventDesc.keyboard.extendedKey = extendedKey;
	            eventDesc.keyboard.isFirst = isFirst;
	            eventDesc.keyboard.isAlt = isAlt;
	            eventDesc.keyboard.isDown = isDown;
	            eventDesc.keyboard.isUp = isUp;
	            Input::main->_eventQueue.push(eventDesc);
	            break;
	        }
	    }


	return DefWindowProc(hWnd, msg, wParam, lParam);
}