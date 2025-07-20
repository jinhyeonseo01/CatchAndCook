#include "pch.h"
#include "GUIItem.h"


COMPONENT(GUIItem)

GUIItem::~GUIItem()
{
}

bool GUIItem::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIItem::Init()
{
	Component::Init();
}

void GUIItem::Start()
{
	Component::Start();
	if (auto icons = GetOwner()->GetChildByName(L"ItemIcon"))
	{
		auto items = icons->GetChildByName(L"Items");
		int i = 0;
		while (true)
		{
			auto name = to_wstring(i);
			auto item = items->GetChildByName(name);
			if (item)
				_itemIcons.push_back(item);
			else
				break;
			++i;
		}

		auto effects = icons->GetChildByName(L"Subs");
		i = 0;
		while (true)
		{
			auto name = to_wstring(i);
			auto item = effects->GetChildByName(name);
			if (item)
				_effectIcons.push_back(item);
			else
				break;
			++i;
		}
	}
}

void GUIItem::Update()
{
	Component::Update();
	for (int i = 0;i< _itemIcons.size(); i++)
	{
		if (_itemData.itemCode == i)
			_itemIcons[i]->SetActiveSelf(true);
		else
			_itemIcons[i]->SetActiveSelf(false);
	}

	for (int i = 0; i < _effectIcons.size(); i++)
	{
		if ((_itemData.itemCode != -1) && (_itemData.itemCookType == i))
			_effectIcons[i]->SetActiveSelf(true);
		else
			_effectIcons[i]->SetActiveSelf(false);
	}
}

void GUIItem::Update2()
{
	Component::Update2();
}

void GUIItem::Enable()
{
	Component::Enable();
}

void GUIItem::Disable()
{
	Component::Disable();
}

void GUIItem::RenderBegin()
{
	Component::RenderBegin();
}

void GUIItem::RenderEnd()
{
	Component::RenderEnd();
}

void GUIItem::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void GUIItem::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void GUIItem::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIItem::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIItem::SetDestroy()
{
	Component::SetDestroy();
}

void GUIItem::Destroy()
{
	Component::Destroy();
}
