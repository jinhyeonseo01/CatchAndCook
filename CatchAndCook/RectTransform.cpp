#include "pch.h"
#include "RectTransform.h"

#include "Gizmo.h"


COMPONENT(RectTransform)

void RectTransform::RenderEnd()
{
	Transform::RenderEnd();
}

void RectTransform::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Transform::ChangeParent(prev, current);
}

void RectTransform::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
	Transform::ChangeScene(currentScene, nextScene);
}

RectTransform::~RectTransform()
{
}

void RectTransform::SetDestroy()
{
	Transform::SetDestroy();
}

void RectTransform::Init()
{
	Transform::Init();
}

void RectTransform::Start()
{
	Transform::Start();
}

void RectTransform::Update()
{
	Transform::Update();
}

void RectTransform::Update2()
{
	Transform::Update2();

    if (GetOwner()->GetParent())
	    if (auto rect = GetOwner()->GetParent()->GetComponent<RectTransform>())
			ComputeRectTransform(rect->_computedRect);
        else
            ComputeRectTransform({});
    else
        ComputeRectTransform({});

	auto min = _computedRect.localRect2D.min;
    auto max = _computedRect.localRect2D.max;
    Vector3 bl(min.x, min.y, 0.0f); // Bottom-Left
    Vector3 tl(min.x, max.y, 0.0f); // Top-Left
    Vector3 tr(max.x, max.y, 0.0f); // Top-Right
    Vector3 br(max.x, min.y, 0.0f); // Bottom-Right

    // 4개 변을 순서대로 그려준다
    Gizmo::Width(1);
	Gizmo::Line(bl, tl);
    Gizmo::Line(tl, tr);
    Gizmo::Line(tr, br);
    Gizmo::Line(br, bl);
    Gizmo::WidthRollBack();
}

void RectTransform::Enable()
{
	Transform::Enable();
}

void RectTransform::Disable()
{
	Transform::Disable();
}

void RectTransform::Destroy()
{
	Transform::Destroy();
}

void RectTransform::RenderBegin()
{
	Transform::RenderBegin();
}

void RectTransform::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Transform::CollisionBegin(collider, other);
}

void RectTransform::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Transform::CollisionEnd(collider, other);
}

bool RectTransform::IsExecuteAble()
{
	return Transform::IsExecuteAble();
}

void RectTransform::SetData(StructuredBuffer* buffer, Material* material)
{
	Transform::SetData(buffer, material);
}

ComputedRect RectTransform::ComputeRectTransform(const ComputedRect& parent)
{
    // 1) 부모 Rect & WorldMatrix
    const Rect2D& parentRect = parent.localRect2D;
    const Matrix& parentWorld = parent.worldMatrix;

    // 2) 부모 크기, anchor 기준점
    Vector2 parentSize = parentRect.max - parentRect.min;
    Vector2 anchorRef = parentRect.min + parentSize * _rectTransformData.anchorMin;

    // 3) 요소 크기 = anchor 영역 크기 + sizeDelta
    Vector2 anchorArea = parentSize * (_rectTransformData.anchorMax - _rectTransformData.anchorMin);
    Vector2 elementSize = anchorArea + _rectTransformData.sizeDelta;

    // 4) anchoredPosition 은 localPosition.x,y 로부터
    Vector2 anchoredPos(_localPosition.x, _localPosition.y);

    // 5) offsetMin/offsetMax: stretch 축은 JSON 값, 나머지는 pivot 기반 계산
    Vector2 offsetMin, offsetMax;
    //  — X 축 (Left/Right)
    if (_rectTransformData.anchorMin.x != _rectTransformData.anchorMax.x) {
        offsetMin.x = _rectTransformData.offsetMin.x;       // Left
        offsetMax.x = _rectTransformData.offsetMax.x;       // Right
    }
    else {
        offsetMin.x = anchoredPos.x - _rectTransformData.pivot.x * elementSize.x;
        offsetMax.x = anchoredPos.x + (1.0f - _rectTransformData.pivot.x) * elementSize.x;
    }
    //  — Y 축 (Bottom/Top)
    if (_rectTransformData.anchorMin.y != _rectTransformData.anchorMax.y) {
        offsetMin.y = _rectTransformData.offsetMin.y;       // Bottom
        offsetMax.y = _rectTransformData.offsetMax.y;       // Top
    }
    else {
        offsetMin.y = anchoredPos.y - _rectTransformData.pivot.y * elementSize.y;
        offsetMax.y = anchoredPos.y + (1.0f - _rectTransformData.pivot.y) * elementSize.y;
    }

    // 6) local-space Rect(min/max)
    Vector2 localMin2D, localMax2D;
    //  — X축
    if (_rectTransformData.anchorMin.x != _rectTransformData.anchorMax.x) {
        localMin2D.x = parentRect.min.x + offsetMin.x;
        localMax2D.x = parentRect.max.x + offsetMax.x;
    }
    else {
        localMin2D.x = anchorRef.x + offsetMin.x;
        localMax2D.x = anchorRef.x + offsetMax.x;
    }
    //  — Y축
    if (_rectTransformData.anchorMin.y != _rectTransformData.anchorMax.y) {
        localMin2D.y = parentRect.min.y + offsetMin.y;
        localMax2D.y = parentRect.max.y + offsetMax.y;
    }
    else {
        localMin2D.y = anchorRef.y + offsetMin.y;
        localMax2D.y = anchorRef.y + offsetMax.y;
    }
    Rect2D localRect{ localMin2D, localMax2D };

    // 7) pivot 반영 localPosition 재계산 (X,Y) 및 Z 유지
    Vector2 anchoredCalc = offsetMin + _rectTransformData.pivot * elementSize;
    Vector3 localPos3D(
        anchoredCalc.x,
        anchoredCalc.y,
        _localPosition.z
    );

    // 8) 3D TRS 행렬 & 누적 World 행렬
    Matrix trs = Matrix::CreateScale(_localScale)
        * Matrix::CreateFromQuaternion(_localRotation)
        * Matrix::CreateTranslation(localPos3D);
    Matrix worldMat = trs * parentWorld;

    // 9) local-space Rect 코너 → world-space AABB
    Vector3 corners[4] = {
        { localMin2D.x, localMin2D.y, 0.0f },
        { localMin2D.x, localMax2D.y, 0.0f },
        { localMax2D.x, localMin2D.y, 0.0f },
        { localMax2D.x, localMax2D.y, 0.0f }
    };
    Vector2 worldMin(+FLT_MAX, +FLT_MAX), worldMax(-FLT_MAX, -FLT_MAX);
    for (auto& c : corners) {
        Vector3 w = Vector3::Transform(c, worldMat);
        worldMin.x = std::min(worldMin.x, w.x);
        worldMin.y = std::min(worldMin.y, w.y);
        worldMax.x = std::max(worldMax.x, w.x);
        worldMax.y = std::max(worldMax.y, w.y);
    }
    Rect2D worldRect{ worldMin, worldMax };

    // 10) 저장 & 리턴
    _computedRect = {
        localPos3D,
        anchoredCalc,
        localRect,
        worldRect,
        worldMat
    };
    return _computedRect;
}