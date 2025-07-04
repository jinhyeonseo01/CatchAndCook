#pragma once
#include "Component.h"
#include "GraphData.h"
#include "PathFinder.h"
#include "RendererBase.h"

class GraphPathFinder : public Component, public RenderStructuredSetter
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
	virtual void SetData(StructuredBuffer* buffer = nullptr, Material* material = nullptr);


public:
	void SetAutoPliotMode(bool mode){_autoPliot=mode;}
	void CalculatePath(float speed);

private:
	bool _autoPliot = false;
	float _speed{};
	FishInfo _info;
	LeftRight _leftRight;
	int _currentTargetIndex{};
	vec3 GetRamdomTarget();


};

