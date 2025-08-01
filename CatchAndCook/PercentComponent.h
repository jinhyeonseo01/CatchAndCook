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
	 void Init() override;
	 void Start() override;
	 void Update()override ;
	 void Update2()override;
	 void Enable()override;
	 void Disable()override;
	 void RenderBegin()override;
	 void RenderEnd()override;
	 void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)override;
	 void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)override;
	 void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)override;
	 void ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)override;
	 void SetDestroy() override;
	 void Destroy()override;
	 void Reset() override;


public:
	virtual void SetData(StructuredBuffer* buffer = nullptr, Material* material = nullptr);
public:
	void BindingPercentage(float* ptr) { _percentagePtr = ptr;  _displayPercentage = *ptr-0.0001f; }

private:
	float _displayPercentage = 100.0f;
	float* _percentagePtr = nullptr;


};

