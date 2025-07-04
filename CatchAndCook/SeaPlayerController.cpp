#include "pch.h"
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
#include "ParticleManager.h"
#include "AnimationSpriteComponent.h"
#include "FishMonster.h"
#include "GraphData.h"
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

    _weapons = make_shared<Weapon>();
    _weapons->Init(this);

 
 

}

void SeaPlayerController::Start()
{
    wcout << " 호출스택:"<< debug_test++ << " 이름:" << GetOwner()->GetRoot()->GetName() << "SeaPlayerController" << endl;

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

    _animations = GetOwner()->GetComponentWithChilds<AnimationListComponent>()->GetAnimations();
    _skined  =GetOwner()->GetRoot()->GetComponentWithChilds<SkinnedHierarchy>();

    if (_skined == nullptr)
    {
        cout << "NULL" << endl;
        _skined = GetOwner()->AddComponent<SkinnedHierarchy>();
    }

    if (_animations.find("shot") != _animations.end())
    {
        _animations.find("shot")->second->_isLoop = false;
        _animations.find("shot")->second->_speedMultiplier = 3.0f;
    }

    _weapons->AddWeapon(L"Gun", L"GunSlot",1000.0f);
    _weapons->SetCurrentWeapon(L"Gun");
    _weapons->SetTargetHudVisible(false);

    _fireEffect = SceneManager::main->GetCurrentScene()->Find(L"FireEffect");

    if (_animations.find("idle") != _animations.end())
    {
        _skined->Play(_animations["idle"], 0.5f);
        SetState(SeaPlayerState::Idle);
    };


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

        _weapons->SetTargetHudVisible(false);

    }

  
    float dt = Time::main->GetDeltaTime();
    Quaternion playerRotation = CalCulateYawPitchRoll();

    _transform->SetWorldRotation(playerRotation);

    Quaternion cameraRotation = Quaternion::CreateFromYawPitchRoll(_yaw*D2R, _cameraPitchOffset * D2R + _pitch * D2R, 0);
    //Quaternion cameraRotation = pitchQuat * playerRotation;
    _camera->SetCameraRotation(cameraRotation);

    vec3 inputDir = vec3::Zero;
    KeyUpdate(inputDir, cameraRotation, dt);
    UpdatePlayerAndCamera(dt, playerRotation, cameraRotation);
    UpdateState(dt);
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
    _camera->Calculate();
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


    if (Input::main->GetMouseDown(KeyCode::RightMouse))
    {
        if (_skined->_blendEnd)
        {
            if (_state == SeaPlayerState::Idle)
            {
                SetState(SeaPlayerState::Aiming);
            }

            else if (_state == SeaPlayerState::Aiming)
            {
                SetState(SeaPlayerState::Idle);
            }
        }
    }

    if (Input::main->GetMouseDown(KeyCode::LeftMouse))
    {
        if (_state == SeaPlayerState::Aiming)
        {
            if (_skined->_blendEnd)
            {
                SetState(SeaPlayerState::Shot);
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


    if (Input::main->GetKey(KeyCode::F1))
    {
        auto& camera = CameraManager::main->GetCamera(CameraType::DebugCamera);
        GetOwner()->_transform->SetWorldPosition(camera->GetCameraPos());
        SetState(SeaPlayerState::Idle);
    }


}

void SeaPlayerController::Update2()
{
  
    /*vec3 nearPlane = vec3(0.0f, 0.0f, 0.0f);
    vec3 farPlane = vec3(0.0f, 0.0f, 1.0f);

    vec3 worldNear = vec3::Transform(nearPlane, CameraManager::main->GetCamera(CameraType::SeaCamera)->_params.InvertVPMatrix);
    vec3 worldFar = vec3::Transform(farPlane, CameraManager::main->GetCamera(CameraType::SeaCamera)->_params.InvertVPMatrix);

    vec3 dir = worldFar - worldNear;
    dir.Normalize();

    Gizmo::main->Width(3.0f);
    Gizmo::main->Line(worldNear, worldFar);*/
}

void SeaPlayerController::Enable()
{
}

void SeaPlayerController::Disable()
{
}

void SeaPlayerController::RenderBegin()
{

   //auto& graphData= GraphData::datas;

   //for (size_t i = 0; i < graphData.size() - 1; ++i)
   //{
   //    const auto& a = graphData[i];
   //    const auto& b = graphData[i + 1];

   //    Gizmo::main->Line(a, b);
   //}

 

}

void SeaPlayerController::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
    /*if (auto& owner = other->GetOwner())
    {
        if (owner->GetName() == L"2")
        {
			_state = SeaPlayerState::PushBack;
			_other = owner;
        }
    }*/
}

void SeaPlayerController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
 /*   if (auto& owner = other->GetOwner())
    {
        if (owner->GetName() == L"2")
        {
            _state = SeaPlayerState::Idle;
        }
    }*/
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

    _weapons->SetTargetHudSize();

    switch (_state)
    {
    case SeaPlayerState::Idle:
      
        break;
    case SeaPlayerState::Aiming:
   
        break;
    case SeaPlayerState::Shot:
        if (_skined->_isPlaying==false)
        {
            SetState(SeaPlayerState::Aiming);
        }
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
    {

        if (_animations.find("idle") != _animations.end())
        {
            _weapons->SetTargetHudVisible(false);
            _skined->Play(_animations["idle"], 0.5f);
        };
    }
    break;
    case SeaPlayerState::Aiming:
    {

        if (_animations.find("aiming") != _animations.end())
        {
            _weapons->SetTargetHudVisible(true);
            _skined->Play(_animations["aiming"], 0.5f);
        };

        //_fireEffect->SetActiveSelf(false);

    }
    break;
    case SeaPlayerState::Shot:
    {
  
        vec3 nearPlane = vec3(0.0f, 0.0f, 0.0f);
        vec3 farPlane = vec3(0.0f, 0.0f, 1.0f);

        vec3 worldNear = vec3::Transform(nearPlane, CameraManager::main->GetCamera(CameraType::SeaCamera)->_params.InvertVPMatrix);
        vec3 worldFar = vec3::Transform(farPlane, CameraManager::main->GetCamera(CameraType::SeaCamera)->_params.InvertVPMatrix);

        vec3 dir = worldFar - worldNear;
        dir.Normalize();

        float maxDist = 1000000.0f;

        auto ray = ColliderManager::main->RayCast({ worldNear, dir }, maxDist, GetOwner());

        if (ray.isHit)
        {
            if (ray.gameObject->GetRoot())
            {
                if (ray.gameObject->GetRoot()->HasTag(GameObjectTag::Monster))
                {
                    ParticleManager::main->GenParticle(10.0f, 200, 10.0f, 60.0f, ray.worldPos, ray.normal, ParticleMoveType::BloodUnderwater, ParticleColorType::Red, { 0,0,0,0 }
                    , ResourceManager::main->Get<Texture>(L"bleedTexture"));
                 
                    if (auto& fishMonster =ray.gameObject->GetRoot()->GetComponent<FishMonster>())
                    {
                        fishMonster->EventDamage(10.0f);
                    }
                }

                else if (ray.gameObject->HasTag(GameObjectTag::Defualt) && !ray.gameObject->HasTag(GameObjectTag::EVENT))
                {
                    ParticleManager::main->GenParticle(3.0f, 200, 6.0f,50.0f, ray.worldPos, ray.normal, ParticleMoveType::RadialSpread, ParticleColorType::Red, { 0,0,0,0 }
                        , ResourceManager::main->Get<Texture>(L"smokeTexture"));
                }

            }
        }

        if (_animations.find("shot") != _animations.end())
        {
            _skined->Play(_animations["shot"], 0.5f);
        };

        _weapons->Shot();
        _fireEffect->SetActiveSelf(true);
        _fireEffect->GetComponent<AnimationSpriteComponent>()->Reset();
        break;
    }
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