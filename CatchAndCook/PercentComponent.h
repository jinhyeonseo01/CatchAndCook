#pragma once
#include "Component.h"
#include "RendererBase.h"

struct PercentageData
{
	float percentage;
	float p1;
	float p2;
	float p3;
};

class PercentComponent :public Component , public  RenderStructuredSetter
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
	virtual void SetData(StructuredBuffer* buffer = nullptr, Material* material = nullptr);
public:
	void BindingPercentage(float* ptr) { _percentagePtr = ptr;  _displayPercentage = *ptr; }

private:
	float _displayPercentage{};
	float* _percentagePtr = nullptr;


};

