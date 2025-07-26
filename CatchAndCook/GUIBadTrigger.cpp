#include "pch.h"
#include "GUIBadTrigger.h"

#include "Collider.h"
#include "PhysicsComponent.h"
#include "Transform.h"

COMPONENT(GUIBadTrigger)

GUIBadTrigger::~GUIBadTrigger()
{
}

bool GUIBadTrigger::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void GUIBadTrigger::Init()
{
	Component::Init();
}

void GUIBadTrigger::Start()
{
	Component::Start();
}

void GUIBadTrigger::Update()
{
	Component::Update();

	if (onTrigger && Input::main->GetKeyDown(KeyCode::F))
	{
		GetOwner()->GetScene()->Find(L"GUIBad")->SetActiveSelf(true);
		Input::main->SetMouseLock(false);
		std::cout << "1234\n";
	}
}

void GUIBadTrigger::Update2()
{
	Component::Update2();
}

void GUIBadTrigger::Enable()
{
	Component::Enable();
}

void GUIBadTrigger::Disable()
{
	Component::Disable();
}

void GUIBadTrigger::RenderBegin()
{
	Component::RenderBegin();
}

void GUIBadTrigger::RenderEnd()
{
	Component::RenderEnd();
}

void GUIBadTrigger::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				auto ui = GetOwner()->GetScene()->Find(L"GUIBadTriggerUI");
				if (ui)
				{
					ui->_transform->SetWorldPosition(GetOwner()->_transform->GetWorldPosition() + Vector3::Up * 1.35);
					ui->SetActiveSelf(true);
					onTrigger = true;
				}
			}
		}
	}
}

void GUIBadTrigger::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
	if (collider->IsTrigger())
	{
		if (auto physics = other->GetOwner()->GetComponentWithParents<PhysicsComponent>())
		{
			if (physics->GetOwner()->HasTag(GameObjectTag::Player))
			{
				auto ui = GetOwner()->GetScene()->Find(L"GUIBadTriggerUI");
				if (ui)
				{
					ui->SetActiveSelf(false);
					onTrigger = false;
				}
			}
		}
	}
}

void GUIBadTrigger::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void GUIBadTrigger::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void GUIBadTrigger::SetDestroy()
{
	Component::SetDestroy();
}

void GUIBadTrigger::Destroy()
{
	Component::Destroy();
}

void GUIBadTrigger::Reset()
{
	Component::Reset();
}
