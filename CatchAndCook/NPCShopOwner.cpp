#include "pch.h"
#include "NPCShopOwner.h"

#include "Collider.h"
#include "PhysicsComponent.h"
#include "Transform.h"

COMPONENT(NPCShopOwner)

NPCShopOwner::~NPCShopOwner()
{

}

bool NPCShopOwner::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void NPCShopOwner::Init()
{
	Component::Init();
}

void NPCShopOwner::Start()
{
	Component::Start();
}

void NPCShopOwner::Update()
{
	Component::Update();

	if (onTrigger && Input::main->GetKeyDown(KeyCode::F))
	{
		GetOwner()->GetScene()->Find(L"GUIShop")->SetActiveSelf(true);
		Sound::main->PlayImmediate("j_button_swap", 0.1f);
		Input::main->SetMouseLock(false);
	}
}

void NPCShopOwner::Update2()
{
	Component::Update2();
}

void NPCShopOwner::Enable()
{
	Component::Enable();
}

void NPCShopOwner::Disable()
{
	Component::Disable();
}

void NPCShopOwner::RenderBegin()
{
	Component::RenderBegin();
}

void NPCShopOwner::RenderEnd()
{
	Component::RenderEnd();
}

void NPCShopOwner::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				if (auto ui = GetOwner()->GetScene()->Find(L"NPCShopOwnerTriggerUI"))
				{
					ui->_transform->SetWorldPosition(GetOwner()->_transform->GetWorldPosition() + Vector3::Up * 2.05);
					ui->SetActiveSelf(true);
					onTrigger = true;
				}
			}
		}
	}
}

void NPCShopOwner::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);

	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				auto ui = GetOwner()->GetScene()->Find(L"NPCShopOwnerTriggerUI");
				if (ui)
				{
					ui->SetActiveSelf(false);
					onTrigger = false;
				}
			}
		}
	}
}

void NPCShopOwner::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void NPCShopOwner::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void NPCShopOwner::SetDestroy()
{
	Component::SetDestroy();
}

void NPCShopOwner::Destroy()
{
	Component::Destroy();
}

void NPCShopOwner::Reset()
{
	Component::Reset();
}
