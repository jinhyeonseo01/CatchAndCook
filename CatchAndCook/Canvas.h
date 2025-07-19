#pragma once


enum class CanvasType : int
{
	Overlay,
	World
};

class Canvas : public Component
{
public:
	~Canvas() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void RenderEnd() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current) override;
	void ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene) override;
	void SetDestroy() override;
	void Destroy() override;

	Matrix GetOverlayMatrix();

	CanvasType type = CanvasType::World; // Overlay, World
	Vector2 resolution = Vector2(1920, 1080); // 화면 해상도
	float match = 0.0;
};

