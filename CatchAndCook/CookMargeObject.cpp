#include "pch.h"
#include "CookMargeObject.h"
#include "Collider.h"
#include "GUICookProgress.h"
#include "GUIInventory.h"
#include "GUIItemBox.h"
#include "Transform.h"
#include "PhysicsComponent.h"

COMPONENT(CookMargeObject)

bool CookMargeObject::HasOwnSlot()
{
	for (int i = 0; i < slotCount; i++)
		if (itemDatas[i].itemCode != -1)
			return true;
	return false;
}

bool CookMargeObject::HasEmptySlot()
{
	for (int i = 0; i < slotCount; i++)
		if (itemDatas[i].itemCode == -1)
			return true;
	return false;
}

int CookMargeObject::GetEmptySlotIndex()
{
	for (int i = 0; i < slotCount; i++)
		if (itemDatas[i].itemCode == -1)
			return i;
	return -1;
}

bool CookMargeObject::PushItemData(const ItemData& itemData)
{
	for (int i = 0; i < slotCount; i++)
	{
		if (itemDatas[i].itemCode == -1)
		{
			itemDatas[i] = itemData;
			return true;
		}
	}
	return false;
}

bool CookMargeObject::SetItemData(const ItemData& itemData, int index)
{
	itemDatas[index] = itemData;
	return true;
}

ItemData CookMargeObject::GetItemDataIndex(int index)
{
	return itemDatas[index];
}

ItemData CookMargeObject::PopItemDataIndex(int index)
{
	auto temp = itemDatas[index];
	if (index < 0 || index >= slotCount)
		return ItemData();
	itemDatas[index].Clear();
	return temp;
}

void CookMargeObject::SlotUpdate()
{
	for (int i = 0; i < _slots.size(); i++)
	{
		if (auto item = _slots[i]->GetComponent<GUIItem>())
		{
			item->PopItemData();
			if (i < itemDatas.size())
				item->PushItemData(itemDatas[i]);
		}
	}
}


CookMargeObject::~CookMargeObject()
{
}

bool CookMargeObject::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void CookMargeObject::Init()
{
	Component::Init();
}

void CookMargeObject::Start()
{
	Component::Start();

	_CookUI_CookMarge = GetOwner()->GetScene()->Find(L"CookUI_CookMarge");

	for (int i = 0; i < slotCount; i++)
	{
		wstring name = L"Slot_" + to_wstring(i);
		_slots.push_back(_CookUI_CookMarge->GetChildByName(name));
	}
}

void CookMargeObject::Update()
{
	Component::Update();

	SlotUpdate();

	if (onTrigger)
	{
		if (cookType == 0)
		{
			if (Input::main->GetKeyDown(KeyCode::F))
			{
				if (GUIInventory::main)
				{
					if (GUIInventory::main->GetItemDataIndex(GUIInventory::main->selectIndex).itemCode != -1 && HasEmptySlot())
					{
						PushItemData(GUIInventory::main->PopItemDataIndex(GUIInventory::main->selectIndex));
					}
					else if (GUIInventory::main->GetItemDataIndex(GUIInventory::main->selectIndex).itemCode == -1 && HasOwnSlot())
					{
						cookType = 1;

						std::vector<shared_ptr<GameObject>> cookUIs;
						GetOwner()->GetScene()->Finds(L"CookUI_Cooking", cookUIs);
						if (cookUIs.size() != 0)
						{
							for (auto& ui : cookUIs)
								if (ui->GetActiveSelf() == false)
									selectedUI = ui;
							if (selectedUI)
							{
								if (auto target = _CookUI_CookMarge)
									target->SetActiveSelf(false);

								selectedUI->SetActiveSelf(true);
								selectedUI->_transform->SetWorldPosition(GetOwner()->_transform->GetLocalPosition() + Vector3::Up * 1.75);

								auto process = selectedUI->GetComponent<GUICookProgress>();
								process->type = 0;
								process->SetMaxTime(4);
								process->Clear();
								process->Play();
							}
						}
					}
				}
			}
			if (GUIInventory::main)
			{
				auto CookUI_CookMarge = _CookUI_CookMarge;
				if (GUIInventory::main->GetItemDataIndex(GUIInventory::main->selectIndex).itemCode != -1)
				{
					CookUI_CookMarge->GetChildByName(L"Text")->SetActiveSelf(true);
					CookUI_CookMarge->GetChildByName(L"Text_End")->SetActiveSelf(false);
				}
				else
				{
					CookUI_CookMarge->GetChildByName(L"Text")->SetActiveSelf(false);
					CookUI_CookMarge->GetChildByName(L"Text_End")->SetActiveSelf(true);
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

				if (auto target = _CookUI_CookMarge)
				{
					target->SetActiveSelf(true);
					target->_transform->SetWorldPosition(GetOwner()->_transform->GetLocalPosition() + Vector3::Up * 1.65);
				}

				bool found = false;
				for (auto cookRes : InGameGlobal::main->cookTable)
				{
					found = ranges::all_of(
						cookRes.second,
						[&](const ItemData& item) {
							// itemDatas 안에 item 이 있으면 true
							return std::find(itemDatas.begin(), itemDatas.end(), item)
								!= itemDatas.end();
						});
					if (found)
					{
						if (GUIInventory::main->HasEmptySlot())
						{
							GUIInventory::main->PushItemData(cookRes.first);
						}
						else if (GUIItemBox::main->HasEmptySlot())
						{
							GUIItemBox::main->PushItemData(cookRes.first);
						}
						break;
					}
				}
				if (!found)
				{
					if (GUIInventory::main->HasEmptySlot())
					{
						GUIInventory::main->PushItemData(ItemData{ 9,2 });
					}
					else if (GUIItemBox::main->HasEmptySlot())
					{
						GUIItemBox::main->PushItemData(ItemData{ 9,2 });
					}
				}

				for (auto& item : itemDatas)
					item.Clear();
			}
		}
	}
}

void CookMargeObject::Update2()
{
	Component::Update2();
}

void CookMargeObject::Enable()
{
	Component::Enable();
}

void CookMargeObject::Disable()
{
	Component::Disable();
}

void CookMargeObject::RenderBegin()
{
	Component::RenderBegin();
}

void CookMargeObject::RenderEnd()
{
	Component::RenderEnd();
}


void CookMargeObject::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				onTrigger = true;
			}
		}
	}
}

void CookMargeObject::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)

{
	Component::CollisionEnd(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				onTrigger = false;
			}
		}
	}
}

void CookMargeObject::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void CookMargeObject::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void CookMargeObject::SetDestroy()
{
	Component::SetDestroy();
}

void CookMargeObject::Destroy()
{
	Component::Destroy();
}
