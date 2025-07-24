#include "pch.h"
#include "NPCComponent.h"

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
#include "InitComponent.h"
#include "NavMeshManager.h"

COMPONENT(NPCComponent)

NPCComponent::~NPCComponent()
{

}

bool NPCComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void NPCComponent::Init()
{
	Component::Init();

	fsm = make_shared<NPCFSMGroup>();
	fsm->Init();
	fsm->AddState(StateType::goto_any, make_shared<NPCGotoAny>());
	fsm->AddState(StateType::idle, make_shared<NPCIdle>());
	fsm->AddState(StateType::goto_shop, make_shared<NPCGotoShop>());
	fsm->AddState(StateType::goto_table, make_shared<NPCGotoTable>());
	fsm->AddState(StateType::eat, make_shared<NPCEatting>());
	fsm->SetNPC(GetCast<NPCComponent>());
}

void NPCComponent::Start()
{
	Component::Start();

	auto _animationList = GetOwner()->GetComponentWithChilds<AnimationListComponent>();
	auto _skinnedHierarchy = GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

	if (auto skinnedHierarchy = _skinnedHierarchy)
	{
		skinnedHierarchy->_speedMultiple = 1.2f;
	}
	if (auto animationList = _animationList)
	{
		auto walk = animationList->GetAnimations()["walk"];
		auto idle = animationList->GetAnimations()["idle"];
		auto run = animationList->GetAnimations()["run"];
		auto sit  = animationList->GetAnimations()["sitting"];
		auto sitdown  = animationList->GetAnimations()["sitdown"];
		auto situp  = animationList->GetAnimations()["situp"];
		run->_speedMultiplier = 1.15f;

		sitdown->_isLoop = false;
		situp->_isLoop = false;
		_skinnedHierarchy->Play(idle, 0.25);
	}

	std::vector<std::shared_ptr<GameObject>> pathPointObjects;
	SceneManager::main->GetCurrentScene()->FindsInclude(L"$PathPoint", pathPointObjects);
	for (auto path : pathPointObjects)
		gotoPoints.push_back(path->_transform->GetWorldPosition());


	//pathPointObjects
	//shopEntryPoint

	fsm->ChangeState(StateType::goto_any);
}

void NPCComponent::Update()
{
	Component::Update();
	fsm->Update();
	fsm->AnyUpdate();
}

void NPCComponent::MoveControl()
{
	auto _skinnedHierarchy = GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

	Vector3 prevVelocity = velocity;
	if (auto skinnedHierarchy = _skinnedHierarchy)
		velocity = skinnedHierarchy->GetDeltaPosition() * GetOwner()->_transform->GetWorldScale().y;
	velocity = Vector3(velocity.x, prevVelocity.y, velocity.z);

	
	GetOwner()->_transform->LookUp(lookDirection, Vector3::Up);


	isGround = false;

	std::vector<BoundingData> playerColliderDatas;
	std::vector<std::shared_ptr<Collider>> playerColliders;

	GetOwner()->GetComponentsWithChilds<Collider>(playerColliders);
	int npcGroupID = PhysicsComponent::GetPhysicsGroupID(GetOwner());
	for (auto& collider : playerColliders) {
		if (npcGroupID == collider->GetGroupID()) {
			collider->CalculateBounding();
			playerColliderDatas.push_back(collider->GetBoundingData());
		}
	}


	Vector3 currentPos = GetOwner()->_transform->GetWorldPosition();
	Vector3 nextPos = currentPos;

	Vector3 velocityDirectionXZ = velocity; //  * Time::main->GetDeltaTime()
	velocityDirectionXZ.y = 0;

	//if (velocityDirectionXZ != Vector3::Zero)
	//if ((CameraManager::main->GetActiveCamera()->GetCameraPos() - GetOwner()->_transform->GetWorldPosition()).Length() <= 50)
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
					if (otherCollider->GetGroupID() != npcGroupID && !otherCollider->IsTrigger())
					{
						auto otherBoundingData = otherCollider->GetBoundingData();
						Vector3 hitPosition = Collider::GetContactPoint(boundingData, otherBoundingData);
						auto pushDir = nextCenter - hitPosition;
						auto pushNormal = pushDir;
						pushNormal.Normalize();

						Vector3 finalPushForce = pushNormal * std::max(currentRadius - pushDir.Length(), 0.0f);
						velocityDirectionXZ += finalPushForce;

						if (auto tag = otherCollider->GetOwner()->GetComponentWithParents<NPCComponent>()) {
							if (tag->GetOwner()->HasTag(GameObjectTag::NPC))
								velocityDirectionXZ += pushNormal.Cross(Vector3::Up) * finalPushForce.Length() * 0.25;
						}
					}
				}
			}
		}
	}
	float velocityDirectionY = velocity.y * Time::main->GetDeltaTime() * 60;
	nextPos += Vector3(velocityDirectionXZ.x, velocityDirectionY, velocityDirectionXZ.z); // velocityDirectionXZ.y + velocityDirectionY



	float gitMargin = 0.25f;//오차 범위
	float upDistance = 1.0f;
	Vector3 upRayOffset = nextPos + Vector3::Up * upDistance;

	RayHit foundGround;
	std::vector<RayHit> hitList;

	if (auto isHit = ColliderManager::main->RayCastAllForMyCellDirect({ upRayOffset, Vector3::Down }, upDistance + gitMargin, hitList,
		BoundingData{ CollisionType::Box, BoundingUnion(BoundingOrientedBox(currentPos, Vector3(2,2,2), Quaternion::Identity)) }))
	{
		for (auto& hit : hitList)
		{
			if (hit.gameObject->GetRoot() != GetOwner()->GetRoot() 
				&& (hit.collider == nullptr || (hit.collider != nullptr && (!hit.collider->IsTrigger()))))
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
}

void NPCComponent::Update2()
{
	Component::Update2();
}

void NPCComponent::Enable()
{
	Component::Enable();
}

void NPCComponent::Disable()
{
	Component::Disable();
}

void NPCComponent::RenderBegin()
{
	Component::RenderBegin();
}

void NPCComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void NPCComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void NPCComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void NPCComponent::SetDestroy()
{
	Component::SetDestroy();
}

void NPCComponent::Destroy()
{
	Component::Destroy();
}

Vector3 NPCComponent::AdvanceAlongPath(const std::vector<Vector3>& path, const Vector3& worldPos, float t)

{
	size_t n = path.size();
	if (n == 0) return Vector3::Zero;

	// 누적 거리 계산
	std::vector<float> cum(n);
	cum[0] = 0;
	for (size_t i = 1; i < n; ++i)
		cum[i] = cum[i - 1] + (path[i] - path[i - 1]).Length();

	float totalLen = cum[n - 1];

	// 1) P에 가장 가까운 투영점 찾기
	float bestD2 = FLT_MAX;
	float startDist = 0;

	for (size_t i = 0; i + 1 < n; ++i)
	{
		Vector3 A = path[i], B = path[i + 1], AB = B - A;
		float ab2 = AB.Dot(AB);
		float t = (ab2 > 0) ? (worldPos - A).Dot(AB) / ab2 : 0;
		t = std::clamp(t, 0.0f, 1.0f);

		Vector3 Q = A + AB * t;
		float d2 = (Q - worldPos).LengthSquared();

		if (d2 < bestD2)
		{
			bestD2 = d2;
			float segLen = std::sqrt(ab2);
			startDist = cum[i] + t * segLen;
		}
	}

	// 2) 목표 거리 계산 및 클램핑
	float target = startDist + t;
	if (target <= 0)        return path.front();
	if (target >= totalLen) return path.back();

	// 3) 목표 지점이 속한 세그먼트 찾아서 보간
	for (size_t i = 0; i + 1 < n; ++i)
	{
		if (cum[i + 1] >= target)
		{
			float segLen = cum[i + 1] - cum[i];
			float u = (target - cum[i]) / segLen;
			return path[i] + (path[i + 1] - path[i]) * u;
		}
	}

	return path.back(); // 안전 장치
}


// -------- FSM --------

void NPCFSMGroup::AnyUpdate()
{
	StatePatternGroup::AnyUpdate();

}

void NPCFSMGroup::Init()
{
	StatePatternGroup::Init();
}

void NPCFSMGroup::Update()
{
	StatePatternGroup::Update();
}

void NPCFSMGroup::SetNPC(const std::shared_ptr<NPCComponent>& npc)
{
	this->npc = npc;
	for (auto state : statePatterns)
		if (auto state2 = std::dynamic_pointer_cast<NPCState>(state.second))
			state2->SetNPC(npc);
}

// -------- Goto_Any --------

void NPCGotoAny::Init()
{
	StatePattern::Init();
}

void NPCGotoAny::Begin(StateType type, const std::shared_ptr<StatePattern>& prevState)
{
	StatePattern::Begin(type, prevState);

	auto npc = this->npc.lock();

	std::vector<std::shared_ptr<GameObject>> pathPointObjects;
	std::vector<std::shared_ptr<GameObject>> pathPointObjectsFilter;
	SceneManager::main->GetCurrentScene()->FindsInclude(L"$PathPoint", pathPointObjects);
	std::ranges::copy_if(pathPointObjects, std::back_inserter(pathPointObjectsFilter), [&](const std::shared_ptr<GameObject>& current) {
		return (current->GetName().find(L"Out") != std::wstring::npos);
		});

	runMove = false;

	if (pathPointObjectsFilter.size() != 0)
	{
		npc->paths.clear();
		int i = 0;
		while (npc->paths.empty())
		{
			auto endPoint = pathPointObjectsFilter[_random_dist(_random) % pathPointObjectsFilter.size()]->_transform->GetWorldPosition();
			endPoint += Vector3(_random_dist01(_random) * 6 - 3, 0, _random_dist01(_random) * 6 - 3);
			npc->paths = NavMeshManager::main->CalculatePath(npc->GetOwner()->_transform->GetWorldPosition(), endPoint);
			++i;
			if (i > 5)
				break;
		}


		runMove = (abs(_random_dist(_random)) % 10) == 0;
		
		auto animationList = npc->GetOwner()->GetComponentWithChilds<AnimationListComponent>();
		auto skinnedHierarchy = npc->GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

		skinnedHierarchy->_speedMultiple = 1.25f;

		if (animationList)
		{
			auto walk = animationList->GetAnimations()["walk"];
			auto idle = animationList->GetAnimations()["idle"];
			auto run = animationList->GetAnimations()["run"];
			skinnedHierarchy->Play(runMove ? run : walk, 0.25);
		}
	}
}

void NPCGotoAny::Update()
{
	StatePattern::Update();
	auto npc = this->npc.lock();

	if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::NavMesh))
	{
		Gizmo::Width(0.2f);
		for (int i = 1; i < (int)npc->paths.size(); ++i) {
			Gizmo::Line(npc->paths[i - 1], npc->paths[i], Vector4(1, 0, 0, 1));
		}
		Gizmo::WidthRollBack();
	}

	Vector3 currentWorldPos = npc->GetOwner()->_transform->GetWorldPosition();
	Vector3 nextPos = npc->AdvanceAlongPath(npc->paths, currentWorldPos, 0.75);
	Vector3 nextDir = Vector2(nextPos.x, nextPos.z) - Vector2(currentWorldPos.x, currentWorldPos.z);
	nextDir.Normalize();
	npc->lookDirection = Vector3(nextDir.x, 0, nextDir.y);

	for (auto& terrain : TerrainManager::main->GetTerrains())
		terrain->AddObjectPosition(currentWorldPos);

	auto animationList = npc->GetOwner()->GetComponentWithChilds<AnimationListComponent>();
	auto skinnedHierarchy = npc->GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();
	
	if (animationList)
	{
		auto walk = animationList->GetAnimations()["walk"];
		auto idle = animationList->GetAnimations()["idle"];
		auto run = animationList->GetAnimations()["run"];
		auto sit = animationList->GetAnimations()["sitting"];
		auto sitdown = animationList->GetAnimations()["sitdown"];
		auto situp = animationList->GetAnimations()["situp"];

		if (npc->paths.size() != 0)
		{
			auto endPoint = Vector2(npc->paths[npc->paths.size() - 1].x, npc->paths[npc->paths.size() - 1].z);
			if ((endPoint - Vector2(currentWorldPos.x, currentWorldPos.z)).Length() <= 0.25)
			{
				skinnedHierarchy->Play(idle, 0.25);
				bool gotoShop = false;
				if (InGameMainField::GetMain()->shopOpen)
					gotoShop = (_random_dist(_random) % 5) == 0; // 25% 확률로 가게로 이동
				if (gotoShop)
					GetGroup()->ChangeState(StateType::goto_shop);
				else
					GetGroup()->ChangeState(StateType::idle);
			}
			else
			{
				skinnedHierarchy->Play(runMove ? run : walk, 0.25);
			}
		}
		else
		{
			skinnedHierarchy->Play(idle, 0.25);
			GetGroup()->ChangeState(StateType::idle);
		}
	}

	ColliderManager::main->UpdateDynamicCells();
	npc->MoveControl();
}


bool NPCGotoAny::TriggerUpdate()
{
	return StatePattern::TriggerUpdate();
}

void NPCGotoAny::End(const std::shared_ptr<StatePattern>& nextState)
{
	StatePattern::End(nextState);
}


//GOTO Shop

void NPCGotoShop::Init()
{
	NPCState::Init();
}

void NPCGotoShop::Update()
{
	NPCState::Update();

	auto npc = this->npc.lock();

	if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::NavMesh))
	{
		Gizmo::Width(0.2f);
		for (int i = 1; i < (int)npc->paths.size(); ++i) {
			Gizmo::Line(npc->paths[i - 1], npc->paths[i], Vector4(1, 0, 0, 1));
		}
		Gizmo::WidthRollBack();
	}

	Vector3 currentWorldPos = npc->GetOwner()->_transform->GetWorldPosition();
	Vector3 nextPos = npc->AdvanceAlongPath(npc->paths, currentWorldPos, 0.75);
	Vector3 nextDir = Vector2(nextPos.x, nextPos.z) - Vector2(currentWorldPos.x, currentWorldPos.z);
	nextDir.Normalize();
	npc->lookDirection = Vector3(nextDir.x, 0, nextDir.y);

	auto animationList = npc->GetOwner()->GetComponentWithChilds<AnimationListComponent>();
	auto skinnedHierarchy = npc->GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

	if (animationList)
	{
		auto walk = animationList->GetAnimations()["walk"];
		auto idle = animationList->GetAnimations()["idle"];
		auto run = animationList->GetAnimations()["run"];
		auto sit = animationList->GetAnimations()["sitting"];
		auto sitdown = animationList->GetAnimations()["sitdown"];
		auto situp = animationList->GetAnimations()["situp"];

		if (npc->paths.size() != 0)
		{
			auto endPoint = Vector2(npc->paths[npc->paths.size() - 1].x, npc->paths[npc->paths.size() - 1].z);
			if ((endPoint - Vector2(currentWorldPos.x, currentWorldPos.z)).Length() <= 0.25)
			{
				skinnedHierarchy->Play(idle, 0.25);
				if (InGameMainField::GetMain()->shopOpen)
					GetGroup()->ChangeState(StateType::goto_table);
				else
					GetGroup()->ChangeState(StateType::goto_any);
			}
			else
			{
				skinnedHierarchy->Play(walk, 0.25);
			}
		}
		else
		{
			skinnedHierarchy->Play(idle, 0.25);
			GetGroup()->ChangeState(StateType::idle);
		}
	}

	ColliderManager::main->UpdateDynamicCells();
	npc->MoveControl();
}

void NPCGotoShop::Begin(StateType type, const std::shared_ptr<StatePattern>& prevState)
{
	NPCState::Begin(type, prevState);

	std::vector<std::shared_ptr<GameObject>> pathPointObjects;
	std::vector<std::shared_ptr<GameObject>> pathPointObjects2;
	SceneManager::main->GetCurrentScene()->FindsInclude(L"$PathPoint", pathPointObjects);
	std::ranges::copy_if(pathPointObjects, std::back_inserter(pathPointObjects2), [&](const std::shared_ptr<GameObject>& current) {
		return (current->GetName().find(L"Shop") != std::wstring::npos) &&
		current->GetName().find(L"Entry") != std::wstring::npos;
	});
	//Shop
	if (pathPointObjects2.size() != 0)
	{
		auto npc = this->npc.lock();
		auto endPoint = pathPointObjects2[0]->_transform->GetWorldPosition();
		endPoint += Vector3(_random_dist01(_random) * 4 - 2, 0, _random_dist01(_random) * 4 - 2);
		npc->paths = NavMeshManager::main->CalculatePath(npc->GetOwner()->_transform->GetWorldPosition(), endPoint);

		auto animationList = npc->GetOwner()->GetComponentWithChilds<AnimationListComponent>();
		auto skinnedHierarchy = npc->GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

		if (animationList)
		{
			auto walk = animationList->GetAnimations()["walk"];
			auto idle = animationList->GetAnimations()["idle"];
			auto run = animationList->GetAnimations()["run"];
			skinnedHierarchy->Play(walk, 0.25);
		}
	}
}

bool NPCGotoShop::TriggerUpdate()
{
	if ((GetGroup()->currentState->GetType() == StateType::goto_any ||
		GetGroup()->currentState->GetType() == StateType::idle) &&
		Input::main->GetKeyDown(KeyCode::Num8)) // A // InGameMainField::GetMain()->shopOpen
	{
		return true;
	}
	return false;
}

void NPCGotoShop::End(const std::shared_ptr<StatePattern>& nextState)
{
	NPCState::End(nextState);
}

// -------- Goto Table --------

void NPCGotoTable::Init()
{
	NPCState::Init();
}

void NPCGotoTable::Update()
{
	NPCState::Update();


	auto npc = this->npc.lock();

	if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::NavMesh))
	{
		Gizmo::Width(0.2f);
		for (int i = 1; i < (int)npc->paths.size(); ++i) {
			Gizmo::Line(npc->paths[i - 1], npc->paths[i], Vector4(1, 0, 0, 1));
		}
		Gizmo::WidthRollBack();
	}

	Vector3 currentWorldPos = npc->GetOwner()->_transform->GetWorldPosition();
	Vector3 nextPos = npc->AdvanceAlongPath(npc->paths, currentWorldPos, 0.75);
	Vector3 nextDir = Vector2(nextPos.x, nextPos.z) - Vector2(currentWorldPos.x, currentWorldPos.z);
	nextDir.Normalize();
	npc->lookDirection = Vector3(nextDir.x, 0, nextDir.y);

	auto animationList = npc->GetOwner()->GetComponentWithChilds<AnimationListComponent>();
	auto skinnedHierarchy = npc->GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

	if (animationList)
	{
		auto walk = animationList->GetAnimations()["walk"];
		auto idle = animationList->GetAnimations()["idle"];
		auto run = animationList->GetAnimations()["run"];
		auto sit = animationList->GetAnimations()["sitting"];
		auto sitdown = animationList->GetAnimations()["sitdown"];
		auto situp = animationList->GetAnimations()["situp"];

		if (npc->paths.size() != 0)
		{
			auto endPoint = Vector2(npc->paths[npc->paths.size() - 1].x, npc->paths[npc->paths.size() - 1].z);
			if ((endPoint - Vector2(currentWorldPos.x, currentWorldPos.z)).Length() <= 0.20)
			{
				skinnedHierarchy->Play(idle, 0.25);
				GetGroup()->ChangeState(StateType::eat);
			}
			else
			{
				skinnedHierarchy->Play(walk, 0.25);
			}
		}
		else
		{
			skinnedHierarchy->Play(idle, 0.25);
			GetGroup()->ChangeState(StateType::idle);
		}
	}

	ColliderManager::main->UpdateDynamicCells();
	npc->MoveControl();
}

void NPCGotoTable::Begin(StateType type, const std::shared_ptr<StatePattern>& prevState)
{
	NPCState::Begin(type, prevState);

	auto npc = this->npc.lock();


	if (InGameMainField::GetMain()->shopTablePointsPool.size() != 0)
	{
		npc->paths.clear();
		int i = 0;
		auto pointIter = InGameMainField::GetMain()->shopTablePointsPool.begin() + (abs(_random_dist(_random)) % InGameMainField::GetMain()->shopTablePointsPool.size());
		auto point = pointIter->lock();
		
		InGameMainField::GetMain()->shopTablePointsPool.erase(pointIter);

		auto endPoint = point->_transform->GetWorldPosition();
		npc->paths = NavMeshManager::main->CalculatePath(npc->GetOwner()->_transform->GetWorldPosition(), endPoint);

		this->point = point;

		auto animationList = npc->GetOwner()->GetComponentWithChilds<AnimationListComponent>();
		auto skinnedHierarchy = npc->GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

		skinnedHierarchy->_speedMultiple = 1;

		if (animationList)
		{
			auto walk = animationList->GetAnimations()["walk"];
			auto idle = animationList->GetAnimations()["idle"];
			auto run = animationList->GetAnimations()["run"];
			skinnedHierarchy->Play(walk, 0.25);
		}
	}
}

bool NPCGotoTable::TriggerUpdate()
{
	return NPCState::TriggerUpdate();
}

void NPCGotoTable::End(const std::shared_ptr<StatePattern>& nextState)
{
	NPCState::End(nextState);
}

// -------- Eating --------

void NPCEatting::Init()
{
	NPCState::Init();
}

void NPCEatting::Update()
{
	NPCState::Update();


	auto npc = this->npc.lock();

	auto animationList = npc->GetOwner()->GetComponentWithChilds<AnimationListComponent>();
	auto skinnedHierarchy = npc->GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

	if (animationList)
	{
		auto walk = animationList->GetAnimations()["walk"];
		auto idle = animationList->GetAnimations()["idle"];
		auto run = animationList->GetAnimations()["run"];
		auto sit = animationList->GetAnimations()["sitting"];
		auto sitdown = animationList->GetAnimations()["sitdown"];
		auto situp = animationList->GetAnimations()["situp"];


		if (auto point = this->point.lock())
		{
			auto pos = npc->GetOwner()->_transform->GetWorldPosition();
			npc->GetOwner()->_transform->SetWorldPosition(Vector3::Lerp(pos, this->point.lock()->_transform->GetWorldPosition(), std::clamp(_time, 0.0f, 1.0f)));
			auto rot = npc->GetOwner()->_transform->GetWorldRotation();
			npc->GetOwner()->_transform->SetWorldRotation(Quaternion::Slerp(rot, this->point.lock()->_transform->GetWorldRotation(), std::clamp(_time, 0.0f, 1.0f)));
		}

		if (this->isSitBegin && !skinnedHierarchy->IsPlay())
		{
			this->isSitBegin = false;
			this->isSit = true;
			skinnedHierarchy->Play(sit, 0.25);
		}

		if (this->isSit && !InGameMainField::GetMain()->shopOpen)
		{
			this->isSit = false;
			this->isSitEnd = true;
			skinnedHierarchy->Play(situp, 0.25);
		}
		if (this->isSitEnd && !skinnedHierarchy->IsPlay())
		{
			this->isSitEnd = false;
			npc->GetOwner()->_transform->SetWorldPosition(npc->GetOwner()->_transform->GetWorldPosition() - npc->GetOwner()->_transform->GetForward());
			GetGroup()->ChangeState(StateType::goto_shop);
		}
	}
}

void NPCEatting::Begin(StateType type, const std::shared_ptr<StatePattern>& prevState)
{
	NPCState::Begin(type, prevState);

	auto npc = this->npc.lock();

	this->isSitBegin = false;
	this->isSit = false;
	this->isSitEnd = false;

	auto animationList = npc->GetOwner()->GetComponentWithChilds<AnimationListComponent>();
	auto skinnedHierarchy = npc->GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

	if (auto prev = std::dynamic_pointer_cast<NPCGotoTable>(prevState))
	{
		this->point = prev->point;
	}

	if (animationList)
	{
		auto walk = animationList->GetAnimations()["walk"];
		auto idle = animationList->GetAnimations()["idle"];
		auto run = animationList->GetAnimations()["run"];
		auto sit = animationList->GetAnimations()["sitting"];
		auto sitdown = animationList->GetAnimations()["sitdown"];
		auto situp = animationList->GetAnimations()["situp"];

		this->isSitBegin = true;
		this->isSit = false;
		this->isSitEnd = false;
		skinnedHierarchy->Play(sitdown, 0.25);
	}
}

bool NPCEatting::TriggerUpdate()
{
	return NPCState::TriggerUpdate();
}

void NPCEatting::End(const std::shared_ptr<StatePattern>& nextState)
{
	NPCState::End(nextState);
	auto npc = this->npc.lock();
	auto point = this->point.lock();

	if (point)
	{
		InGameMainField::GetMain()->shopTablePointsPool.push_back(point);
	}
	//this->point
}


// -------- IDLE --------

void NPCIdle::Init()
{
	NPCState::Init();
}

void NPCIdle::Update()
{
	NPCState::Update();

	if (waitTime < _time)
	{
		GetGroup()->ChangeState(StateType::goto_any);
	}
}

void NPCIdle::Begin(StateType type, const std::shared_ptr<StatePattern>& prevState)
{
	NPCState::Begin(type, prevState);

	waitTime = _random_dist01(_random) * 10 + 3;
	//GetGroup()->ChangeState(StateType::goto_any);
}

bool NPCIdle::TriggerUpdate()
{
	return NPCState::TriggerUpdate();
}

void NPCIdle::End(const std::shared_ptr<StatePattern>& nextState)
{
	NPCState::End(nextState);
}
