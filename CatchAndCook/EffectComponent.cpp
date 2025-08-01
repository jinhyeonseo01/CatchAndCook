#include "pch.h"
#include "EffectComponent.h"

#include "ObjectSettingComponent.h"

EffectComponent::~EffectComponent()
{
}

bool EffectComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void EffectComponent::Init()
{
	Component::Init();
}

void EffectComponent::Start()
{
	Component::Start();
	_objectSettingComponent = GetOwner()->GetComponentWithParents<ObjectSettingComponent>();
	if (!_objectSettingComponent.lock())
		_objectSettingComponent = GetOwner()->AddComponent<ObjectSettingComponent>();

}

void EffectComponent::Update()
{
	Component::Update();
	if (auto objectSettingComponent = _objectSettingComponent.lock())
	{
		
	}
}

void EffectComponent::Update2()
{
	Component::Update2();
}

void EffectComponent::Enable()
{
	Component::Enable();
}

void EffectComponent::Disable()
{
	Component::Disable();
}

void EffectComponent::RenderBegin()
{
	Component::RenderBegin();
}

void EffectComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void EffectComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void EffectComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void EffectComponent::SetDestroy()
{
	Component::SetDestroy();
}

void EffectComponent::Destroy()
{
	Component::Destroy();
}
