#include "pch.h"
#include "GUIItemBox.h"

#include "Canvas.h"
#include "GUIInventory.h"
#include "RectTransform.h"


COMPONENT(GUIItemBox)

shared_ptr<GUIItemBox> GUIItemBox::main = nullptr;


bool GUIItemBox::AddItemData(const ItemData& itemData)
{
	if (_itemList.size() < _slots.size())
	{
		_itemList.push_back(itemData);
		return true;
	}
	return false;
}

ItemData GUIItemBox::GetItemDataIndex(int index)
{
	return _itemList[index];
}

bool GUIItemBox::RemoveItemDataIndex(int index)
{
	if (_itemList.size() <= index)
		return false;
	_itemList.erase(_itemList.begin() + index);
	return true;
}

int GUIItemBox::HasEmptySlot()
{
	for (int i = 0; i < _slots.size(); i++)
	{
		if (i >= _itemList.size() || _itemList[i].itemCode == -1)
			return i;
	}
	return -1;
}


GUIItemBox::~GUIItemBox()
{
}

bool GUIItemBox::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIItemBox::Init()
{
	Component::Init();
	GUIItemBox::main = GetCast<GUIItemBox>();

}

void GUIItemBox::Start()
{
	Component::Start();
	int i = 0;
	while (auto slot = GetOwner()->GetChildByName(L"Slot_" + to_wstring(i)))
	{
		_slots.push_back(slot);
		i++;
	}
	_exit = GetOwner()->GetChildByName(L"Exit");


	ItemData itemData;

	itemData.itemCode = 0;
	itemData.itemCookType = -1;
	AddItemData(itemData);

	itemData.itemCode = 1;
	itemData.itemCookType = -1;
	AddItemData(itemData);
	itemData.itemCode = 2;
	itemData.itemCookType = 0;
	AddItemData(itemData);

}

void GUIItemBox::SlotUpdate()
{
	//아이템 갱신//_selectedIndex
	for (int i = 0; i < _slots.size(); i++)
	{
		if (auto item = _slots[i]->GetComponent<GUIItem>())
		{
			item->PopItemData();
			if (_itemList.size() > i)
				item->PushItemData(_itemList[i]);
		}
	}

}

void GUIItemBox::Update()
{
	Component::Update();

	SlotUpdate();

	Vector2 mousePos;

	if (auto canvas = GetOwner()->GetComponentWithParents<Canvas>())
		if (canvas->type == CanvasType::Overlay)
			mousePos = canvas->GetScreenToCanvasPos(Input::main->GetMousePosition());

	if (Input::main->GetMouseDown(KeyCode::LeftMouse))
	{

		if (auto exitObj = GetOwner()->GetChildByName(L"Exit"))
		{
			if (exitObj->GetComponent<RectTransform>()->IsBoundScreenPos(Input::main->GetMousePosition()))
			{
				GetOwner()->SetActiveSelf(false);
			}
		}
	}

	for (int i =0; i < _slots.size(); i++)
	{
		auto slot = _slots[i];
		auto rt = slot->GetComponent<RectTransform>();
		if (Input::main->GetMouseDown(KeyCode::LeftMouse) && rt->IsBoundCanvasPos(mousePos)) // 아이템 선택
		{
			if (i < _itemList.size())
			{
				_selectedItemData = slot->GetComponent<GUIItem>()->GetItemData();
				_selectedIndex = i;//_slots
				std::cout << "push\n";
			}
		}

		if (Input::main->GetMouseUp(KeyCode::LeftMouse) && rt->IsBoundCanvasPos(mousePos))
		{
			std::cout << "pop\n";
			if (_selectedIndex != -1)
			{
				if (_selectedIndex == i)
				{
					if (GUIInventory::main)
					{
						//먼저 인벤토리에 칸 남는지 체크
						if (GUIInventory::main->HasEmptySlot() != -1)
						{
							//아이템을 인벤토리로
							RemoveItemDataIndex(i);
							GUIInventory::main->PushItemData(_selectedItemData);
						}
					}
				}
				else
				{
					if (i < _itemList.size()) // 스왑
						std::swap(_itemList[i], _itemList[_selectedIndex]);
				}
			}

			_selectedItemData.Clear();
			_selectedIndex = -1;
		}
	}

}

void GUIItemBox::Update2()
{
	Component::Update2();
}

void GUIItemBox::Enable()
{
	Component::Enable();
	Input::main->SetMouseLock(false);
}

void GUIItemBox::Disable()
{
	Component::Disable();
	Input::main->SetMouseLock(true);
}

void GUIItemBox::RenderBegin()
{
	Component::RenderBegin();
}

void GUIItemBox::RenderEnd()
{
	Component::RenderEnd();
}

void GUIItemBox::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIItemBox::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIItemBox::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIItemBox::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIItemBox::SetDestroy()
{
	Component::SetDestroy();
}

void GUIItemBox::Destroy()
{
	Component::Destroy();
	GUIItemBox::main = nullptr;
}
