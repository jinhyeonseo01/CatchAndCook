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

        if (normalizedX >= (sprite->_ndcPos.x) &&
            normalizedX <= (sprite->_ndcPos.x + sprite->_ndcSize.x) &&
            normalizedY >= (sprite->_ndcPos.y) &&
            normalizedY <= (sprite->_ndcPos.y + sprite->_ndcSize.y))
        {
            sprite->_spriteWorldParam.alpha -= 0.1f;

            for (auto& child : sprite->_children)
            {
                child->_spriteWorldParam.alpha -= 0.1f;
            }
        }
    } 
}

void ActionFunc::OnDragAction(KeyCode key, Sprite* sprite)
{
    static Sprite* _dragSprite = nullptr;
    static vec2 _lastMousePos; // ���� ���콺 ��ġ ���� ����

    // ��Ŭ�� ����
    if (Input::main->GetMouseDown(key))
    {
        vec2 pos = Input::main->GetMouseDownPosition(key);

        float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
        float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

        if (normalizedX >= (sprite->_ndcPos.x) &&
            normalizedX <= (sprite->_ndcPos.x + sprite->_ndcSize.x) &&
            normalizedY >= (sprite->_ndcPos.y) &&
            normalizedY <= (sprite->_ndcPos.y + sprite->_ndcSize.y))
        {
            _dragSprite = sprite;
            _lastMousePos = pos; 
        }
    }

    // �巡�� ��
    if (_dragSprite && Input::main->GetMouse(key))
    {
        vec2 pos = Input::main->GetMousePosition();
        vec2 delta = pos - _lastMousePos; 

        auto size = _dragSprite->_screenSize;
        _dragSprite->SetPos(vec3(_dragSprite->_screenPos.x + delta.x,
            _dragSprite->_screenPos.y + delta.y,
            _dragSprite->_spriteWorldParam.ndcPos.z));

        for (auto& child : _dragSprite->_children)
        {
            child->SetPos(vec3(child->_screenPos.x + delta.x,
                child->_screenPos.y + delta.y,
                child->_spriteWorldParam.ndcPos.z));
        }

        _lastMousePos = pos; 
    }

    // ��Ŭ�� ����
    if (_dragSprite && Input::main->GetMouseUp(key))
    {
        _dragSprite = nullptr;
    }
}

void ActionFunc::OnClickDisableAction(KeyCode key, Sprite* sprite)
{

    if (Input::main->GetMouseDown(key))
    {
        auto pos = Input::main->GetMouseDownPosition(key);

        float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
        float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

        if (normalizedX >= (sprite->_ndcPos.x) &&
            normalizedX <= (sprite->_ndcPos.x + sprite->_ndcSize.x) &&
            normalizedY >= (sprite->_ndcPos.y) &&
            normalizedY <= (sprite->_ndcPos.y + sprite->_ndcSize.y))
        {
            sprite->_parent.lock()->_enable = false;
            sprite->_enable = false;
        }
    }
}

void ActionFunc::OnKeySpriteEnableDisable(KeyCode key, Sprite* sprite)
{
    if (Input::main->GetKeyDown(key))
    {
        sprite->_enable = !sprite->_enable;

        for (auto& child : sprite->_children)
        {
            child->_enable = !child->_enable;
        }
    }
}

