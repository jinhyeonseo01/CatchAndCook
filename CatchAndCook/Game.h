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


	void AddFunction(const std::shared_ptr<GameObject>& obj, function<void()>);
	void AddFunctionBack(const std::shared_ptr<GameObject>& obj, function<void()>);
	void RemoveFunction(const std::shared_ptr<GameObject>& obj);
private:
	HWND _hwnd;
	HINSTANCE _hInstance;

	bool _fullScreen = false;
	bool _quit = false;

	std::vector<std::pair<std::shared_ptr<GameObject>, function<void()>>> escStack;

	BoundingOrientedBox box;
};

