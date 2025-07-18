#pragma once
#include "IDelayDestroy.h"
#include "IGuid.h"

class Collider;

class Component : public IGuid, public IDelayDestroy
{
public:
	Component();
	virtual ~Component() override;
	bool operator<(const Component& other) const;

protected:
	void SetOrder(int order) { _order = order; };
	int GetOrder() const { return _order; };
public:
	void SetOwner(const std::shared_ptr<GameObject>& owner) { this->_owner = owner; };
	std::shared_ptr<GameObject> GetOwner() {return _owner.lock(); };

	bool IsExecuteAble() override;

	virtual void Init(); 
	virtual void Start(); 
	virtual void Update(); 
	virtual void Update2(); 
	virtual void Enable(); 
	virtual void Disable(); 
	virtual void RenderBegin();
	virtual void RenderEnd();
	virtual void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void CollisionEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other);
	virtual void ChangeParent(const std::shared_ptr<GameObject>& prev,const std::shared_ptr<GameObject>& current);
	virtual void ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene);
	virtual void SetDestroy();
	virtual void Destroy();


	virtual void Reset();


private:
	int _order = 0;
	std::weak_ptr<GameObject> _owner;

	friend class RectTransform;
};