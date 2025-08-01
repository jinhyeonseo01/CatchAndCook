#pragma once

class InitComponent : public Component
{
public:
	~InitComponent() override;
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

class TagsComponent : public Component
{
public:
	std::vector<std::string> _tagNames;

	void Init() override;
	void Start() override;
};

class ScriptsComponent : public InitComponent
{
public:
	std::vector<std::string> _scriptNames;
	void Init() override;
};