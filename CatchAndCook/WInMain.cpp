	#include "pch.h"
#include "WInMain.h"
#include "resource.h" 
#include "Game.h"
#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING] = L"Catch&Cook";
WCHAR szWindowClass[MAX_LOADSTRING] = L"Catch&Cook";
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND hWnd;
HCURSOR g_cursor = NULL;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow)
{

	g_cursor = LoadCursorFromFile(L"../Resources/Cursor/cursor5.ani");

	if (g_cursor == NULL)
	{
		MessageBox(NULL, L"애니메이션 커서 로드 실패", L"에러", MB_OK | MB_ICONERROR);
		return 1;
	}
	if (!g_cursor)
	{
		DWORD err = GetLastError();
		std::wcerr << L"커서 로딩 실패, 오류 코드: " << err << std::endl;
	}

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;


	MSG msg;

	auto game = Game::main = make_shared<Game>();
	game->Init(hWnd);

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			game->Run();
		}
	}

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));         // 큰 아이콘
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);             // 배경 브러시
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"Catch&Cook";                                   // 클래스 이름
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));       // 작은 아이콘


    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

	RECT wr = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowW(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		0, 0,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr,
		hInstance, nullptr);



    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	InputEvent eventDesc;
	std::memset(&eventDesc, 0, sizeof(InputEvent));
	eventDesc.type = InputType::Event;

	switch (message)
	{
	case WM_SETCURSOR:
	{
		if (g_cursor && LOWORD(lParam) == HTCLIENT) 
		{
			SetCursor(g_cursor);
			return TRUE; 
		}
		break; 
	}

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
		switch (message)
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
		switch (message)
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


	return DefWindowProc(hWnd, message, wParam, lParam);
}