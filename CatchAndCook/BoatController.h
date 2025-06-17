#pragma once
#include "Component.h"

class AnimationListComponent;
class SkinnedHierarchy;
class CameraComponent;

class BoatController :public Component
{


public:
	~BoatController() override;
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

private:
	std::weak_ptr<CameraComponent> camera;
	std::weak_ptr<SkinnedHierarchy> _skinnedHierarchy;
	std::weak_ptr<AnimationListComponent> _animationList;
};

