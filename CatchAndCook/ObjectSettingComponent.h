#pragma once


struct ObjectSettingParam
{
	unsigned int o_select = 0; // bool
	Vector4 o_selectColor = Vector4::One;

	unsigned int o_hit = 0; // bool
	float o_hitValue = 0; // value

	Vector4 o_hitColor = Vector4::UnitX;
};

class ObjectSettingComponent : public Component, public RenderStructuredSetter
{
public:
	ObjectSettingParam _objectSettingParam;

	~ObjectSettingComponent() override;
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
	void SetData(StructuredBuffer* buffer, Material* material) override;
	void RenderEnd() override;
};

