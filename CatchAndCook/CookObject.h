#pragma once
#include "GUIItem.h"

class CookObject : public Component
{
public:
	shared_ptr<GameObject> selectedUI;

	ItemData ownItemData;

	int cookType = 0;
	bool onTrigger = false;

	bool HasOwnItem();
	bool PushItemData(const ItemData& item);
	ItemData PopItemData();

	~CookObject() override;
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

