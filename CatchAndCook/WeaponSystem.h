#pragma once
#include "Component.h"
#include "RendererBase.h"
class SeaPlayerController;

struct HookPos
{

	vec3 pos1;
	float padding;
	vec3 pos2;
	float padding2;

};

class WeaponSystem : public Component , public RenderCBufferSetter
{

public:

	WeaponSystem();
	~WeaponSystem() override;
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
	void SetDestroy() override;
	void Destroy() override;

	virtual void SetData(Material* material = nullptr);


public:
	void SetController(SeaPlayerController* controller) { _controller = controller; }

private:
	SeaPlayerController* _controller;
	HookPos _hookPos;


};

