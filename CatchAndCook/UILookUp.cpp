#include "pch.h"
#include "UILookUp.h"

#include "Camera.h"
#include "CameraManager.h"
#include "Transform.h"

COMPONENT(UILookUp)

UILookUp::~UILookUp()
{

}

bool UILookUp::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void UILookUp::Init()
{
	Component::Init();
}

void UILookUp::Start()
{
	Component::Start();
}

void UILookUp::Update()
{
	Component::Update();
	Vector3 look = GetOwner()->_transform->GetWorldPosition() - CameraManager::main->GetActiveCamera()->GetCameraPos();
	look.y = 0;
	look.Normalize();

	GetOwner()->_transform->LookUp(look, Vector3::Up);
}

void UILookUp::Update2()
{
	Component::Update2();
}

void UILookUp::Enable()
{
	Component::Enable();
}

void UILookUp::Disable()
{
	Component::Disable();
}

void UILookUp::RenderBegin()
{
	Component::RenderBegin();
}

void UILookUp::RenderEnd()
{
	Component::RenderEnd();
}

void UILookUp::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void UILookUp::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void UILookUp::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void UILookUp::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Component::ChangeScene(currentScene, nextScene);
}

void UILookUp::SetDestroy()
{
	Component::SetDestroy();
}

void UILookUp::Destroy()
{
	Component::Destroy();
}
