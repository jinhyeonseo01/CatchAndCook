#pragma once
#include "Component.h"

struct SeaGrassParam
{
	float amplitude;
	float frequency;
	float boundsCenterY;
	float boundsSizeY;
	float PushPower;
	float padding1;
	float padding2;
	float padding3;

};

class SeaGrassComponent : public Component , public RenderCBufferSetter
{

public:
	SeaGrassComponent();
	~SeaGrassComponent() override;

	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current) override;
	void SetDestroy() override;
	void Destroy() override;

	virtual void SetData(Material* material = nullptr);
private:
	SeaGrassParam _param;
	weak_ptr<RendererBase> _renderBase;
};

