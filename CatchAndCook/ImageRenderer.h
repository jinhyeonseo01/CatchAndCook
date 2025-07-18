#pragma once
#include "GUISprite.h"

class ImageRenderer : public Component, public RenderCBufferSetter
{
public:
	~ImageRenderer() override;
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


	void SetSprite(const std::shared_ptr<GUISprite>& sprite) { _sprite = sprite; }
	std::shared_ptr<GUISprite> GetSprite() const { return _sprite; }
	void SetData(Material* material) override;

	std::shared_ptr<Material> _material;

private:
	std::shared_ptr<GUISprite> _sprite;
};

