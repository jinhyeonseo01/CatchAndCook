#include "pch.h"
#include "InGameMainField.h"

#include "Game.h"


COMPONENT(InGameMainField)

InGameMainField* InGameMainField::main = nullptr;

InGameMainField::~InGameMainField()
{

}

bool InGameMainField::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void InGameMainField::Init()
{
	Component::Init();
	main = this;
}

void InGameMainField::Start()
{
	Component::Start();


	std::vector<std::shared_ptr<GameObject>> pathPointObjects;
	SceneManager::main->GetCurrentScene()->FindsInclude(L"$PathPoint", pathPointObjects);

	std::vector<std::shared_ptr<GameObject>> pathPointObjectTables;
	std::ranges::copy_if(pathPointObjects, std::back_inserter(pathPointObjectTables), [&](const std::shared_ptr<GameObject>& current) {
		return (current->GetName().find(L"Shop") != std::wstring::npos) &&
			(current->GetName().find(L"Sit") != std::wstring::npos);
		});
	for (auto point : pathPointObjectTables)
	{
		shopTablePointsPool.push_back(point);
	}
}

void InGameMainField::Update()
{
	Component::Update();

	if (Input::main->GetKeyDown(KeyCode::Num8))
	{
		shopOpen = true;
	}
	if (Input::main->GetKeyDown(KeyCode::Num7))
	{
		shopOpen = false;
	}
	float normTime = InGameGlobal::main->skyTime;

	while (normTime >= 4)
		normTime -= 4;
	if (normTime >= 0.9999 && normTime < 2)
	{
		InGameGlobal::main->skyTime += Time::main->GetDeltaTime() * (1 / 180.0f);
		InGameMainField::GetMain()->shopOpen = true;
	}
	else
		InGameMainField::GetMain()->shopOpen = false;

	if (normTime >= 3 && normTime < 4)
	{
		InGameGlobal::main->skyTime += Time::main->GetDeltaTime() * (1 / 180.0f);
	}
}

void InGameMainField::Update2()
{
	Component::Update2();
}

void InGameMainField::Enable()
{
	Component::Enable();
}

void InGameMainField::Disable()
{
	Component::Disable();
}

void InGameMainField::RenderBegin()
{
	Component::RenderBegin();
}

void InGameMainField::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void InGameMainField::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void InGameMainField::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void InGameMainField::SetDestroy()
{
	Component::SetDestroy();
}

void InGameMainField::Destroy()
{
	Component::Destroy();
	main = nullptr;
}

void InGameMainField::AddObjectSetting(const std::shared_ptr<ObjectSettingComponent>& object_setting_component)
{
	objectSettings.push_back(object_setting_component);
}

void InGameMainField::RemoveObjectSetting(const std::shared_ptr<ObjectSettingComponent>& object_setting_component)
{
	auto it = std::ranges::find_if(objectSettings, [&](const std::weak_ptr<ObjectSettingComponent>& obj) {
		if (auto comp = obj.lock())
			if(comp == object_setting_component)
				return true;
		return false;
	});
	if (it != objectSettings.end())
		objectSettings.erase(it);
}

void InGameMainField::AddNPC(const std::shared_ptr<NPCComponent>& ele)
{
	npcList.push_back(ele);
}

void InGameMainField::RemoveNPC(const std::shared_ptr<NPCComponent>& ele)
{
	auto& list = npcList;
	auto it = std::ranges::find_if(list, [&](const std::weak_ptr<NPCComponent>& obj)
		{
			if (auto obj2 = obj.lock())
				if (auto comp = ele)
					return true;
			return false;
		});
	if (it != list.end())
		list.erase(it);
}
