#include "pch.h"
#include "SpriteAction.h"
#include "Sprite.h"

void SpriteAction::OnClickAction()
{
	if (Input::main->GetMouseDown(KeyCode::LeftMouse))
	{
		auto pos = Input::main->GetMouseDownPosition(KeyCode::LeftMouse);
			float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
			float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

			for (auto& [rect, sprite] : Sprite::_collisionMap)
			{
				if (normalizedX >= rect.left && normalizedX <= rect.right &&
					normalizedY >= rect.top && normalizedY <= rect.bottom)
				{
					sprite->_spriteWorldParam.alpha *= 0.99f;
					break;
				}
			}
	}
}

void SpriteAction::OnDragAction()
{
	static Sprite* _dragSprtie = nullptr;
	static CollisionRect* _dragRect = nullptr;

	// ��Ŭ�� ����
	if (Input::main->GetMouseDown(KeyCode::RightMouse))
	{
		vec2 pos = Input::main->GetMouseDownPosition(KeyCode::RightMouse);
		float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
		float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

		// �浹�� ��������Ʈ �˻�
		for (auto& [rect, sprite] : Sprite::_collisionMap)
		{
			if (normalizedX >= rect.left && normalizedX <= rect.right &&
				normalizedY >= rect.top && normalizedY <= rect.bottom)
			{
				_dragSprtie = sprite;
				_dragRect = &rect;
				break; // ù ��°�� �浹�� ��������Ʈ�� ����
			}
		}
	}

	// �巡�� ��
	if (_dragSprtie && Input::main->GetMouse(KeyCode::RightMouse))
	{
		vec2 pos = Input::main->GetMousePosition();
		auto size = _dragSprtie->_screenSize;
		_dragSprtie->SetPos(vec3(pos.x - size.x / 2, pos.y - size.y / 2, _dragSprtie->_spriteWorldParam.ndcPos.z));
	}

	// ��Ŭ�� ����
	if (_dragSprtie && Input::main->GetMouseUp(KeyCode::RightMouse))
	{
		_dragRect->left = _dragSprtie->_ndcPos.x;
		_dragRect->top = _dragSprtie->_ndcPos.y;
		_dragRect->right = _dragSprtie->_ndcPos.x + _dragSprtie->_ndcSize.x;
		_dragRect->bottom = _dragSprtie->_ndcPos.y + _dragSprtie->_ndcSize.y;

		// �巡�� ���� ����
		_dragSprtie = nullptr;
		_dragRect = nullptr;
	};
}
