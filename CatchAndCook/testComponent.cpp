#include "pch.h"
#include "testComponent.h"
#include "GameObject.h"
#include <random>
#include "GameObject.h"
#include "Transform.h"
#include "CameraManager.h"
#include "Camera.h"
#include "ColliderManager.h"
std::random_device dre;
std::mt19937 gen(dre());
std::uniform_real_distribution<float> uid(0.0f, 1.0f);


testComponent::~testComponent()
{

}

void testComponent::Init()
{


}

void testComponent::Start()
{

}

void testComponent::Update()
{
	const float speed = 50.0f;
	const float dt =Time::main->GetDeltaTime() *speed;

	std::shared_ptr<Transform> transform =GetOwner()->_transform;

	auto& camera= CameraManager::main->GetCamera(CameraType::DebugCamera);

	if(transform==nullptr)
		return;

	if(Input::main->GetKey(KeyCode::UpArrow))
	{

		auto direction = camera->GetCameraLook();
		auto& pos =transform->GetLocalPosition();
		transform->SetLocalPosition(pos + direction * dt);
	
	}

	if(Input::main->GetKey(KeyCode::DownArrow))
	{

		auto direction = camera->GetCameraLook();
		auto& pos =transform->GetLocalPosition();
		transform->SetLocalPosition(pos - direction * dt);

	}

	if (Input::main->GetKey(KeyCode::RightArrow))
	{
		auto direction = camera->GetCameraRight();
		auto& pos = transform->GetLocalPosition();
		transform->SetLocalPosition(pos + direction * dt);
	}

	if (Input::main->GetKey(KeyCode::LeftArrow))
	{
		auto direction = camera->GetCameraRight();
		auto& pos = transform->GetLocalPosition();
		transform->SetLocalPosition(pos - direction * dt);
	}

	if (Input::main->GetKey(KeyCode::O))
	{
		static float i = 0;;
		transform->SetLocalRotation(vec3(i * D2R, 0, 0));
		i+=0.1f;
	}

	if (Input::main->GetKey(KeyCode::P))
	{
		static float i = 0;;
		transform->SetLocalScale(vec3(i * D2R, i * D2R, i * D2R));
		i += 0.1f;
	}



}

void testComponent::Update2()
{

}

void testComponent::Enable()
{

}

void testComponent::Disable()
{

}

void testComponent::Destroy()
{

}

void testComponent::RenderBegin()
{

}

void testComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	
}

void testComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

}

bool testComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void testComponent::SetDestroy()
{
	IDelayDestroy::SetDestroy();
}

void testComponent::DestroyComponentOnly()
{

}

