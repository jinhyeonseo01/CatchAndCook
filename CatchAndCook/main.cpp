#include "pch.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "Game.h"
#include "Shader.h"

unique_ptr<Game> game;
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


	WNDCLASSEX wc = { sizeof(WNDCLASSEX),
				CS_CLASSDC,
				WndProc,
				0L,
				0L,
				GetModuleHandle(NULL),
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

	// �����츦 ���鶧 ������ ����� wr ���
	HWND hwnd = CreateWindow(wc.lpszClassName, L"Game",
		WS_OVERLAPPEDWINDOW,
		100, // ������ ���� ����� x ��ǥ
		100, // ������ ���� ����� y ��ǥ
		wr.right - wr.left, // ������ ���� ���� �ػ�
		wr.bottom - wr.top, // ������ ���� ���� �ػ�
		NULL, NULL, wc.hInstance, NULL);



	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	game = make_unique<Game>();
	game->Init(hwnd);

	MSG msg = {};


	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			game->Run();
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
	        int deltaZ = (wParam >> 16) & 0xffff;
	        char sign = std::sign(deltaZ);
	        eventDesc.type = InputType::Mouse;
	        eventDesc.mouse.posX = 0;
	        eventDesc.mouse.posY = deltaZ;
	        eventDesc.mouse.isDown = sign < 0;
	        eventDesc.mouse.isUp = sign > 0;
	        eventDesc.mouse.isCtrl = wParam & MK_CONTROL;
	        eventDesc.mouse.isShift = wParam & MK_SHIFT;
	        eventDesc.keyCode = KeyCode::CenterMouse;
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