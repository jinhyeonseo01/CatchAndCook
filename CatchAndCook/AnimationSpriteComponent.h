#pragma once
#include "Component.h"
class AnimationSpriteComponent : public Component, public RenderCBufferSetter
{
public:

	virtual void Init();
	virtual void Start();
	virtual void Update();
	virtual void Update2();
	virtual void Enable();
	virtual void Disable();
	virtual void RenderBegin();
	virtual void RenderEnd();
	virtual void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current);
	virtual void ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene);
	virtual void SetDestroy() override;
	virtual void Destroy();

public:
	virtual void SetData(Material* material = nullptr);
	void SetFrameRate(float frameRate) { _frameRate = frameRate; }
	void SetTextures(vector<shared_ptr<Texture>>& textures);
	void Reset();

private:
	bool _roop = false;

	unordered_map<int, shared_ptr<Texture>> _spriteImages;

	float _frameRate{ 1.0f }; //  애니메이션 진행 속도
	float _currentTime{}; //현재 애니메이션 진행 시간
	int32 _currentFrameIndex{}; // 현재 애니메이션 인덱스
	int32 _maxFrameIndex = 0; // 최대 애니메이션 프레임
};

