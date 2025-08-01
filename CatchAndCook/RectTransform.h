#pragma once
#include "Transform.h"





struct RectTransformData {
	Vector2    pivot = Vector2(0.5, 0.5);
	Vector2    anchorMin = Vector2(0.5, 0.5);
	Vector2    anchorMax = Vector2(0.5, 0.5);
	Vector2    rectSize = Vector2(0, 0);
	Vector2    paddingMin; // left, bottom
	Vector2    paddingMax; // right, top
};


struct Rect2D {
	Vector2 min;  // 왼쪽-아래
	Vector2 max;  // 오른쪽-위
};
struct Rect3D {
	Vector3 min;  // 왼쪽-아래
	Vector3 max;  // 오른쪽-위
};

// 반환용 구조체
struct ComputedRect {
	Vector3 localPosition;       // Pivot 반영된 3D localPosition
	Vector2 anchoredPosition;
	Rect2D  absoluteRect;      // 계산된 local-space min/max
	Rect2D  relativeRect = { Vector2::Zero, Vector2::Zero };
	Matrix  rectTranslateMatrix = Matrix::Identity; // x,y -1 ~ 1 -> relativeRect
	Matrix  localMatrix = Matrix::Identity;
	Matrix  worldMatrix = Matrix::Identity;      // 누적된 World TRS
};

class RectTransform : public Transform, public RenderCBufferSetter
{
public:
	void RenderEnd() override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current) override;
	void ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene) override;
	~RectTransform() override;
	void SetDestroy() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	bool IsExecuteAble() override;
	void SetData(StructuredBuffer* buffer, Material* material) override;

	bool IsBoundCanvasPos(const Vector2& pos);
	bool IsBoundScreenPos(const Vector2& pos);

	ComputedRect ComputeRectTransform(const ComputedRect& parent);
	static ComputedRect ComputeTransform(const shared_ptr<Transform>& transform, const ComputedRect& parent);
	static void TopDownCompute(const shared_ptr<Transform>& transform, const ComputedRect& parent);
	static shared_ptr<RectTransform> ChangeTransToRectTrans(const shared_ptr<Transform>& transform);

	void SetData(Material* material) override;

	RectTransformData _rectTransformData;
	ComputedRect _computedRect;
};
