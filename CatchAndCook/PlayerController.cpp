#include "pch.h"
#include "PlayerController.h"

#include "Animation.h"
#include "Camera.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "Collider.h"
#include "ColliderManager.h"
#include "Gizmo.h"
#include "PhysicsComponent.h"
#include "SkinnedMeshRenderer.h"
#include "Terrain.h"
#include "TerrainManager.h"
#include "Transform.h"
#include "AnimationListComponent.h"
#include "InGameMainField.h"
#include "BoatController.h"


COMPONENT(PlayerController)

PlayerController::~PlayerController()
{

}

bool PlayerController::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void PlayerController::Init()
{
	Component::Init();

}

void PlayerController::Start()
{
	Component::Start();
	camera = FindObjectByType<CameraComponent>();
	_animationList = GetOwner()->GetComponentWithChilds<AnimationListComponent>();
	_skinnedHierarchy = GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

	if (auto skinnedHierarchy = _skinnedHierarchy.lock())
	{
		skinnedHierarchy->_speedMultiple = 1.2f;
	}
	if (auto animationList = _animationList.lock())
	{
		auto walk = animationList->GetAnimations()["walk"];
		auto idle = animationList->GetAnimations()["idle"];
		auto run = animationList->GetAnimations()["run"];
		run->_speedMultiplier = 1.15f;
	}
}

void PlayerController::Update()
{
	if (_onBoard)
		return;


	Component::Update();
	ColliderManager::main->UpdateDynamicCells();

	if (Input::main->IsMouseLock())
		CameraControl();


	MoveControl();


	Vector3 worldPos = GetOwner()->_transform->GetWorldPosition();
	float selectDist = 10000;
	std::shared_ptr<ObjectSettingComponent> selectObjectSetting;
	for (auto obj : InGameMainField::GetMain()->objectSettings)
	{
		
		auto currentObjectSetting = obj.lock();
		auto currentDist = (currentObjectSetting->GetOwner()->_transform->GetWorldPosition() - worldPos).Length();
		if (((!selectObjectSetting) || currentDist <= selectDist) && (currentObjectSetting->GetOwner() != GetOwner()))
		{
			selectObjectSetting = currentObjectSetting;
			selectDist = currentDist;
		}
	}

	if (selectDist <= 1 && selectObjectSetting)
	{
		selectObjectSetting->_objectSettingParam.o_select = true;
		selectObjectSetting->_objectSettingParam.o_selectColor = Vector4(0.90, 0.90, 0.90, 1) * 0.5;
	}
		

	for (auto& terrain : TerrainManager::main->GetTerrains())
		terrain->AddObjectPositionFront(GetOwner()->_transform->GetWorldPosition());
	//GetOwner()->_transform->SetWorldPosition(GetOwner()->_transform->GetWorldPosition() + Vector3::Forward * 10.0f * Time::main->GetDeltaTime());
}



void PlayerController::CameraControl()
{
	auto cameraTransform = camera.lock()->GetOwner()->_transform;
	auto currentMousePosition = Input::main->GetMousePosition();

	_targetOffset = GetOwner()->_transform->GetWorldPosition() + Vector3::Up * 1.2f;

	_currentOffset = Vector3::Lerp(_currentOffset, _targetOffset,
		std::clamp((float)Time::main->GetDeltaTime() * controlInfo.cameraMoveSmooth
		* std::min(Vector3::Distance(_currentOffset, _targetOffset), 1.0f), 0.0f, 1.0f));

	// 카메라 방향 계산식
	Vector2 mouseDelta = currentMousePosition - Input::main->GetMousePrevPosition();
	Vector3 deltaEuler = Vector3(mouseDelta.y, mouseDelta.x, 0) * 0.01f * 0.5f;
	Vector3 calculateEuler = deltaEuler + _targetEuler;

	calculateEuler.z = 0.0f;
	if (calculateEuler.x * R2D <= -80) calculateEuler.x = -80 * D2R;
	if (calculateEuler.x * R2D >= 85) calculateEuler.x = 85 * D2R;

	_targetEuler = calculateEuler;
	_currentEuler = Vector3::Lerp(_currentEuler, _targetEuler, std::clamp(Time::main->GetDeltaTime() * controlInfo.cameraRotationSmooth, 0.0, 1.0));


	Vector3 cameraNextPosition = _currentOffset;
	Quaternion finalRotation = Quaternion::CreateFromYawPitchRoll(_currentEuler);
	Vector3 _currentNextDirection = Vector3::Transform(Vector3::Forward, finalRotation);


	finalRotation = Quaternion::CreateFromYawPitchRoll(_currentEuler);
	_currentNextDirection = Vector3::Transform(Vector3::Forward, finalRotation);




	currentCameraDistance += ((Input::main->GetMouseDown(KeyCode::WheelDownMouse) ? 1 : 0) - (Input::main->GetMouseDown(KeyCode::WheelUpMouse) ? 1 : 0)) * 0.5f;

	currentCameraDistance = std::clamp(currentCameraDistance, minCameraDistance, maxCameraDistance);
	double finalCameraDistance = currentCameraDistance;
	double bias = 0.05f;
	std::vector<RayHit> hitList;
	if (auto isHit = ColliderManager::main->RayCastAll({ _currentOffset, -_currentNextDirection }, finalCameraDistance + bias, hitList))
	{
		for (auto& hit : hitList)
		{
			if (PhysicsComponent::GetPhysicsGroupID(hit.gameObject->GetCast<GameObject>()) != PhysicsComponent::GetPhysicsGroupID(GetOwner()) &&
				(hit.collider == nullptr || (hit.collider != nullptr && (!hit.collider->IsTrigger()))))
			{
				finalCameraDistance = std::min((double)hit.distance, finalCameraDistance) - bias;
				break;
			}
		}
	}

	cameraNextPosition = _currentOffset - _currentNextDirection * finalCameraDistance;

	cameraTransform->SetWorldPosition(cameraNextPosition);
	cameraTransform->LookUp(_currentNextDirection, Vector3::Up);
}

void PlayerController::MoveControl()
{
	Quaternion lookRotation = Quaternion::Identity;
	lookRotation = Quaternion::CreateFromYawPitchRoll(Vector3(0, _targetEuler.y, 0));

	// ------------- 공통 로직 ------------- 
	Vector3 moveDirection = Vector3(
		(Input::main->GetKey(KeyCode::D) ? 1 : 0) - (Input::main->GetKey(KeyCode::A) ? 1 : 0), 0,
		(Input::main->GetKey(KeyCode::W) ? 1 : 0) - (Input::main->GetKey(KeyCode::S) ? 1 : 0));
	moveDirection.Normalize();
	targetLookWorldDirection = Vector3::Transform(moveDirection, lookRotation);
	targetLookWorldDirectionDelayed += (targetLookWorldDirection - targetLookWorldDirectionDelayed) * Time::main->GetDeltaTime() * 10;

	if (auto animationList = _animationList.lock())
	{
		if (auto skinnedHierarchy = _skinnedHierarchy.lock())
		{
			auto walk = animationList->GetAnimations()["walk"];
			auto idle = animationList->GetAnimations()["idle"];
			auto run = animationList->GetAnimations()["run"];
			if (targetLookWorldDirectionDelayed.Length() < 0.15 && targetLookWorldDirection == Vector3::Zero)
				skinnedHierarchy->Play(idle, 0.25);
			else
				if (Input::main->GetKey(KeyCode::Shift))
					skinnedHierarchy->Play(run, 0.25);
				else
					skinnedHierarchy->Play(walk, 0.25);
		}
	}


	if (targetLookWorldDirection != Vector3::Zero)
	{
		currentLookWorldRotation = Quaternion::Slerp(currentLookWorldRotation, 
			Quaternion::LookRotation(targetLookWorldDirection, Vector3::Transform(Vector3::Up, lookRotation)),
			std::clamp(Time::main->GetDeltaTime() * 15, 0.0, 1.0));
	}

	//if (!(targetLookWorldDirectionDelayed.Length() < 0.1 && targetLookWorldDirection == Vector3::Zero))
	//{
	//	// 등속 운동 로직
	//	Vector3 prevVelocity = velocity;
	//	velocity += targetLookWorldDirection * controlInfo.moveForce * Time::main->GetDeltaTime() * 60;
	//	velocity.y = 0;
	//	if (velocity.Length() > controlInfo.maxSpeed) {
	//		velocity.Normalize();
	//		velocity *= controlInfo.maxSpeed;
	//	}
	//	// 이동
	//	velocity = Vector3(velocity.x, prevVelocity.y, velocity.z);
	//}
	Vector3 prevVelocity = velocity;
	if (auto skinnedHierarchy = _skinnedHierarchy.lock())
		velocity = skinnedHierarchy->GetDeltaPosition() * GetOwner()->_transform->GetWorldScale().y;
	velocity = Vector3(velocity.x, prevVelocity.y, velocity.z);

	GetOwner()->_transform->LookUp(Vector3::Transform(Vector3::Forward, currentLookWorldRotation), Vector3::Up);


	isGround = false;


	std::vector<BoundingData> playerColliderDatas;
	std::vector<std::shared_ptr<Collider>> playerColliders;

	GetOwner()->GetComponentsWithChilds<Collider>(playerColliders);
	int playerGroupID = PhysicsComponent::GetPhysicsGroupID(GetOwner());
	for (auto& collider : playerColliders) {
		if (playerGroupID == collider->GetGroupID()) {
			collider->CalculateBounding();
			playerColliderDatas.push_back(collider->GetBoundingData());
		}
	}


	Vector3 currentPos = GetOwner()->_transform->GetWorldPosition();
	Vector3 nextPos = currentPos;

	Vector3 velocityDirectionXZ = velocity; //  * Time::main->GetDeltaTime()
	velocityDirectionXZ.y = 0;

	//if (velocityDirectionXZ != Vector3::Zero)
	{
		for (auto boundingData : playerColliderDatas)
		{
			// 이동할 위치 미리 계산
			Vector3 nextCenter;
			float currentRadius;

			currentRadius = boundingData.GetRadius();
			nextCenter = boundingData.SetCenter(boundingData.GetCenter() + velocityDirectionXZ);


			// 이동할 곳에서 충돌되는지 보기.
			std::vector<std::shared_ptr<Collider>> otherColliders;

			if (ColliderManager::main->CollisionChecksDirect(boundingData, otherColliders))
			{
				for (auto& otherCollider : otherColliders)
				{

					if (otherCollider->GetGroupID() != playerGroupID && !otherCollider->IsTrigger())
					{
						auto otherBoundingData = otherCollider->GetBoundingData();
						Vector3 hitPosition = Collider::GetContactPoint(boundingData, otherBoundingData);
						
						auto pushDir = nextCenter - hitPosition;
						auto pushNormal = pushDir;
						pushNormal.Normalize();

						velocityDirectionXZ += pushNormal * std::max(currentRadius - pushDir.Length(), 0.0f);
					}
				}
			}
		}
	}
	float velocityDirectionY = velocity.y * Time::main->GetDeltaTime() * 60;
	nextPos += Vector3(velocityDirectionXZ.x, velocityDirectionY, velocityDirectionXZ.z); // velocityDirectionXZ.y + velocityDirectionY



	float gitMargin = 0.15f;//오차 범위
	float upDistance = 1.0f;
	Vector3 upRayOffset = nextPos + Vector3::Up * upDistance;

	RayHit foundGround;
	std::vector<RayHit> hitList;

	if (auto isHit = ColliderManager::main->RayCastAllForMyCellDirect({ upRayOffset, Vector3::Down }, upDistance + gitMargin, hitList, BoundingData{CollisionType::Box, BoundingUnion(BoundingOrientedBox(currentPos, Vector3(2,2,2), Quaternion::Identity))}))
	{
		for (auto& hit : hitList)
		{
		
			if (PhysicsComponent::GetPhysicsGroupID(hit.gameObject->GetCast<GameObject>()) != PhysicsComponent::GetPhysicsGroupID(GetOwner()) && 
				(hit.collider ==nullptr || (hit.collider != nullptr && (!hit.collider->IsTrigger()))))
			{
				if (hit.distance <= upDistance + gitMargin) {
					foundGround = hit;
					break;
				}
			}
		}
	}
	if (foundGround)
	{
		isGround = true;
		nextPos.y = upRayOffset.y - foundGround.distance; //upRayOffset.y -
	}

	if (isGround)
	{
		velocity.y = 0;
	}
	else
	{
		velocity.y -= 9.81 * 0.5 * Time::main->GetDeltaTime();
	}

	// ------------- 공통 로직 ------------- 

	GetOwner()->_transform->SetWorldPosition(nextPos);
	velocity = velocity * (1 - (controlInfo.drag * Time::main->GetDeltaTime() * 60));

	//temp

	if (Input::main->GetKeyDown(KeyCode::P))
	{
		auto& cameraPos = CameraManager::main->GetCamera(CameraType::DebugCamera)->GetCameraPos();

		GetOwner()->_transform->SetWorldPosition(cameraPos);

	}

	if (Input::main->GetKeyDown(KeyCode::J))
	{
		SetOnBoard();
	}
}


void PlayerController::Update2()
{

	if (_onBoard)
		return;

	Component::Update2();

	camera.lock()->Calculate();

	CameraManager::main->Setting();
}

void PlayerController::Enable()
{
	Component::Enable();
}

void PlayerController::Disable()
{
	Component::Disable();
}

void PlayerController::RenderBegin()
{
	Component::RenderBegin();
}

void PlayerController::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);

}

void PlayerController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void PlayerController::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void PlayerController::SetDestroy()
{
	Component::SetDestroy();
}

void PlayerController::Destroy()
{
	Component::Destroy();
}

void PlayerController::SetOnBoard()
{
	_onBoard = true;

	auto playerShip =SceneManager::main->GetCurrentScene()->Find(L"PlayerShip");
	playerShip->GetComponent<BoatController>()->SetOnBaord();

	auto boatSeat = SceneManager::main->GetCurrentScene()->Find(L"BoatSeat");
	GetOwner()->GetRoot()->SetParent(boatSeat);
	GetOwner()->_transform->SetLocalPosition(vec3(0, 0, 0));
	GetOwner()->_transform->SetLocalRotation(vec3(0, 0, 0));

}

void PlayerController::SetOFFBoard()
{
	_onBoard = false;
}
