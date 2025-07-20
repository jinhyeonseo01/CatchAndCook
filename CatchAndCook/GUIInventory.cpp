#include "pch.h"
#include "GUIInventory.h"

#include "ItemBox.h"


COMPONENT(GUIInventory)

shared_ptr<GUIInventory> GUIInventory::main = nullptr;

int GUIInventory::HasEmptySlot()
{
	for (int i=0;i<InventoryCount;i++)
		if (_itemList[i].itemCode == -1)
			return i;
	return -1;
}

bool GUIInventory::PushItemData(const ItemData& itemData)
{
	for (int i = 0; i < InventoryCount; i++)
	{
		if (_itemList[i].itemCode == -1)
		{
			_itemList[i] = itemData;
			return true;
		}
	}
	return false;
}

bool GUIInventory::SetItemData(const ItemData& itemData, int index)
{
	_itemList[index] = itemData;
	return true;
}

ItemData GUIInventory::PopItemDataIndex(int index)
{
	auto temp = _itemList[index];
	if (index < 0 || index >= InventoryCount)
		return ItemData();
	_itemList[index].Clear();
	return temp;
}

void GUIInventory::SlotUpdate()
{
	for (int i = 0; i < _slots.size(); i++)
	{
		if (auto item = _slots[i]->GetComponent<GUIItem>())
		{
			item->PopItemData();
			if (i < _itemList.size())
				item->PushItemData(_itemList[i]);
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
	for (int i=0;i< InventoryCount;i++)
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
