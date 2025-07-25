#pragma once
//

class Game
{
public:
	static std::shared_ptr<Game> main;
	void Init(HWND hwnd);
	void PrevUpdate();
	void Run();
	void Release();
	void CameraUpdate(); //temp
	void SetHandle(HWND hwnd,HINSTANCE hInst);
	void Quit() { _quit = true; };
private:
	HWND _hwnd;
	HINSTANCE _hInstance;

	bool _fullScreen = false;
	bool _quit = false;


	BoundingOrientedBox box;
};

