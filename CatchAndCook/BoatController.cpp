#include "pch.h"
#include "BoatController.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Transform.h"
#include "ColliderManager.h"
#include "AnimationListComponent.h"
#include "SkinnedHierarchy.h"
COMPONENT(BoatController)

float BoatController::heightOffset=15.0f;
float BoatController::SpringArmLength=30.0f;
float  BoatController::_yaw=0.0f;
float BoatController::_pitch=0.0f;
float BoatController::_roll=0.0f;

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
	auto player = SceneManager::main->GetCurrentScene()->Find(L"player");
	_animation = player->GetComponent<AnimationListComponent>()->GetAnimations();
	_skined = player->GetComponent<SkinnedHierarchy>();


}

void BoatController::Update()
{
	if (_onBoard == false)
		return;


	if (Input::main->GetKeyDown(KeyCode::F))
	{
		_seq = Sequnce::turnRight;
		
		if (_animation.find("walk") != _animation.end())
		{
			_skined->Play(_animation["walk"], 0.5f);
		};
	};

	if (_seq == Sequnce::Driving)
	{

		float dt = Time::main->GetDeltaTime();
		Quaternion quat = CalCulateYawPitchRoll();
		Quaternion yawOnlyQuat = Quaternion::CreateFromYawPitchRoll(_yaw * D2R, 0, 0);

		GetOwner()->_transform->SetLocalRotation(yawOnlyQuat);

		if (Input::main->GetKey(KeyCode::W))
		{

			vec3 pos = GetOwner()->_transform->GetLocalPosition();
			GetOwner()->_transform->SetLocalPosition(pos + GetOwner()->_transform->GetForward() * dt * 50.0f);
		}

		auto ray = ColliderManager::main->RayCastForMyCell({ GetOwner()->_transform->GetLocalPosition(), GetOwner()->_transform->GetForward() }, 1.0f, GetOwner());

		if (ray.isHit)
		{
			vec3 normal = ray.normal;

			float penetrationBuffer = 0.05f;
			GetOwner()->_transform->SetLocalPosition(GetOwner()->_transform->GetLocalPosition() + normal * penetrationBuffer);
		}


		_camera->SetCameraPos(GetOwner()->_transform->GetWorldPosition() - GetOwner()->_transform->GetForward() * SpringArmLength + vec3(0, heightOffset, 0));
		_camera->SetCameraRotation(quat);
		//CameraManager::main->Setting();
	}

	else
	{
		DivingSequnce();
	}





	

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

void BoatController::DivingSequnce()
{
	auto player = SceneManager::main->GetCurrentScene()->Find(L"player");


	static auto right = player->_transform->GetRight();
	vec3 forward = player->_transform->GetForward();

	if (_seq == Sequnce::turnRight)
	{
		player->_transform->LookUpSmooth(right, vec3::UnitY, 10.0f);

		if ((right - forward).Length() < 0.0001f)
		{
			_seq = Sequnce::Walk;
		}
	};


	if (_seq == Sequnce::Walk)
	{
		static float moveLange = 50.0f;

		if (moveLange >= 0.0f)
		{
			vec3& pos = player->_transform->GetWorldPosition();
			player->_transform->SetWorldPosition(pos + player->_transform->GetForward() * Time::main->GetDeltaTime() * 1.0f);
			moveLange -= Time::main->GetDeltaTime() * 30.0f;
		}
		else
		{
			_seq = Sequnce::Dive;
			moveLange = 50.0f;
		}
	}


	if (_seq == Sequnce::Dive)
	{

	}











};

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

		lastMousePos = currentMousePos;
	}
	else
	{
		vec2 currentMousePos = Input::main->GetMousePosition();
		vec2 centerPos = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		vec2 delta = (currentMousePos - centerPos) * 0.1f;

		_yaw += delta.x;


		POINT center;
		center.x = static_cast<LONG>(WINDOW_WIDTH / 2);
		center.y = static_cast<LONG>(WINDOW_HEIGHT / 2);
		ClientToScreen(Core::main->GetHandle(), &center);
		SetCursorPos(center.x, center.y);
	}

	return Quaternion::CreateFromYawPitchRoll(_yaw * D2R, _pitch * D2R , _roll * D2R);
}