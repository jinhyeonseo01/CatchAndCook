#include "pch.h"
#include "GUIInventory.h"



COMPONENT(GUIInventory)

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
}

void GUIInventory::Start()
{
	Component::Start();
	for (int i=0;i<4;i++)
	{
		wstring name = L"Slot_" + to_wstring(i);
		_slots.push_back(GetOwner()->GetScene()->Find(name));
		std::cout << to_string(name) << "\n";
	}
}

void GUIInventory::Update()
{
	Component::Update();

	std::shared_ptr<GameObject> selected;
	int selectedIndex = -1;
	for (int i = 0; i < _slots.size(); i++)
	{
		if (Input::main->GetKeyDown(KeyCode::Num1 + i))
		{
			selected = _slots[i]->GetChildByName(L"Selected");
			if (selected)
			{
				selectedIndex = i;
				break;
			}
		}
	}
	if (selectedIndex != -1)
	{
		for (int i = 0; i < _slots.size(); i++)
			if (selectedIndex == i)
				_slots[i]->GetChildByName(L"Selected")->SetActiveSelf(true);
			else
				_slots[i]->GetChildByName(L"Selected")->SetActiveSelf(false);
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
}
