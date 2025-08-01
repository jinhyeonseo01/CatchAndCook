#pragma once
#include "Camera.h"
#include "Component.h"

class CameraComponent : public Component, public Camera
{
public:
	CameraComponent();

	void Update() override;
	~CameraComponent() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current) override;
	void SetDestroy() override;
	void Destroy() override;
	void RenderEnd() override;
	void ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene) override;
	void Reset() override;
};

