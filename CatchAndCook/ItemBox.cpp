#include "pch.h"
#include "ItemBox.h"

#include "Canvas.h"


COMPONENT(ItemBox)

shared_ptr<ItemBox> ItemBox::main = nullptr;

ItemBox::~ItemBox()
{
}

bool ItemBox::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void ItemBox::Init()
{
	Component::Init();
	ItemBox::main = GetCast<ItemBox>();
}

void ItemBox::Start()
{
	Component::Start();
	for (int i = 0; i < 4; i++)
	{
		wstring name = L"Slot_" + to_wstring(i);
		_slots.push_back(GetOwner()->GetChildByName(name));
	}
	_exit = GetOwner()->GetChildByName(L"Exit");

}

void ItemBox::Update()
{
	Component::Update();
	if (Input::main->GetMouseDown(KeyCode::LeftMouse))
	{
		if (auto canvas = GetOwner()->GetComponentWithParents<Canvas>())
		{
			//auto rt = GetOwner()->GetComponent<RectTransform>();
			if (canvas->type == CanvasType::Overlay)
			{
				auto pos = canvas->GetScreenToCanvasPos(Input::main->GetMousePosition());
				//rt->;
			}
		}
	}
	for (int i = 0; i < _slots.size(); i++)
	{
		if (Input::main->GetMouseDown(KeyCode::Num1 + i))
		{
			_slots[i]->SetActiveSelf(true);
			_exit->SetActiveSelf(false);
			break;
		}
	}
}

void ItemBox::Update2()
{
	Component::Update2();
}

void ItemBox::Enable()
{
	Component::Enable();
}

void ItemBox::Disable()
{
	Component::Disable();
}

void ItemBox::RenderBegin()
{
	Component::RenderBegin();
}

void ItemBox::RenderEnd()
{
	Component::RenderEnd();
}

void ItemBox::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void ItemBox::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void ItemBox::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void ItemBox::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void ItemBox::SetDestroy()
{
	Component::SetDestroy();
}

void ItemBox::Destroy()
{
	Component::Destroy();
	ItemBox::main = nullptr;
}
