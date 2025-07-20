#include "pch.h"
#include "FoodShopObject.h"

#include "Collider.h"
#include "PhysicsComponent.h"
#include "Transform.h"

COMPONENT(FoodShopObject)

FoodShopObject::~FoodShopObject()
{
}

bool FoodShopObject::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void FoodShopObject::Init()
{
	Component::Init();
}

void FoodShopObject::Start()
{
	Component::Start();
}

void FoodShopObject::Update()
{
	Component::Update();
}

void FoodShopObject::Update2()
{
	Component::Update2();
}

void FoodShopObject::Enable()
{
	Component::Enable();
}

void FoodShopObject::Disable()
{
	Component::Disable();
}

void FoodShopObject::RenderBegin()
{
	Component::RenderBegin();
}

void FoodShopObject::RenderEnd()
{
	Component::RenderEnd();
}

void FoodShopObject::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			std::vector<std::shared_ptr<GameObject>> nameList;
			if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
			{
				target->SetActiveSelf(true);
				target->_transform->SetWorldPosition(GetOwner()->_transform->GetLocalPosition() + Vector3::Up * 1.65);
			}
		}
	}
}

void FoodShopObject::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)

{
	Component::CollisionEnd(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			std::vector<std::shared_ptr<GameObject>> nameList;
			if (auto target = GetOwner()->GetScene()->Find(L"CookUI_BeginText"))
			{
				target->SetActiveSelf(false);
			}
		}
	}
}

void FoodShopObject::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void FoodShopObject::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void FoodShopObject::SetDestroy()
{
	Component::SetDestroy();
}

void FoodShopObject::Destroy()
{
	Component::Destroy();
}
