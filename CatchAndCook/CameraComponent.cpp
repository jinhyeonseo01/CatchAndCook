﻿#include "pch.h"
#include "CameraComponent.h"

#include "CameraManager.h"
#include "Transform.h"


COMPONENT(CameraComponent)
CameraComponent::CameraComponent()
{
	_type = CameraType::ComponentCamera;
}


CameraComponent::~CameraComponent()
{
}

bool CameraComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void CameraComponent::Init()
{
	Component::Init();
	auto camera = std::static_pointer_cast<Camera>(GetCast<CameraComponent>());
	_near = 0.1f;
	_far = 2000.f;

	CameraManager::main->AddCamera(_type, camera);
	CameraManager::main->SetActiveCamera(_type);
}

void CameraComponent::Start()
{
	Component::Start();
	Calculate();
}
void CameraComponent::Update()
{
	if (CameraManager::main->GetCameraType() == CameraType::DebugCamera)
	{
		return;
	}

	Component::Update();
	Calculate();
}

void CameraComponent::Update2()
{
	if (CameraManager::main->GetCameraType() == CameraType::DebugCamera)
		return;

	Component::Update2();
	auto trans = GetOwner()->_transform;
	SetCameraPos(trans->GetWorldPosition());
	SetCameraRotation(trans->GetWorldRotation());
}

void CameraComponent::Enable()
{
	Component::Enable();
}

void CameraComponent::Disable()
{
	Component::Disable();
}

void CameraComponent::RenderBegin()
{
	Component::RenderBegin();
}

void CameraComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void CameraComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void CameraComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void CameraComponent::SetDestroy()
{
	Component::SetDestroy();
}

void CameraComponent::Destroy()
{
	Component::Destroy();
	CameraManager::main->RemoveCamera(CameraType::ComponentCamera);
}
