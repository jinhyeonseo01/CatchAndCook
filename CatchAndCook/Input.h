#pragma once

#include "pch.h"
#include "input_module.h"


class InputKeyData
{
public:
    bool down;
    bool stay;
    bool up;

    std::chrono::time_point<std::chrono::steady_clock> _lastDown_Time;
    std::chrono::time_point<std::chrono::steady_clock> _lastUp_Time;
};

class InputMouseData : public InputKeyData
{
public:
    Vector2 downPos{0, 0};
    Vector2 upPos{0, 0};
};

class Input 
{
public:
    static unique_ptr<Input> main;

    Input();
    ~Input();

    void Update();
  

public:

    std::queue<InputEvent> _eventQueue;

    //한번만 눌렸을때 (최초)
    bool GetKeyDown(int keycode);
    //꾹누르고있을때 
    bool GetKey(int keycode);
    //키를 업했을때
    bool GetKeyUp(int keycode);

    bool GetMouseDown(int keycode);
    bool GetMouse(int keycode);
    bool GetMouseUp(int keycode);

    void SetMousePosition(const vec2& pos);
    vec2 GetMousePosition();
    Vector2 GetMousePrevPosition();
    vec2 GetMouseDownPosition(int keycode);
	vec2 GetNDCMouseDownPosition(int keycode);
    vec2 GetMouseUpPosition(int keycode);

    bool IsMouseLock() { return _isMouseLock; }
    void SetMouseLock(bool isMouseLock);
    void SetCursor(const Vector2& pos);

private:
    bool _isMouseLock = false;
    bool DataUpdate(const InputEvent& event);
    void DataBeginUpdate();

    static const int _maxInputKeyField = 512;
    static const int _maxInputMouseField = 540;
    InputKeyData _inputKeyboardField[Input::_maxInputKeyField];
    InputMouseData _inputMouseField[Input::_maxInputMouseField];
    Vector2 mousePos{ 0, 0 };
    Vector2 prevMousePos{ 0, 0 };


};
