#pragma once
#include "Component.h"
#include "IComponentFactory.h"
#include "ObjectSettingComponent.h"


class AnimationListComponent;
class SkinnedHierarchy;
class CameraComponent;



struct ControlInfo
{
	float cameraRotationSmooth = 40;
	float cameraMoveSmooth = 60;

	float moveForce = 0.32f; // speed * drug 수치만큼은 나와야 MAX를 찍음.
	float maxSpeed = 8.0f;
	float drag = 0.04f;
};

const ControlInfo fieldInfo = {
	40,
	60,
	7,
	8,
	0.25,
};

class PlayerController : public Component
{
public:
	std::weak_ptr<CameraComponent> camera;
	std::weak_ptr<SkinnedHierarchy> _skinnedHierarchy;
	std::weak_ptr<AnimationListComponent> _animationList;

	~PlayerController() override;
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


public:
	void SetOnBoard();
	void SetOFFBoard();

public:
	void CameraControl();
	void MoveControl();

private: // Camera Control
	Vector3 _targetOffset;
	Vector3 _currentOffset;

	Vector3 _targetEuler;
	Vector3 _currentEuler;

	Vector3 targetLookWorldDirection;
	Vector3 targetLookWorldDirectionDelayed;

	Quaternion currentLookWorldRotation;

	double currentCameraDistance = 3.6;
	double minCameraDistance = 0.75f;
	double maxCameraDistance = 3.5;

private:
	Vector3 velocity = Vector3::Zero;
	ControlInfo controlInfo = fieldInfo;
	bool isGround = true;
	bool _onBoard = false;
public:
	vec3 _GenPos{};
	Quaternion _GenRotate{};
};

