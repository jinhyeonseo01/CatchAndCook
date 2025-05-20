#pragma once
#include "Component.h"

class SeaPlayerController;

class WeaponSystem : public Component
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


public:
	void SetController(SeaPlayerController* controller) { _controller = controller; }

private:
	SeaPlayerController* _controller;

};

