#include "pch.h"
#include "PercentComponent.h"
#include "Transform.h"
#include "CameraManager.h"
#include "Camera.h"

COMPONENT(PercentComponent)

void PercentComponent::Init()
{

}

void PercentComponent::Start()
{

	if (GetOwner()->GetRenderer())
	{
		GetOwner()->GetRenderer()->AddStructuredSetter(static_pointer_cast<PercentComponent>(shared_from_this()), BufferType::HPData);
	}

	GetOwner()->SetType(GameObjectType::Dynamic);
}

void PercentComponent::Update()
{

	if (_percentagePtr == nullptr)
	{
		return;
	}

	if (_displayPercentage > *_percentagePtr)
	{
		float diff = _displayPercentage - *_percentagePtr;
		float speed = diff * 4.0f; 
		_displayPercentage -= speed * Time::main->GetDeltaTime();

		if (_displayPercentage < *_percentagePtr)
		{
			_displayPercentage = *_percentagePtr;
		}
	}
}

void PercentComponent::Update2()
{
	vec3 objPos = GetOwner()->_transform->GetWorldPosition();
	vec3 camPos = CameraManager::main->GetActiveCamera()->GetCameraPos();

	vec3 dirToCamera = (camPos - objPos);
	dirToCamera.Normalize();
	GetOwner()->_transform->LookUp(dirToCamera, vec3::Up);

}

void PercentComponent::Enable()
{

}

void PercentComponent::Disable()
{

}

void PercentComponent::RenderBegin()
{

}

void PercentComponent::RenderEnd()
{

}

void PercentComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void PercentComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void PercentComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void PercentComponent::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
}

void PercentComponent::SetDestroy()
{
}

void PercentComponent::Destroy()
{

}

void PercentComponent::SetData(StructuredBuffer* buffer, Material* material)
{
	PercentageData data;
	data.percentage = _displayPercentage;
	
	buffer->AddData(data);
}
