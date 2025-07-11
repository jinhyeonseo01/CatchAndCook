#pragma once
#include "Component.h"
class itemBoxComponent : public Component
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

private:
	float _rotate = 0.0f;
	float _orginY = 0.0f;



};

