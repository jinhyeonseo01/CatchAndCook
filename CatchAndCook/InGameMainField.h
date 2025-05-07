﻿#pragma once


class InGameMainField : public Component
{
	static InGameMainField* main;
public:

	bool shopOpen = false;
	std::vector<std::weak_ptr<GameObject>> shopTablePointsPool;


	static std::shared_ptr<InGameMainField> GetMain() {
		if (main == nullptr)
			return nullptr;
		return main->GetCast<InGameMainField>();
	}

	~InGameMainField() override;
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

