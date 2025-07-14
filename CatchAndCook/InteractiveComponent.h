#pragma once
#include "Component.h"

class SeaPlayerController;

enum class InteractiveState
{
	NONE,
	ONCOLLISION,
	GAMEON,
	SUCCESS
};

class InteractiveComponent : public Component
{
public:
	void Init() override;
	void Start()override;
	void Update()override;
	void Update2()override;
	void Enable()override;
	void Disable()override;
	void RenderBegin()override;
	void RenderEnd()override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)override;
	void ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)override;
	void SetDestroy()override;
	void Destroy()override;

private:

	void UpdateState();
	void SetState(InteractiveState state);

private:
	InteractiveState _state = InteractiveState::NONE;
	shared_ptr<SeaPlayerController> _seaPlayerController;
	weak_ptr<GameObject> _text;

};

