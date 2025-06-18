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

	void SetOnBaord();


private:
	Quaternion CalCulateYawPitchRoll();

public:
	static float heightOffset;
	static float SpringArmLength;
	static float  _yaw;
	static float _pitch;
	static float _roll;
private:




	bool _onBoard = false;



	shared_ptr<Camera> _camera;
	shared_ptr<SkinnedHierarchy> _skined;
	std::weak_ptr<AnimationListComponent> _animationList;
};

