#include "pch.h"
#include "ItemBox.h"



COMPONENT(ItemBox)

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
}
