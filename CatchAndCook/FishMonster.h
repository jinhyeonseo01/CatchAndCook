#pragma once
#include "Component.h"
#include "PathFinder.h"

class AnimationListComponent;
class SkinnedHierarchy;
class Animation;
class GraphPathFinder;


enum class FishMonsterState
{
	Idle,
	Die,
	Hit,
};

class FishMonster :public Component
{
public:
	FishMonster();
	virtual ~FishMonster();
	virtual void Init();
	virtual void Start();
	virtual void Update();
	virtual void Update2();
	virtual void Enable();
	virtual void Disable();
	virtual void RenderBegin();
	virtual void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current);
	virtual void SetDestroy();
	virtual void Destroy();

public:
	void EventDamage(int damage);

public:


private:
	void UpdateState(float dt);
	void SetState(FishMonsterState state);

private:
	shared_ptr<GraphPathFinder> _pathFinder;
	float _moveSpeed = 100.0f;
	float _hit_to_IdleTime = 0.0f;
public:
	float _hp = 100.0f;

private:
	shared_ptr<SkinnedHierarchy> _skined;
	std::unordered_map<string, std::shared_ptr<Animation>> _animations;
	FishMonsterState _state = FishMonsterState::Idle;
};

