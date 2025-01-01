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

    //�ѹ��� �������� (����)
    bool GetKeyDown(int keycode);
    //�ڴ����������� 
    bool GetKey(int keycode);
    //Ű�� ��������
    bool GetKeyUp(int keycode);

    bool GetMouseDown(int keycode);
    bool GetMouse(int keycode);
    bool GetMouseUp(int keycode);

    Vector2 GetMousePosition();
    Vector2 GetMouseDownPosition(int keycode);
    Vector2 GetMouseUpPosition(int keycode);

private:

    bool DataUpdate(const InputEvent& event);
    void DataBeginUpdate();

    static const int _maxInputKeyField = 512;
    static const int _maxInputMouseField = 518;
    InputKeyData _inputKeyboardField[Input::_maxInputKeyField];
    InputMouseData _inputMouseField[Input::_maxInputMouseField];
    Vector2 mousePos{ 0, 0 };



};
