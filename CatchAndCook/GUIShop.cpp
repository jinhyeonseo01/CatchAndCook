#include "pch.h"
#include "GUIShop.h"
#include "RectTransform.h"

#include "Canvas.h"
#include "Game.h"
#include "GUIInventory.h"
#include "GUIItemBox.h"


COMPONENT(GUIShop)

GUIShop::~GUIShop()
{

}

bool GUIShop::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIShop::Init()
{
	Component::Init();
}

void GUIShop::Start()
{
	Component::Start();

	//"Slot_0"
	int i = 0;
	while (auto slot = GetOwner()->GetChildByName(L"Slot_" + to_wstring(i)))
	{
		slots.push_back(slot);
		++i;
	}
	i = 0;
	while (auto slot = GetOwner()->GetChildByName(L"Button_" + to_wstring(i)))
	{
		buttons.push_back(slot);
		++i;
	}

	shopDatas.push_back(make_pair(1, ItemData(7, -1)));
	//shopDatas.push_back(make_pair(2, ItemData(3, -1)));
	//shopDatas.push_back(make_pair(2, ItemData(4, -1)));
	//shopDatas.push_back(make_pair(1, ItemData(5, -1)));
	//shopDatas.push_back(make_pair(1, ItemData(6, -1)));
	shopDatas.push_back(make_pair(9, ItemData(12, -1)));
}

void GUIShop::Update()
{
	Component::Update();

	Game::main->AddFunctionBack(GetOwner(), [=]()
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
				Sound::main->PlayImmediate("j_button_click", 2.0f);
			}
		}
	}

	for (int i=0;i< slots.size();i++)
	{
		slots[i]->GetComponent<GUIItem>()->PopItemData();
		if (i < shopDatas.size())
			slots[i]->GetComponent<GUIItem>()->PushItemData(shopDatas[i].second);
	}
	for (int i = 0; i < buttons.size(); i++)
	{
		if (i < shopDatas.size())
		{
			if (auto num = buttons[i]->GetChildByName(L"Num"))
			{
				for (int j = 0; j < 10; j++)
				{
					if (auto num2 = num->GetChildByName(to_wstring(j)))
					{
						if (shopDatas[i].first == j)
							num2->SetActiveSelf(true);
						else
							num2->SetActiveSelf(false);
					}
				}
			}
		}
	}

	if (Input::main->GetMouseDown(KeyCode::LeftMouse))
	{
		for (int i = 0; i < buttons.size(); i++)
		{
			if (i < shopDatas.size())
			{
				if (buttons[i]->GetComponent<RectTransform>()->IsBoundCanvasPos(mousePos))
				{
					if (InGameGlobal::main->gold >= shopDatas[i].first)
					{
						InGameGlobal::main->gold -= shopDatas[i].first;
						Sound::main->PlayImmediate("j_button_click", 2.0f);
						if (GUIInventory::main && GUIInventory::main->HasEmptySlot())
						{
							GUIInventory::main->PushItemData(shopDatas[i].second);
						}
						else if (GUIItemBox::main && GUIItemBox::main->HasEmptySlot())
						{
							GUIItemBox::main->PushItemData(shopDatas[i].second);
						}
					}
				}
			}
		}
	}

}

void GUIShop::Update2()
{
	Component::Update2();
}

void GUIShop::Enable()
{
	Component::Enable();
}

void GUIShop::Disable()
{
	Component::Disable();
	Game::main->RemoveFunction(GetOwner());
	Input::main->SetMouseLock(true);
}

void GUIShop::RenderBegin()
{
	Component::RenderBegin();
}

void GUIShop::RenderEnd()
{
	Component::RenderEnd();
}

void GUIShop::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIShop::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIShop::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIShop::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIShop::SetDestroy()
{
	Component::SetDestroy();
}

void GUIShop::Destroy()
{
	Component::Destroy();
}

void GUIShop::Reset()
{
	Component::Reset();
}
