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
	/*GetOwner()->_transform->SetForward(CameraManager::main->GetActiveCamera()->GetCameraLook() * -1.0f);*/

	if (_percentagePtr == nullptr)
	{
		cout << "_percentagePtr NO Binding" << endl;
		return;
	}

	if (_displayPercentage > *_percentagePtr)
	{
		
		float diff = _displayPercentage - *_percentagePtr;
		float speed = diff * 3.0f; 
		_displayPercentage -= speed * Time::main->GetDeltaTime();

		if (_displayPercentage < *_percentagePtr)
		{
			_displayPercentage = *_percentagePtr;
		}
	}
}

void PercentComponent::Update2()
{

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
	if (_percentagePtr == nullptr)
	{
		return;
	}

	buffer->AddData(_displayPercentage);
}
