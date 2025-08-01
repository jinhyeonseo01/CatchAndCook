#include "pch.h"
#include "GUIInventory.h"

#include "Canvas.h"
#include "Game.h"
#include "GUIItemBox.h"
#include "RectTransform.h"


COMPONENT(GUIInventory)

shared_ptr<GUIInventory> GUIInventory::main = nullptr;

bool GUIInventory::HasEmptySlot()
{
	for (int i=0;i< InGameGlobal::InventoryCount;i++)
		if (InGameGlobal::main->invItems[i].itemCode == -1)
			return true;
	return false;
}

int GUIInventory::GetEmptySlotIndex()
{
	for (int i = 0; i < InGameGlobal::InventoryCount; i++)
		if (InGameGlobal::main->invItems[i].itemCode == -1)
			return i;
	return -1;
}

bool GUIInventory::PushItemData(const ItemData& itemData)
{
	for (int i = 0; i < InGameGlobal::InventoryCount; i++)
	{
		if (InGameGlobal::main->invItems[i].itemCode == -1)
		{
			InGameGlobal::main->invItems[i] = itemData;
			return true;
		}
	}
	return false;
}

bool GUIInventory::SetItemData(const ItemData& itemData, int index)
{
	InGameGlobal::main->invItems[index] = itemData;
	return true;
}

ItemData GUIInventory::GetItemDataIndex(int index)
{
	return InGameGlobal::main->invItems[index];
}

ItemData GUIInventory::PopItemDataIndex(int index)
{
	auto temp = InGameGlobal::main->invItems[index];
	if (index < 0 || index >= InGameGlobal::InventoryCount)
		return ItemData();
	InGameGlobal::main->invItems[index].Clear();
	return temp;
}

void GUIInventory::SlotUpdate()
{
	for (int i = 0; i < _slots.size(); i++)
	{
		if (auto item = _slots[i]->GetComponent<GUIItem>())
		{
			item->PopItemData();
			if (i < InGameGlobal::main->invItems.size())
				item->PushItemData(InGameGlobal::main->invItems[i]);
		}
	}

}

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
	for (int i=0;i< InGameGlobal::InventoryCount;i++)
	{
		wstring name = L"Slot_" + to_wstring(i);
		_slots.push_back(GetOwner()->GetChildByName(name));
	}
}

void GUIInventory::Update()
{
	Component::Update();

	SlotUpdate();

	std::shared_ptr<GameObject> selected;
	for (int i = 0; i < _slots.size(); i++)
	{
		if (Input::main->GetKeyDown(KeyCode::Num1 + i))
		{
			selectIndex = i;
			Sound::main->PlayImmediate("j_button_swap", 0.12f);
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



	Vector2 mousePos;

	if (auto canvas = GetOwner()->GetComponentWithParents<Canvas>())
		if (canvas->type == CanvasType::Overlay)
			mousePos = canvas->GetScreenToCanvasPos(Input::main->GetMousePosition());

	for (int i = 0; i < InGameGlobal::main->invItems.size(); i++)
	{
		auto slot = _slots[i];
		auto rt = slot->GetComponent<RectTransform>();
		if (Input::main->GetMouseDown(KeyCode::LeftMouse) && rt->IsBoundCanvasPos(mousePos)) // 아이템 선택
		{
			if (InGameGlobal::main->invItems[i].itemCode != -1 && GUIItemBox::main)
			{
				//먼저 인벤토리에 칸 남는지 체크
				if (GUIItemBox::main->HasEmptySlot())
				{
					//아이템을 인벤토리로
					Sound::main->PlayImmediate("j_button_click", 2.0f);
					GUIItemBox::main->PushItemData(PopItemDataIndex(i));
				}
			}
		}
	}


	if (Input::main->GetKeyDown(KeyCode::I))
	{
		GUIItemBox::main->GetOwner()->SetActiveSelf(!GUIItemBox::main->GetOwner()->GetActiveSelf());
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
