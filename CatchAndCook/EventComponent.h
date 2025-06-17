#pragma once
#include "Component.h"


class EventComponent :public Component
{
public:

	virtual void Init();
	virtual void Start();
	virtual void Update();
	virtual void Update2();
	virtual void Enable();
	virtual void Disable();
	virtual void RenderBegin();
	virtual void RenderEnd();
	virtual void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current);
	virtual void ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene);
	virtual void SetDestroy() override;
	virtual void Destroy();

public:
	void SetOneTimeEventCall(const bool& call) { _oneTimeEventCall = call; }
public:
	void BindOnUpdateEvent(const std::function<void()>& func) { _OnUpdateEvents.push_back(func); }
	void BindOnCollisionBegin(const std::function<void()>& func) { _OnCollisionBegins.push_back(func); }
	void BindOnCollisionEnd(const std::function<void()>& func) { _OnCollisionEnds.push_back(func); }

private:
	vector<std::function<void()>> _OnUpdateEvents;
	vector<std::function<void()>> _OnCollisionBegins;
	vector<std::function<void()>> _OnCollisionEnds;

	bool _Oncollision=false;
	bool _oneTimeEventCall = true;
};

