#include "pch.h"
#include "GUIInventory.h"

#include "ItemBox.h"


COMPONENT(GUIInventory)

shared_ptr<GUIInventory> GUIInventory::main = nullptr;

GUIInventory::~GUIInventory()
{
}

bool GUIInventory::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIInventory::Init()
{
	Component::Init();
	GUIInventory::main = GetCast<GUIInventory>();
}

void GUIInventory::Start()
{
	Component::Start();
	for (int i=0;i<4;i++)
	{
		wstring name = L"Slot_" + to_wstring(i);
		_slots.push_back(GetOwner()->GetChildByName(name));
	}
}

void GUIInventory::Update()
{
	Component::Update();

	std::shared_ptr<GameObject> selected;
	for (int i = 0; i < _slots.size(); i++)
	{
		if (Input::main->GetKeyDown(KeyCode::Num1 + i))
		{
			selectIndex = i;
			break;
		}
	}
	if (selectIndex != -1)
	{
		for (int i = 0; i < _slots.size(); i++)
			if (selectIndex == i)
				_slots[i]->GetChildByName(L"Selected")->SetActiveSelf(true);
			else
				_slots[i]->GetChildByName(L"Selected")->SetActiveSelf(false);
	}

	if (Input::main->GetKeyDown(KeyCode::I))
	{
		ItemBox::main->GetOwner()->SetActiveSelf(!ItemBox::main->GetOwner()->GetActiveSelf());
	}
}

void GUIInventory::Update2()
{
	Component::Update2();
}

void GUIInventory::Enable()
{
	Component::Enable();
}

void GUIInventory::Disable()
{
	Component::Disable();
}

void GUIInventory::RenderBegin()
{
	Component::RenderBegin();
}

void GUIInventory::RenderEnd()
{
	Component::RenderEnd();
}

void GUIInventory::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIInventory::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIInventory::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIInventory::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIInventory::SetDestroy()
{
	Component::SetDestroy();
}

void GUIInventory::Destroy()
{
	Component::Destroy();
	GUIInventory::main = nullptr;
}
