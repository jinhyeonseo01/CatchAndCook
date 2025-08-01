#pragma once
#include "Component.h"
#include "RendererBase.h"

struct PlantInfo
{
	float amplitude = 0.2f;
	float frequency = 0.5f;
	float boundsCenterY;
	float boundsSizeY;

	int id;
	float p1;
	float p2;
	float p3;
};

class PlantComponent :public Component, public RenderStructuredSetter
{
public:
	PlantComponent();
	virtual ~PlantComponent() override;
	virtual void Init() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void Update2() override;
	virtual void Enable() override;
	virtual void Disable() override;
	virtual void RenderBegin() override;
	virtual void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	bool IsExecuteAble() override;
	void SetDestroy() override;

	virtual void SetData(StructuredBuffer* buffer = nullptr ,Material* material=nullptr);
private:
	weak_ptr<RendererBase> _renderBase;
	PlantInfo _plantInfo;
	static int idGenator;
};

