#include "pch.h"
#include "SpriteAction.h"
#include "Sprite.h"


void ActionFunc::OnClickAction(KeyCode key, Sprite* sprite)
{

    if (Input::main->GetMouseDown(key))
    {
        auto pos = Input::main->GetMouseDownPosition(key);

        float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
        float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

        if (normalizedX >= (sprite->_ndcWorldPos.x) &&
            normalizedX <= (sprite->_ndcWorldPos.x + sprite->_ndcSize.x) &&
            normalizedY >= (sprite->_ndcWorldPos.y) &&
            normalizedY <= (sprite->_ndcWorldPos.y + sprite->_ndcSize.y))
        {

        }
    }
}

void ActionFunc::OnDragAction(KeyCode key, Sprite* sprite)
{
    static Sprite* _dragSprite = nullptr;
    static vec2 _lastMousePos; // 이전 마우스 위치 추적 변수

    // 우클릭 시작
    if (Input::main->GetMouseDown(key))
    {
        vec2 pos = Input::main->GetMouseDownPosition(key);

        float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
        float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

        _lastMousePos = pos;

        if (normalizedX >= (sprite->_ndcWorldPos.x) &&
            normalizedX <= (sprite->_ndcWorldPos.x + sprite->_ndcSize.x) &&
            normalizedY >= (sprite->_ndcWorldPos.y) &&
            normalizedY <= (sprite->_ndcWorldPos.y + sprite->_ndcSize.y))
        {
            _dragSprite = sprite;
        }
    }

    // 드래그 중
    if (_dragSprite && Input::main->GetMouse(key))
    {
        vec2 pos = Input::main->GetMousePosition();
        vec2 delta = pos - _lastMousePos;

        delta.x = delta.x ;
        delta.y = delta.y ;

        _dragSprite->SetLocalPos(vec3(_dragSprite->_screenLocalPos.x + delta.x,
            _dragSprite->_screenLocalPos.y + delta.y,
            0.1f));

        _lastMousePos = pos;
    }

    // 우클릭 종료
    if (_dragSprite && Input::main->GetMouseUp(key))
    {
        _dragSprite = nullptr;
    }
}

void ActionFunc::DisableMouseAction(KeyCode key, Sprite* sprite)
{
    if (Input::main->GetMouseDown(key))
    {
        auto pos = Input::main->GetMouseDownPosition(key);

        float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
        float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

        if (normalizedX >= (sprite->_ndcWorldPos.x) &&
            normalizedX <= (sprite->_ndcWorldPos.x + sprite->_ndcSize.x) &&
            normalizedY >= (sprite->_ndcWorldPos.y) &&
            normalizedY <= (sprite->_ndcWorldPos.y + sprite->_ndcSize.y))
        {
            sprite->GetOwner()->SetActiveSelf(!sprite->GetOwner()->GetActive());
        }
    }
}

void ActionFunc::EnableDisableKeyAction(KeyCode key, Sprite* sprite)
{
    if (Input::main->GetKeyDown(key))
    {
        sprite->GetOwner()->SetActiveSelf(!sprite->GetOwner()->GetActive());
    }
}

