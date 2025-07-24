#pragma once


class GUICookProgress : public Component
{
public:

	float maxTime = 1;
	float time = 0;

	float maxWidth = 0.0f; // 최대 너비

	bool isSuccess = false;
	bool isPlay = false;

	int type = 0;

	void Clear();
	void Play();

	float GetProgress() const { return time / maxTime; }
	void SetMaxTime(float max) { maxTime = std::max(0.001f, max); }
	void SetTime(float t) { time = t; }
	bool IsSuccessed() { return (min(time, maxTime) / maxTime) >= 1; }

	std::shared_ptr<RectTransform> progressBar;

	~GUICookProgress() override;
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
};

