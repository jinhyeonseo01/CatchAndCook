#include "pch.h"
#include "CookObject.h"
#include "Collider.h"
#include "GUICookProgress.h"
#include "GUIInventory.h"
#include "GUIItemBox.h"
#include "Item.h"
#include "PhysicsComponent.h"
#include "Transform.h"

COMPONENT(CookObject)

bool CookObject::HasOwnItem()
{
	return ownItemData.itemCode != -1;
}

bool CookObject::PushItemData(const ItemData& item)
{
	if (ownItemData.itemCode == -1)
	{
		ownItemData = item;
		return true;
	}
	return false;
}

ItemData CookObject::PopItemData()
{
	ItemData temp = ownItemData;
	ownItemData.Clear();
	return temp;
}

CookObject::~CookObject()
{
}

bool CookObject::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void CookObject::Init()
{
	Component::Init();
}

void CookObject::Start()
{
	Component::Start();
}

void CookObject::Update()
{
	Component::Update();

	if (cookType == 0 && onTrigger)
	{
		if (Input::main->GetKeyDown(KeyCode::F))
		{

			std::vector<shared_ptr<GameObject>> cookUIs;
			GetOwner()->GetScene()->Finds(L"CookUI_Cooking", cookUIs);
			if (cookUIs.size() != 0)
			{
				for (auto& ui : cookUIs)
					if (ui->GetActiveSelf() == false)
						selectedUI = ui;
				if (selectedUI && GUIInventory::main && GUIInventory::main->GetItemDataIndex(GUIInventory::main->selectIndex).itemCode != -1
					&& GUIInventory::main && GUIInventory::main->GetItemDataIndex(GUIInventory::main->selectIndex).itemCookType != 2
					&& !HasOwnItem())
				{
					if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
						target->SetActiveSelf(false);

					PushItemData(GUIInventory::main->PopItemDataIndex(GUIInventory::main->selectIndex));

					selectedUI->SetActiveSelf(true);
					selectedUI->_transform->SetWorldPosition(GetOwner()->_transform->GetLocalPosition() + Vector3::Up * 1.75);
					cookType = 1;

					auto process = selectedUI->GetComponent<GUICookProgress>();
					process->type = 0;
					if (GetOwner()->HasTag(GameObjectTag::CookType_Cut))
						process->SetMaxTime(8);
					if (GetOwner()->HasTag(GameObjectTag::CookType_Bake))
					{
						process->SetMaxTime(15);
						process->type = 2;
					}
					if (GetOwner()->HasTag(GameObjectTag::CookType_Wash))
						process->SetMaxTime(5);
					if (GetOwner()->HasTag(GameObjectTag::CookType_Boil))
					{
						process->SetMaxTime(10);
						process->type = 1;
					}
					process->Clear();
					process->Play();
				}
			}
		}
	}
	if (cookType == 1)
	{
		auto process = selectedUI->GetComponent<GUICookProgress>();


		if (process->IsSuccessed())
			cookType = 2;
	}
	if (cookType == 2 && onTrigger)
	{
		if (Input::main->GetKeyDown(KeyCode::F))
		{
			cookType = 0;
			auto process = selectedUI->GetComponent<GUICookProgress>();
			process->Clear();
			selectedUI->SetActiveSelf(false);
			selectedUI = nullptr;

			if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
			{
				target->SetActiveSelf(true);
				target->_transform->SetWorldPosition(GetOwner()->_transform->GetLocalPosition() + Vector3::Up * 1.65);
			}

			auto itemData = PopItemData();
			ItemData nextItemData = itemData;
			vector<ItemData> resultItems;


			bool isFish = (itemData.itemCode == 3 || itemData.itemCode == 4);
			bool isMeat = (itemData.itemCode == 0 || itemData.itemCode == 1 || itemData.itemCode == 2);
			bool isNonCook = (itemData.itemCookType == 1 || itemData.itemCookType == -1);

			if (GetOwner()->HasTag(GameObjectTag::CookType_Cut))
			{
				nextItemData.itemCookType = 2;

				if ((isFish) && // 고기
					isNonCook) {// 씻거나, 기본
					nextItemData.itemCode = 0;
					nextItemData.itemCookType = -1;

					if (itemData.itemCode == 3)
						nextItemData.itemCode = 0;
					if (itemData.itemCode == 4)
						nextItemData.itemCode = 1;
					for (int i = 0; i < 2; i++)
						resultItems.push_back(nextItemData);
				}
				else
					resultItems.push_back(nextItemData);
			}

			if (GetOwner()->HasTag(GameObjectTag::CookType_Bake))
			{

				nextItemData.itemCookType = 0;
				if (itemData.itemCookType == 3) // 실패
					nextItemData.itemCookType = 2;
				resultItems.push_back(nextItemData);
			}
			if (GetOwner()->HasTag(GameObjectTag::CookType_Wash))
			{
				nextItemData.itemCookType = 1;
				resultItems.push_back(nextItemData);
			}
			if (GetOwner()->HasTag(GameObjectTag::CookType_Boil))
			{
				nextItemData.itemCookType = 3;
				if (itemData.itemCookType == 1)  // 실패
					nextItemData.itemCode = 2;

				if (itemData.itemCode == 7 && itemData.itemCookType == 1) {
					nextItemData.itemCode = 8; nextItemData.itemCookType = -1;
				}
				resultItems.push_back(nextItemData);
			}
		


			for (auto& item : resultItems)
			{
				if (GUIInventory::main->HasEmptySlot())
				{
					GUIInventory::main->PushItemData(item);
				}
				else if (GUIItemBox::main->HasEmptySlot())
				{
					GUIItemBox::main->PushItemData(item);
				}
			}
		}
	}
}

void CookObject::Update2()
{
	Component::Update2();
}

void CookObject::Enable()
{
	Component::Enable();
}

void CookObject::Disable()
{
	Component::Disable();
}

void CookObject::RenderBegin()
{
	Component::RenderBegin();
}

void CookObject::RenderEnd()
{
	Component::RenderEnd();
}

void CookObject::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				if (cookType == 0)
				{
					std::vector<std::shared_ptr<GameObject>> nameList;
					if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
					{
						target->SetActiveSelf(true);
						target->_transform->SetWorldPosition(GetOwner()->_transform->GetLocalPosition() + Vector3::Up * 1.65);
					}
				}
				onTrigger = true;
			}
		}
	}
}

void CookObject::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)

{
	Component::CollisionEnd(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				if (cookType == 0)
				{
					std::vector<std::shared_ptr<GameObject>> nameList;
					if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
					{
						target->SetActiveSelf(false);
					}
				}

				onTrigger = false;
			}
		}
	}
}

void CookObject::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void CookObject::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void CookObject::SetDestroy()
{
	Component::SetDestroy();
}

void CookObject::Destroy()
{
	Component::Destroy();
}
