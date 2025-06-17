#include "pch.h"
#include "BoatController.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Transform.h"
#include "ColliderManager.h"
COMPONENT(BoatController)

float BoatController::heightOffset=15.0f;
float BoatController::SpringArmLength=30.0f;

BoatController::~BoatController()
{
}

bool BoatController::IsExecuteAble()
{
	return true;
}

void BoatController::Init()
{
	_camera = CameraManager::main->GetCamera(CameraType::BoatCamera);

}

void BoatController::Start()
{
}

void BoatController::Update()
{
	if (_onBoard == false)
		return;

	float dt =Time::main->GetDeltaTimeNow();
	 Quaternion quat =  CalCulateYawPitchRoll();

	 GetOwner()->_transform->SetLocalRotation(quat);

	 if (Input::main->GetKey(KeyCode::W))
	 {

		 vec3 pos = GetOwner()->_transform->GetLocalPosition();
		 GetOwner()->_transform->SetLocalPosition(pos + GetOwner()->_transform->GetForward() * dt * 3000.0f);
	 }

	 auto ray = ColliderManager::main->RayCastForMyCell({ GetOwner()->_transform->GetLocalPosition(), GetOwner()->_transform->GetForward()}, 1.0f, GetOwner());

	 if (ray.isHit)
	 {
		 vec3 normal = ray.normal;

		 float penetrationBuffer = 0.05f;
		 GetOwner()->_transform->SetLocalPosition(GetOwner()->_transform->GetLocalPosition()+ normal * penetrationBuffer);
	 }


	_camera->SetCameraPos(GetOwner()->_transform->GetWorldPosition() - GetOwner()->_transform->GetForward()* SpringArmLength + vec3(0,heightOffset,0));
	_camera->SetCameraRotation(quat);
	CameraManager::main->Setting();

}

void BoatController::Update2()
{
	if (_onBoard == false)
		return;
}

void BoatController::Enable()
{
}

void BoatController::Disable()
{
}

void BoatController::RenderBegin()
{
}

void BoatController::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void BoatController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void BoatController::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void BoatController::SetDestroy()
{
}

void BoatController::Destroy()
{
}

void BoatController::SetOnBaord()
{
	_onBoard = true;

	CameraManager::main->SetActiveCamera(CameraType::BoatCamera);

}



Quaternion BoatController::CalCulateYawPitchRoll()
{
	if (Input::main->IsMouseLock() == false)
	{
		static vec2 lastMousePos = Input::main->GetMousePosition();

		if (Input::main->GetKeyDown(KeyCode::BackQoute))
		{
			lastMousePos = Input::main->GetMousePosition();
		}

		vec2 currentMousePos = Input::main->GetMousePosition();

		vec2 delta = (currentMousePos - lastMousePos) * 0.1f;

		_yaw += delta.x;
		_roll = 0;

		lastMousePos = currentMousePos;
	}
	else
	{
		vec2 currentMousePos = Input::main->GetMousePosition();
		vec2 centerPos = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		vec2 delta = (currentMousePos - centerPos) * 0.1f;

		_yaw += delta.x;
		_roll = 0;

		POINT center;
		center.x = static_cast<LONG>(WINDOW_WIDTH / 2);
		center.y = static_cast<LONG>(WINDOW_HEIGHT / 2);
		ClientToScreen(Core::main->GetHandle(), &center);
		SetCursorPos(center.x, center.y);
	}

	return Quaternion::CreateFromYawPitchRoll(_yaw * D2R, 0 , 0);
}