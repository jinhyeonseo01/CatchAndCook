#pragma once
#include "GUIItem.h"


class GUIInventory : public Component
{
public:
	static constexpr int InventoryCount = 4;
	static shared_ptr<GUIInventory> main;
	std::vector<std::shared_ptr<GameObject>> _slots;

	std::array<ItemData, InventoryCount> _itemList;

	int selectIndex = 0;

	int HasEmptySlot();

	bool PushItemData(const ItemData& itemData);
	bool SetItemData(const ItemData& itemData, int index);
	ItemData PopItemDataIndex(int index);
	void SlotUpdate();

	~GUIInventory() override;
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

