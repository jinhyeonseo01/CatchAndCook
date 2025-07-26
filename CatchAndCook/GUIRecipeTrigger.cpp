#include "pch.h"
#include "GUIRecipeTrigger.h"

#include "Collider.h"
#include "PhysicsComponent.h"
#include "Transform.h"


COMPONENT(GUIRecipeTrigger)

GUIRecipeTrigger::~GUIRecipeTrigger()
{
}

bool GUIRecipeTrigger::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIRecipeTrigger::Init()
{
	Component::Init();
}

void GUIRecipeTrigger::Start()
{
	Component::Start();
}

void GUIRecipeTrigger::Update()
{
	Component::Update();

	if (onTrigger && Input::main->GetKeyDown(KeyCode::F))
	{
		GetOwner()->GetScene()->Find(L"GUIRecipe")->SetActiveSelf(true);
		Input::main->SetMouseLock(false);
	}
}

void GUIRecipeTrigger::Update2()
{
	Component::Update2();
}

void GUIRecipeTrigger::Enable()
{
	Component::Enable();
}

void GUIRecipeTrigger::Disable()
{
	Component::Disable();
}

void GUIRecipeTrigger::RenderBegin()
{
	Component::RenderBegin();
}

void GUIRecipeTrigger::RenderEnd()
{
	Component::RenderEnd();
}

void GUIRecipeTrigger::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				auto ui = GetOwner()->GetScene()->Find(L"RecipeTriggerUI");
				if (ui)
				{
					ui->_transform->SetWorldPosition(GetOwner()->_transform->GetWorldPosition() + Vector3::Up * 1.1);
					ui->SetActiveSelf(true);
					onTrigger = true;
				}
			}
		}
	}
}

void GUIRecipeTrigger::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				auto ui = GetOwner()->GetScene()->Find(L"RecipeTriggerUI");
				if (ui)
				{
					ui->SetActiveSelf(false);
					onTrigger = false;
				}
			}
		}
	}
}

void GUIRecipeTrigger::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIRecipeTrigger::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIRecipeTrigger::SetDestroy()
{
	Component::SetDestroy();
}

void GUIRecipeTrigger::Destroy()
{
	Component::Destroy();
}

void GUIRecipeTrigger::Reset()
{
	Component::Reset();
}
