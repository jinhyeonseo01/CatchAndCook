#include "pch.h"
#include "BilboardComponent.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Transform.h"

COMPONENT(BilboardComponent)

void BilboardComponent::Init()
{
}

void BilboardComponent::Start()
{
}

void BilboardComponent::Update()
{
	vec3 objPos = GetOwner()->_transform->GetWorldPosition();
	vec3 camPos = CameraManager::main->GetActiveCamera()->GetCameraPos();

	vec3 dirToCamera = (camPos - objPos);
	dirToCamera.Normalize();

	GetOwner()->_transform->LookUp(dirToCamera, vec3::Up);
}

void BilboardComponent::Update2()
{
}

void BilboardComponent::Enable()
{
}

void BilboardComponent::Disable()
{
}

void BilboardComponent::RenderBegin()
{
}

void BilboardComponent::RenderEnd()
{
}

void BilboardComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void BilboardComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void BilboardComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void BilboardComponent::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
}

void BilboardComponent::SetDestroy()
{
}

void BilboardComponent::Destroy()
{
}
