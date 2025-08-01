#pragma once


class ItemData
{
public:
    int itemCode = -1;
    int itemCookType = -1;

    // 기본 생성자
    ItemData() = default;

    // (명시적) 값 초기화 생성자
    ItemData(int code, int cookType) noexcept
        : itemCode(code), itemCookType(cookType)
    {
    }

    // 복사 생성자
    ItemData(const ItemData&) noexcept = default;
    // 이동 생성자
    ItemData(ItemData&&) noexcept = default;

    // 복사 대입 연산자
    ItemData& operator=(const ItemData&) noexcept = default;
    // 이동 대입 연산자
    ItemData& operator=(ItemData&&) noexcept = default;

    // 소멸자
    ~ItemData() = default;

    // 비교 연산자: C++20 스페이스십 연산자 사용 예시
    auto operator<=>(const ItemData&) const noexcept = default;
    bool operator==(const ItemData&) const noexcept = default;

    // Clear() 메서드는 그대로 두거나 noexcept 붙여도 좋습니다.
    void Clear() noexcept
    {
        itemCode = -1;
        itemCookType = -1;
    }
};


class GUIItem : public Component
{
public:
	std::vector<std::shared_ptr<GameObject>> _itemIcons;
	std::vector<std::shared_ptr<GameObject>> _effectIcons;

	ItemData _itemData;

	bool PushItemData(const ItemData& itemData);
	ItemData PopItemData();
	ItemData GetItemData();

	~GUIItem() override;
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

