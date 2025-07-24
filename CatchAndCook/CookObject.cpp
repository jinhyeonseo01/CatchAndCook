#include "pch.h"
#include "CookObject.h"
#include "Collider.h"
#include "GUIInventory.h"
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
		if (Input::main->GetKeyDown(KeyCode::E))
		{
			std::vector<shared_ptr<GameObject>> cookUIs;
			GetOwner()->GetScene()->Finds(L"CookUI_Cooking", cookUIs);
			if (cookUIs.size() != 0)
			{
				for (auto& ui : cookUIs)
					if (ui->GetActiveSelf() == false)
						selectedUI = ui;
				if (selectedUI && GUIInventory::main && GUIInventory::main->GetItemDataIndex(GUIInventory::main->selectIndex).itemCode != -1
					&& !HasOwnItem())
				{
					if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
						target->SetActiveSelf(false);

					PushItemData(GUIInventory::main->PopItemDataIndex(GUIInventory::main->selectIndex));

					selectedUI->SetActiveSelf(true);
					selectedUI->_transform->SetWorldPosition(GetOwner()->_transform->GetLocalPosition() + Vector3::Up * 1.75);
					cookType = 1;
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
	if (cookType == 0 && collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				std::vector<std::shared_ptr<GameObject>> nameList;
				if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
				{
					target->SetActiveSelf(true);
					target->_transform->SetWorldPosition(GetOwner()->_transform->GetLocalPosition() + Vector3::Up * 1.65);
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
				onTrigger = false;
				std::vector<std::shared_ptr<GameObject>> nameList;
				if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
				{
					target->SetActiveSelf(false);
				}
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
