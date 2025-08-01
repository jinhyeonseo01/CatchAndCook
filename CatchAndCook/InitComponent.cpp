#include "pch.h"
#include "InitComponent.h"

#include "IComponentFactory.h"

InitComponent::~InitComponent()
{
}

bool InitComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void InitComponent::Init()
{
	Component::Init();
	SetDestroy();
}

void InitComponent::Start()
{
	Component::Start();
}

void InitComponent::Update()
{
	Component::Update();
}

void InitComponent::Update2()
{
	Component::Update2();
}

void InitComponent::Enable()
{
	Component::Enable();
}

void InitComponent::Disable()
{
	Component::Disable();
}

void InitComponent::RenderBegin()
{
	Component::RenderBegin();
}

void InitComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void InitComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void InitComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void InitComponent::SetDestroy()
{
	Component::SetDestroy();
}

void InitComponent::Destroy()
{
	Component::Destroy();
}




//--------------------------------


void TagsComponent::Init()
{
	Component::Init();
	for (auto& tag : _tagNames)
	{
		if (GameObjectTagMap.contains(tag))
			GetOwner()->AddTag(GameObjectTagMap[tag]);
	}
}

void TagsComponent::Start()
{
	Component::Start();
}

void ScriptsComponent::Init()
{
	InitComponent::Init();
	for (auto& script : _scriptNames)
	{
		auto component = IComponentFactory::Create(script);
		GetOwner()->AddComponent(component);
	}
}
