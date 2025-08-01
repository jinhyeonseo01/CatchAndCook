#include "pch.h"
#include "PhysicsComponent.h"
#include "Collider.h"

PhysicsComponent::~PhysicsComponent()
{
}

bool PhysicsComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void PhysicsComponent::Init()
{
	Component::Init();
}

void PhysicsComponent::Start()
{
	Component::Start();
}

void PhysicsComponent::Update()
{
	Component::Update();
}

void PhysicsComponent::Update2()
{
	Component::Update2();
}

void PhysicsComponent::Enable()
{
	Component::Enable();
}

void PhysicsComponent::Disable()
{
	Component::Disable();
}

void PhysicsComponent::RenderBegin()
{
	Component::RenderBegin();
}

void PhysicsComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void PhysicsComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void PhysicsComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void PhysicsComponent::SetDestroy()
{
	Component::SetDestroy();
}

void PhysicsComponent::Destroy()
{
	Component::Destroy();
}

int PhysicsComponent::GetPhysicsGroupID(const std::shared_ptr<GameObject>& obj)
{
	auto groupId = obj->GetInstanceID();
	if (auto obj2 = obj->GetComponentWithParents<PhysicsComponent>())
		groupId = obj2->GetOwner()->GetInstanceID();
	return groupId;
}

