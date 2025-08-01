#pragma once
class CookMargeObject : public Component
{
public:
	static const int slotCount = 5; // 슬롯 개수
	std::vector<std::shared_ptr<GameObject>> _slots;

	std::array<ItemData, slotCount> itemDatas;
	shared_ptr<GameObject> selectedUI;

	std::shared_ptr<GameObject> _CookUI_CookMarge;

	int cookType = 0;
	bool onTrigger = false;

	bool HasOwnSlot();
	bool HasEmptySlot();
	int GetEmptySlotIndex();

	bool PushItemData(const ItemData& itemData);
	bool SetItemData(const ItemData& itemData, int index);
	ItemData GetItemDataIndex(int index);
	ItemData PopItemDataIndex(int index);
	void SlotUpdate();

	~CookMargeObject() override;
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

private:
	vec3 _particleGenPos{};
};

