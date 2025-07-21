#include "pch.h"
#include "CookItemBoxObject.h"

#include "Collider.h"
#include "ItemBox.h"
#include "PhysicsComponent.h"
#include "Transform.h"


COMPONENT(CookItemBoxObject)

CookItemBoxObject::~CookItemBoxObject()
{
}

bool CookItemBoxObject::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void CookItemBoxObject::Init()
{
	Component::Init();
}

void CookItemBoxObject::Start()
{
	Component::Start();
}

void CookItemBoxObject::Update()
{
	Component::Update();

	if (onTrigger)
	{
		if (Input::main->GetKeyDown(KeyCode::E))
		{
			if (ItemBox::main)
			{
				ItemBox::main->GetOwner()->SetActiveSelf(true);
			}
		}
	}
}

void CookItemBoxObject::Update2()
{
	Component::Update2();
}

void CookItemBoxObject::Enable()
{
	Component::Enable();
}

void CookItemBoxObject::Disable()
{
	Component::Disable();
}

void CookItemBoxObject::RenderBegin()
{
	Component::RenderBegin();
}

void CookItemBoxObject::RenderEnd()
{
	Component::RenderEnd();
}

void CookItemBoxObject::CollisionBegin(const std::shared_ptr<Collider>& collider,
	const std::shared_ptr<Collider>& other)
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

void CookItemBoxObject::CollisionEnd(const std::shared_ptr<Collider>& collider,
	const std::shared_ptr<Collider>& other)
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

void CookItemBoxObject::ChangeParent(const std::shared_ptr<GameObject>& prev,
	const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void CookItemBoxObject::ChangeScene(const std::shared_ptr<Scene>& currentScene,
	const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void CookItemBoxObject::SetDestroy()
{
	Component::SetDestroy();
}

void CookItemBoxObject::Destroy()
{
	Component::Destroy();
}
