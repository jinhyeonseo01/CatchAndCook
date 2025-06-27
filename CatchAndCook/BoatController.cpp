#include "pch.h"
#include "BoatController.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Transform.h"
#include "ColliderManager.h"
#include "AnimationListComponent.h"
#include "SkinnedHierarchy.h"
#include "Animation.h"
#include "PlayerController.h"
#include "ParticleManager.h"
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

	if (_animation.find("right_turn") != _animation.end())
	{
		_animation.find("right_turn")->second->_isLoop = false;
	}

	if (_animation.find("dive") != _animation.end())
	{
		auto temp = _animation.find("dive")->second;
		temp->_isLoop= false;
		temp->_speedMultiplier = 1.0f;

	}

	_GenPos = GetOwner()->_transform->GetWorldPosition();
	_GenRotate = GetOwner()->_transform->GetWorldRotation();

}

void BoatController::Update()
{
	if (_onBoard == false)
		return;


	if (Input::main->GetKeyDown(KeyCode::F))
	{
		if (_seq == Sequnce::Driving)
		{
			auto player = SceneManager::main->GetCurrentScene()->Find(L"player");
			_seq = Sequnce::turnRight;
			_right = player->_transform->GetRight();

			if (_animation.find("right_turn") != _animation.end())
			{
				_skined->Play(_animation["right_turn"], 0.1f);
			};
		}
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
			GetOwner()->_transform->SetLocalPosition(pos + GetOwner()->_transform->GetForward() * dt * 20.0f);
		}

		auto ray = ColliderManager::main->RayCastForMyCell({ GetOwner()->_transform->GetLocalPosition(), GetOwner()->_transform->GetForward() }, 1.0f, GetOwner());

		if (ray.isHit)
		{
			float penetrationBuffer = 0.2f;
			GetOwner()->_transform->SetLocalPosition(GetOwner()->_transform->GetLocalPosition() + ray.normal * penetrationBuffer);
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

	_skined->Play(_animation["idle"], 0.5f);

	CameraManager::main->SetActiveCamera(CameraType::BoatCamera);

}

void BoatController::DivingSequnce()
{
	auto player = SceneManager::main->GetCurrentScene()->Find(L"player");

	vec3 forward = player->_transform->GetForward();

	if (_seq == Sequnce::turnRight)
	{
		if (_skined->_isPlaying == false)
		{
			player->_transform->LookUp(_right, vec3::Up);
			_skined->Play(_animation["walk"], 0.1f);
			_seq = Sequnce::Walk;
		}
	};


	if (_seq == Sequnce::Walk)
	{
		static float moveLange = 40.0f;

		if (moveLange >= 0.0f)
		{
			vec3& pos = player->_transform->GetWorldPosition();
			player->_transform->SetWorldPosition(pos + player->_transform->GetForward() * Time::main->GetDeltaTime() * 1.0f);
			moveLange -= Time::main->GetDeltaTime() * 30.0f;
		}
		else
		{
			_skined->Play(_animation["dive"], 0.5f);
			_seq = Sequnce::Dive;
			moveLange = 40.0f;
		}
	}



	if (_seq == Sequnce::Dive)
	{

		static bool up = true;
		static float upTime = 0.5f;

		vec3& pos = player->_transform->GetWorldPosition();

		if (up)
		{
			vec3 worldPos = pos + player->_transform->GetForward() * Time::main->GetDeltaTime() * 4.0f + vec3(0, 1, 0) * Time::main->GetDeltaTime() * 4.0f;
			player->_transform->SetWorldPosition(worldPos);

			upTime -= Time::main->GetDeltaTime();

			if (upTime <= 0)
			{
				upTime = 0.5f;
				up = false;
			}
		}


		else
		{
			vec3 worldPos = pos + player->_transform->GetForward() * Time::main->GetDeltaTime() * 4.0f + vec3(0, -1, 0) * Time::main->GetDeltaTime() * 7.0f;
			auto& currentPos = player->_transform->SetWorldPosition(worldPos);
		/*	static bool GenParticle = false;

			if (currentPos.y <= 45.0f)
			{
				if (GenParticle == false)
				{
					GenParticle = true;
					ParticleManager::main->GenParticle(1.5f, 500, 1.0, player->_transform->GetWorldPosition(), ParticleMoveType::RadialSpread, ParticleColorType::Random);
				}
			}*/

			if (currentPos.y < 35.0f)
			{
				_onBoard = false;
	/*			GenParticle = false;*/
				player->GetComponent<PlayerController>()->SetOFFBoard();

				GetOwner()->_transform->SetLocalPosition(_GenPos);
				GetOwner()->_transform->SetLocalRotation(_GenRotate);

				_seq = Sequnce::Driving;
				up = true;

				Scene::_changeScene = true;
				CameraManager::main->SetActiveCamera(CameraType::SeaCamera);
			}

		}


		


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