#pragma once


class AnimationListComponent : public Component
{
public:
	std::unordered_map<string, string> _animationKeys;
	std::unordered_map<string, std::shared_ptr<Animation>> _animations;

	std::unordered_map<string, std::shared_ptr<Animation>>& GetAnimations() 
	{
		return _animations;
	}
	
	~AnimationListComponent() override;
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
};

