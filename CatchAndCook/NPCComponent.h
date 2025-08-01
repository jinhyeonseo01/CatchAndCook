#pragma once
#include "StatePattern.h"


class NPCComponent;


class NPCFSMGroup : public StatePatternGroup
{
public:
	void AnyUpdate() override;
	void Init() override;
	void Update() override;


	void SetNPC(const std::shared_ptr<NPCComponent>& npc);
	std::weak_ptr<NPCComponent> npc;
};

class NPCState : public StatePattern
{
public:

	void SetNPC(const std::shared_ptr<NPCComponent>& npc) { this->npc = npc; };

	std::weak_ptr<NPCComponent> npc;
};
class NPCGotoAny : public NPCState
{
public:
	void Init() override;
	void Update() override;
	void Begin(StateType type, const std::shared_ptr<StatePattern>& prevState) override;
	bool TriggerUpdate() override;
	void End(const std::shared_ptr<StatePattern>& nextState) override;

	bool runMove = false;
};

class NPCGotoShop : public NPCState
{
public:
	void Init() override;
	void Update() override;
	void Begin(StateType type, const std::shared_ptr<StatePattern>& prevState) override;
	bool TriggerUpdate() override;
	void End(const std::shared_ptr<StatePattern>& nextState) override;
};

class NPCGotoTable : public NPCState
{
public:
	void Init() override;
	void Update() override;
	void Begin(StateType type, const std::shared_ptr<StatePattern>& prevState) override;
	bool TriggerUpdate() override;
	void End(const std::shared_ptr<StatePattern>& nextState) override;

	std::weak_ptr<GameObject> point;
};

class NPCEatting : public NPCState
{
public:
	void Init() override;
	void Update() override;
	void Begin(StateType type, const std::shared_ptr<StatePattern>& prevState) override;
	bool TriggerUpdate() override;
	void End(const std::shared_ptr<StatePattern>& nextState) override;

	bool isSitBegin = false;
	bool isSit = false;
	bool isSitEnd = false;

	bool isEatting = false;

	std::shared_ptr<GameObject> selectedUI;
	std::shared_ptr<GameObject> selectedCookObj;
	std::weak_ptr<GameObject> point;
};

class NPCIdle : public NPCState
{
public:
	void Init() override;
	void Update() override;
	void Begin(StateType type, const std::shared_ptr<StatePattern>& prevState) override;
	bool TriggerUpdate() override;
	void End(const std::shared_ptr<StatePattern>& nextState) override;

	float waitTime = 0;
};



class NPCComponent : public Component
{
public:
	~NPCComponent() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void MoveControl();
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current) override;
	void SetDestroy() override;
	void Destroy() override;

	Vector3 AdvanceAlongPath(const std::vector<Vector3>& path, const Vector3& worldPos, float t);
	
	std::shared_ptr<NPCFSMGroup> fsm;

	std::vector<Vector3> gotoPoints;
	std::vector<Vector3> paths;
	Vector3 shopEntryPoint;

	std::vector<Vector3> shopTablePoints;
	std::vector<weak_ptr<GameObject>> shopTablePointsPool;

	Vector3 velocity;
	Vector3 lookDirection = Vector3::Forward;
	bool isGround = true;


	bool isEat = false;
};
