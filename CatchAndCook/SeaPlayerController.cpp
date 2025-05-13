﻿#include "pch.h"
#include "SeaPlayerController.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Collider.h"
#include "Transform.h"
#include "TerrainManager.h"
#include "Terrain.h"
#include "Gizmo.h"
#include "AnimationListComponent.h"
#include "SkinnedHierarchy.h"
#include "Animation.h"
#include "Weapon.h"

SeaPlayerController::SeaPlayerController()
{
}

SeaPlayerController::~SeaPlayerController()
{
}
void SeaPlayerController::Init()
{
	ImguiManager::main->playerHeightOffset = &_cameraHeightOffset;
	ImguiManager::main->playerForwardOffset = &_cameraForwardOffset;
	ImguiManager::main->cameraPitchOffset = &_cameraPitchOffset;
    ImguiManager::main->cameraYawOffset = &_cameraYawOffset;

    _weapons = GetOwner()->AddComponent<Weapon>();
}

void SeaPlayerController::Start()
{
	_camera = CameraManager::main->GetCamera(CameraType::SeaCamera);
	_transform = GetOwner()->_transform;
	_collider = GetOwner()->GetComponent<Collider>();

	if (auto terrian = SceneManager::main->GetCurrentScene()->Find(L"Terrain"))
	{
		_terrian = terrian->GetComponent<Terrain>();
	}


	GetOwner()->_transform->SetForward(vec3(0.308061f, 0.164842f, 0.936977f));
	_camera->SetCameraLook(vec3(0.308061f, 0.164842f, 0.936977f));
	//GetOwner()->_transform->SetWorldPosition(vec3(0, 1000.0f, 1.0f));

    _animations = GetOwner()->GetComponent<AnimationListComponent>()->GetAnimations();
    _skined  =GetOwner()->GetComponent<SkinnedHierarchy>();

	for (auto& animation : _animations)
	{
		cout << animation.first << endl;
	}


    _weapons->AddWeapon(L"test", L"Gun", L"Harpoon",L"HarpoonSlot");
    _weapons->SetWeapon(L"test");
}

void SeaPlayerController::Update()
{



    if (CameraManager::main->GetCameraType() == CameraType::DebugCamera)
    {
        Gizmo::Width(0.02f);
        auto o = _camera->GetCameraPos() + _camera->GetCameraLook() * _camera->GetNear();
        auto f = _camera->GetCameraLook();
        auto u = _camera->GetCameraUp();
        auto r = _camera->GetCameraRight();

        Gizmo::Line(o, o + f, Vector4(0, 0, 1, 1));
        Gizmo::Line(o, o + u, Vector4(0, 1, 0, 1));
        Gizmo::Line(o, o + r, Vector4(1, 0, 0, 1));

    }

  
    float dt = Time::main->GetDeltaTime();
    Quaternion playerRotation = CalCulateYawPitchRoll();

    _transform->SetWorldRotation(playerRotation);

    Quaternion cameraRotation = Quaternion::CreateFromYawPitchRoll(_yaw*D2R, _cameraPitchOffset * D2R + _pitch * D2R, 0);
    //Quaternion cameraRotation = pitchQuat * playerRotation;
    _camera->SetCameraRotation(cameraRotation);

    vec3 inputDir = vec3::Zero;
    KeyUpdate(inputDir, cameraRotation, dt);
    UpdateState(dt);
    UpdatePlayerAndCamera(dt, playerRotation, cameraRotation);
}

void SeaPlayerController::UpdatePlayerAndCamera(float dt, Quaternion& playerRotation, Quaternion& cameraRotation)
{
    vec3 currentPos = _transform->GetWorldPosition();
    vec3 nextPos = currentPos + _velocity * dt;
    vec3 headOffset = vec3(0, _cameraHeightOffset, 0);
    vec3 rotatedHeadOffset = vec3::Transform(headOffset, playerRotation);
    vec3 nextHeadPos = nextPos + rotatedHeadOffset + _transform->GetForward() * _cameraForwardOffset ;

    vec3 dir = _velocity;
    dir.Normalize();
    float maxDist = _playerRadius; 
    auto ray = ColliderManager::main->RayCastForMyCell({ nextHeadPos, dir }, maxDist, GetOwner());

    if (ray.isHit)
    {
        vec3 normal = ray.normal;

        _velocity = _velocity - _velocity.Dot(normal) * normal;

        float penetrationBuffer = 0.05f;
        nextPos += normal * penetrationBuffer;
        nextHeadPos += normal * penetrationBuffer;
    }



    // 지형 충돌 처리
    float terrainHeightAtHead = _terrian->GetLocalHeight(nextHeadPos);
    float desiredCameraHeight = terrainHeightAtHead + 5.0f;

    if (nextHeadPos.y < desiredCameraHeight)
    {
        float deltaY = desiredCameraHeight - nextHeadPos.y;
        nextHeadPos.y += deltaY;
        nextPos.y += deltaY;
    }

  

     // 최종 위치 적용
    _transform->SetWorldPosition(nextPos);
    _camera->SetCameraPos(nextHeadPos + _camera->GetCameraRight() * _cameraYawOffset );
    _velocity *= (1 - (_resistance * dt));

}

void SeaPlayerController::KeyUpdate(vec3& inputDir, Quaternion& rotation, float dt)
{
    if (Input::main->GetKey(KeyCode::W))
    {
        inputDir += vec3::Forward;
    }
    if (Input::main->GetKey(KeyCode::S))
    {
        inputDir += vec3::Backward;
    }
    if (Input::main->GetKey(KeyCode::A))
    {
        inputDir += vec3::Left;
    }
    if (Input::main->GetKey(KeyCode::D))
    {
        inputDir += vec3::Right;
    }

    if (Input::main->GetMouseDown(KeyCode::LeftMouse))
    {
        if (_state == SeaPlayerState::Aiming)
        {
            if (_weapons->GetState() == WeaponState::Idle)
            {
                _weapons->ChangeState(WeaponState::Shot);
            }
        }
    }

    if (Input::main->GetKey(KeyCode::Space))
    {
        inputDir += vec3(0, 2, 0);
    }

    if (inputDir != vec3::Zero)
    {
        inputDir.Normalize();
        vec3 moveDir = vec3::Transform(inputDir, rotation);
        _velocity += moveDir * _moveForce * dt;

        if (_velocity.Length() > _maxSpeed)
        {
            _velocity.Normalize();
            _velocity = _velocity * _maxSpeed;
        }
    }

    if (Input::main->GetMouseDown(KeyCode::RightMouse))
    {
        if (_state == SeaPlayerState::Aiming)
            SetState(SeaPlayerState::Idle);
        else
            SetState(SeaPlayerState::Aiming);
    }
  
    if (Input::main->GetKey(KeyCode::F1))
    {
        auto& camera = CameraManager::main->GetCamera(CameraType::DebugCamera);

		_transform->SetWorldPosition(camera->GetCameraPos());
    }


}

void SeaPlayerController::Update2()
{
}

void SeaPlayerController::Enable()
{
}

void SeaPlayerController::Disable()
{
}

void SeaPlayerController::RenderBegin()
{
}

void SeaPlayerController::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
    if (auto& owner = other->GetOwner())
    {
        if (owner->GetName() == L"2")
        {
			_state = SeaPlayerState::PushBack;
			_other = owner;
        }
    }
}

void SeaPlayerController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
    if (auto& owner = other->GetOwner())
    {
        if (owner->GetName() == L"2")
        {
            _state = SeaPlayerState::Idle;
        }
    }
}

void SeaPlayerController::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void SeaPlayerController::SetDestroy()
{
}

void SeaPlayerController::Destroy()
{
}

Quaternion SeaPlayerController::CalCulateYawPitchRoll()
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
		_pitch += delta.y;
        float minPitch = -90.0f - _cameraPitchOffset;
        float maxPitch = 90.0f - _cameraPitchOffset;
        _pitch = std::clamp(_pitch, minPitch, maxPitch);
		_roll = 0;

		lastMousePos = currentMousePos;
	}

	else
	{
		vec2 currentMousePos = Input::main->GetMousePosition();
		vec2 centerPos = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		vec2 delta = (currentMousePos - centerPos) * 0.1f;

		_yaw += delta.x;
		_pitch += delta.y;
        float minPitch = -90.0f - _cameraPitchOffset;
        float maxPitch = 90.0f - _cameraPitchOffset;
        _pitch = std::clamp(_pitch, minPitch, maxPitch);
		_roll = 0;

		POINT center;
		center.x = static_cast<LONG>(WINDOW_WIDTH/2);
		center.y = static_cast<LONG>(WINDOW_HEIGHT/2);
		ClientToScreen(Core::main->GetHandle(), &center);
		SetCursorPos(center.x, center.y);
	
	}


    return  Quaternion::CreateFromYawPitchRoll(_yaw * D2R, _pitch * D2R, 0);
}

void SeaPlayerController::UpdateState(float dt)
{

    switch (_state)
    {
    case SeaPlayerState::Idle:
        break;
    case SeaPlayerState::Aiming:
  
        break;
    case SeaPlayerState::Skill:
        break;
    case SeaPlayerState::Die:
        break;
    case SeaPlayerState::PushBack:
    {
        vec3 pushDir{};
        float pushSpeed{};
       
        if (auto& object =_other.lock())
        {
            vec3 otherCenter =object->GetComponent<Collider>()->GetBoundCenter();
            vec3 myCenter = _collider->GetBoundCenter();
            pushSpeed = (otherCenter - myCenter).Length();
            pushDir = (myCenter - otherCenter);
            pushDir.Normalize();
        }

        vec3 newPosition = _transform->GetWorldPosition() + pushDir * pushSpeed * dt;
        _transform->SetWorldPosition(newPosition);
    }
    break;
    case SeaPlayerState::Hit:
        break;
    default:
        break;
    }
}
void SeaPlayerController::SetState(SeaPlayerState state)
{
	if (_state == state)
		return;

	_state = state;

	switch (_state)
	{
	case SeaPlayerState::Idle:
        if (_animations.find("Swim_Idle") != _animations.end())
        {
            _skined->Play(_animations["Swim_Idle"], 0.5f);
        };
		break;
	case SeaPlayerState::Aiming:
        if (_animations.find("Swim_Run") != _animations.end())
        {
            _skined->Play(_animations["Swim_Run"], 0.5f);
        };
		break;
	case SeaPlayerState::Attack:

		break;
	case SeaPlayerState::Skill:

		break;
	case SeaPlayerState::Die:

		break;
	case SeaPlayerState::Hit:

		break;
	default:
		break;
	}

}
void SeaPlayerController::Idle(float dt)
{
}
void SeaPlayerController::Move(float dt)
{

}
void SeaPlayerController::Attack(float dt)
{

};