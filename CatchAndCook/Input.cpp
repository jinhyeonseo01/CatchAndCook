#include "pch.h"
#include "Input.h"


unique_ptr<Input> Input::main = nullptr;

Input::Input()
{

}

Input::~Input()
{


}

void Input::Update()
{
    prevMousePos = mousePos;

    Input::main->DataBeginUpdate();

    while (!_eventQueue.empty())
    {
        Input::main->DataUpdate(_eventQueue.front());
        Input::main->_eventQueue.pop();
    }

    if (GetKeyDown(KeyCode::BackQoute))
    {
        SetMouseLock(!IsMouseLock());
    }

    if (IsMouseLock())
    {
        Vector2 windowCenter = Vector2(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
        float limitDistance = std::min(windowCenter.x, windowCenter.y) / 2;
        Vector2 centerToPosDirection = GetMousePosition() - windowCenter;
        Vector2 prevToCurrentDirection = GetMousePosition() - GetMousePrevPosition();
        float centerToPosDistance = centerToPosDirection.Length();
        float prevToCurrentDistance = prevToCurrentDirection.Length();
        centerToPosDirection.Normalize();
        prevToCurrentDirection.Normalize();

        if (centerToPosDistance >= limitDistance)
        {
            mousePos = windowCenter + centerToPosDirection * (centerToPosDistance - limitDistance);

            prevMousePos = mousePos + (-prevToCurrentDirection * prevToCurrentDistance);

            SetCursor(mousePos);
        }
    }
}

void Input::SetCursor(const Vector2& pos)
{
    POINT pt;
    pt.x = (int)pos.x; // 창 내부 상대 좌표
    pt.y = (int)pos.y; // 창 내부 상대 좌표
    ClientToScreen(Core::main->GetHandle(), &pt);
    SetCursorPos(pt.x, pt.y);
}

void Input::SetMouseLock(bool isMouseLock)
{

    _isMouseLock = isMouseLock;

    if (_isMouseLock)
    {
        mousePos = prevMousePos = Vector2(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
        SetCursor(mousePos);
		ShowCursor(false);
    }
    else
    {
		ShowCursor(true);
    }

}



bool Input::GetKeyDown(int keycode)
{
    return _inputKeyboardField[keycode].down;
}

bool Input::GetKey(int keycode)
{
    return _inputKeyboardField[keycode].stay;
}

bool Input::GetKeyUp(int keycode)
{
    return _inputKeyboardField[keycode].up;
}

bool Input::GetMouseDown(int keycode)
{
    return _inputMouseField[keycode].down;
}

bool Input::GetMouse(int keycode)
{
    return _inputMouseField[keycode].stay;
}

bool Input::GetMouseUp(int keycode)
{
    return _inputMouseField[keycode].up;
}


bool Input::DataUpdate(const InputEvent& e)
{
    switch (e.type)
    {
    case InputType::Keyboard:
        if ((!_inputKeyboardField[e.keyCode].stay) && e.keyboard.isDown)
            _inputKeyboardField[e.keyCode].down = e.keyboard.isDown;

        if ((_inputKeyboardField[e.keyCode].stay) && e.keyboard.isUp)
            _inputKeyboardField[e.keyCode].up = e.keyboard.isUp;

        if (_inputKeyboardField[e.keyCode].down)
        {
            if (_inputKeyboardField[e.keyCode].stay)
                _inputKeyboardField[e.keyCode].up = true;
            _inputKeyboardField[e.keyCode].stay = true;
            _inputKeyboardField[e.keyCode]._lastDown_Time = e.time;
        }

        if (_inputKeyboardField[e.keyCode].up)
        {
            _inputKeyboardField[e.keyCode]._lastUp_Time = e.time;
        }

        return true;
        break;

    case InputType::Mouse:
        if ((!e.mouse.isUp) && (!e.mouse.isDown))
        {
            mousePos = Vector2(e.mouse.posX, e.mouse.posY);
        }
        else
        {
            if ((!_inputMouseField[e.keyCode].down) && e.mouse.isDown)
                _inputMouseField[e.keyCode].down = e.mouse.isDown;
            if ((!_inputMouseField[e.keyCode].up) && e.mouse.isUp)
                _inputMouseField[e.keyCode].up = e.mouse.isUp;

            if (_inputMouseField[e.keyCode].down)
            {
                if (_inputMouseField[e.keyCode].stay)
                    _inputMouseField[e.keyCode].up = true;
                _inputMouseField[e.keyCode].stay = true;
                _inputMouseField[e.keyCode]._lastDown_Time = e.time;
                _inputMouseField[e.keyCode].downPos = Vector2(e.mouse.posX, e.mouse.posY);
            }
            if (_inputMouseField[e.keyCode].up)
            {
                _inputMouseField[e.keyCode]._lastUp_Time = e.time;
                _inputMouseField[e.keyCode].upPos = Vector2(e.mouse.posX, e.mouse.posY);
            }
        }
        return true;
        break;
    }
    return false;
}


void Input::DataBeginUpdate()
{
    for (int i = 0; i < KeyCode::KEYBOARD_KEYCODE_ENDPOINT; i++)
    {
        if (_inputKeyboardField[i].down)
            _inputKeyboardField[i].down = false;
        if (_inputKeyboardField[i].up)
        {
            _inputKeyboardField[i].stay = false;
            _inputKeyboardField[i].down = false;
            _inputKeyboardField[i].up = false;
        }
    }
    for (int i = static_cast<short>(KeyCode::MOUSE_KEYCODE_OFFSET); i < Input::_maxInputMouseField; i++)
    {
        if (_inputMouseField[i].down)
            _inputMouseField[i].down = false;
        if (_inputMouseField[i].up)
        {
            _inputMouseField[i].stay = false;
            _inputMouseField[i].down = false;
            _inputMouseField[i].up = false;
        }
    }
}

Vector2 Input::GetMousePosition()
{
    return mousePos;
}
Vector2 Input::GetMousePrevPosition()
{
    return prevMousePos;
}


Vector2 Input::GetMouseDownPosition(int keycode)
{
    return _inputMouseField[keycode].downPos;
}

vec2 Input::GetNDCMouseDownPosition(int keycode)
{
    vec2 screenPos = _inputMouseField[keycode].downPos;

    float cursorNdcX = screenPos.x * 2.0f / WINDOW_WIDTH - 1.0f;
    float cursorNdcY = -screenPos.y * 2.0f / WINDOW_HEIGHT + 1.0f;

	return vec2(cursorNdcX, cursorNdcY);
}

Vector2 Input::GetMouseUpPosition(int keycode)
{
    return _inputMouseField[keycode].upPos;
}
