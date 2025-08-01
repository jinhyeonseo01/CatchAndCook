#include "pch.h"
#include "GUIItemBox.h"

#include "Canvas.h"
#include "Game.h"
#include "GUIInventory.h"
#include "RectTransform.h"


COMPONENT(GUIItemBox)

shared_ptr<GUIItemBox> GUIItemBox::main = nullptr;


bool GUIItemBox::PushItemData(const ItemData& itemData)
{
	if (InGameGlobal::main->boxItems.size() < _slots.size())
	{
		
		InGameGlobal::main->boxItems.push_back(itemData);
		return true;
	}
	return false;
}

ItemData GUIItemBox::GetItemDataIndex(int index)
{
	return InGameGlobal::main->boxItems[index];
}

bool GUIItemBox::RemoveItemDataIndex(int index)
{
	if (InGameGlobal::main->boxItems.size() <= index)
		return false;
	InGameGlobal::main->boxItems.erase(InGameGlobal::main->boxItems.begin() + index);
	return true;
}

bool GUIItemBox::HasEmptySlot()
{
	for (int i = 0; i < _slots.size(); i++)
	{
		if (i >= InGameGlobal::main->boxItems.size() || InGameGlobal::main->boxItems[i].itemCode == -1)
			return true;
	}
	return false;
}

int GUIItemBox::GetEmptySlotIndex()
{
	for (int i = 0; i < _slots.size(); i++)
	{
		if (i >= InGameGlobal::main->boxItems.size() || InGameGlobal::main->boxItems[i].itemCode == -1)
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


}

void GUIItemBox::SlotUpdate()
{
	//아이템 갱신//_selectedIndex
	for (int i = 0; i < _slots.size(); i++)
	{
		if (auto item = _slots[i]->GetComponent<GUIItem>())
		{
			item->PopItemData();
			if (InGameGlobal::main->boxItems.size() > i)
				item->PushItemData(InGameGlobal::main->boxItems[i]);
		}
	}

}

void GUIItemBox::Update()
{
	Component::Update();

	SlotUpdate();

	Game::main->AddFunction(GetOwner(), [=]()
		{
			GetOwner()->SetActiveSelf(false);
		});

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
			if (i < InGameGlobal::main->boxItems.size())
			{
				_selectedItemData = slot->GetComponent<GUIItem>()->GetItemData();
				_selectedIndex = i;//_slots
			/*	std::cout << "push\n";*/
			}
		}

		if (Input::main->GetMouseUp(KeyCode::LeftMouse) && rt->IsBoundCanvasPos(mousePos))
		{
	/*		std::cout << "pop\n";*/
			if (_selectedIndex != -1)
			{
				if (_selectedIndex == i)
				{
					if (GUIInventory::main)
					{
						//먼저 인벤토리에 칸 남는지 체크
						if (GUIInventory::main->HasEmptySlot())
						{
							//아이템을 인벤토리로
							RemoveItemDataIndex(i);
							GUIInventory::main->PushItemData(_selectedItemData);
							Sound::main->PlayImmediate("j_button_click", 2.0f);
						}
					}
				}
				else
				{
					if (i < InGameGlobal::main->boxItems.size()) // 스왑
						std::swap(InGameGlobal::main->boxItems[i], InGameGlobal::main->boxItems[_selectedIndex]);
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
	Game::main->RemoveFunction(GetOwner());
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
