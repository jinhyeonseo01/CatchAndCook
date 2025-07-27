#pragma once
#include "Component.h"

enum fireballColor
{
	yellow,
	red,
	green
};
class FireBall :public Component
{
public:
	FireBall() {};
	~FireBall() override = default;

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

	void SetColor(fireballColor color) { _color = color; }

private:
	fireballColor _color = fireballColor::yellow;
	float _autoDestroyTime = 2.0f;
	float _currTime = 0;
};

