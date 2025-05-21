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

    
    ComputedRect cr;
    //GetLocalToWorldMatrix_BottomUp(cr.worldMatrix);
    cr.worldMatrix = Matrix::Identity;
    cr.localMatrix = Matrix::Identity;
    if (GetOwner()->GetParent())
        if (auto rect = GetOwner()->GetParent()->GetComponent<RectTransform>())
            _computedRect = ComputeRectTransform(rect->_computedRect);
        else
            _computedRect = ComputeRectTransform({});
    else
        _computedRect = ComputeRectTransform(cr);

	auto min = _computedRect.relativeRect.min;
    auto max = _computedRect.relativeRect.max;
    Vector3 bl(min.x, min.y, 0.0f); // Bottom-Left
    Vector3 tl(min.x, max.y, 0.0f); // Top-Left
    Vector3 tr(max.x, max.y, 0.0f); // Top-Right
    Vector3 br(max.x, min.y, 0.0f); // Bottom-Right
    Vector3 pos(0,0,0); // Bottom-Right
    Vector3::Transform(bl, _computedRect.worldMatrix, bl);
    Vector3::Transform(tl, _computedRect.worldMatrix, tl);
    Vector3::Transform(tr, _computedRect.worldMatrix, tr);
    Vector3::Transform(br, _computedRect.worldMatrix, br);
    Vector3::Transform(pos, _computedRect.worldMatrix, pos);

    // 4개 변을 순서대로 그려준다
    Gizmo::Width(2);
    Gizmo::Sphere({ pos , 10}, Vector4(0,1,0,1));
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
    const Rect2D& parentAbsoluteRect = parent.absoluteRect;
    const Rect2D& parentRelativeRect = parent.relativeRect;
    const Vector3& parentLocalPosition = parent.localPosition;
	const Matrix& parentWorldMatrix = parent.worldMatrix;
    Vector2 parentRectSize = parentAbsoluteRect.max - parentAbsoluteRect.min;

    // 2) 부모 크기, anchor 기준점
    Vector2  anchorMin = _rectTransformData.anchorMin;
    Vector2  anchorMax = _rectTransformData.anchorMax;

    Vector2 parentAnchorAbsolutePosition = parentAbsoluteRect.min + parentRectSize * anchorMin;
    Vector2 parentAnchorRelativePosition = parentRelativeRect.min + parentRectSize * anchorMin;

    Vector2 rectSize = _rectTransformData.sizeDelta;
    Vector2 anchoredPosition(_localPosition.x, _localPosition.y);

    bool stretchX = (anchorMin.x != anchorMax.x);
    bool stretchY = (anchorMin.y != anchorMax.y);

    // 5) offsetMin/offsetMax: stretch 축은 JSON 값, 나머지는 pivot 기반 계산
    Vector2 relativeMin, relativeMax;
	Vector2 offsetMin, offsetMax;
    Vector2 absoluteMin, absoluteMax;
    //  — X 축 (Left/Right)
    if (stretchX) {
        offsetMin.x = _rectTransformData.offsetMin.x;       // Left
        offsetMax.x = _rectTransformData.offsetMax.x;       // Right
        absoluteMin.x = parentAbsoluteRect.min.x + offsetMin.x;
        absoluteMax.x = parentAbsoluteRect.max.x + offsetMax.x;

        float relativeRectMin = parentRelativeRect.min.x + offsetMin.x;
        float relativeRectMax = parentRelativeRect.max.x + offsetMax.x;

        anchoredPosition.x = (relativeRectMax - relativeRectMin) * _rectTransformData.pivot.x + relativeRectMin;
        SetLocalPosition(Vector3(anchoredPosition.x, anchoredPosition.y, _localPosition.z));
        parentAnchorRelativePosition.x = 0;
        relativeMin.x = relativeRectMin -  anchoredPosition.x;
        relativeMax.x = relativeRectMax -  anchoredPosition.x;
    }
    else {
        relativeMin.x = -_rectTransformData.pivot.x * rectSize.x;
        relativeMax.x = (1.0f - _rectTransformData.pivot.x) * rectSize.x;

        offsetMin.x = anchoredPosition.x + relativeMin.x;
        offsetMax.x = anchoredPosition.x + relativeMax.x;
        absoluteMin.x = parentAnchorAbsolutePosition.x + offsetMin.x;
        absoluteMax.x = parentAnchorAbsolutePosition.x + offsetMax.x;
    }
    //  — Y 축 (Bottom/Top)
    if (stretchY) {
        offsetMin.y = _rectTransformData.offsetMin.y;       // Bottom
        offsetMax.y = _rectTransformData.offsetMax.y;       // Top

        absoluteMin.y = parentAbsoluteRect.min.y + offsetMin.y;
        absoluteMax.y = parentAbsoluteRect.max.y + offsetMax.y;
        float relativeRectMin = parentRelativeRect.min.y + offsetMin.y;
        float relativeRectMax = parentRelativeRect.max.y + offsetMax.y;

        anchoredPosition.y = (relativeRectMax - relativeRectMin) * _rectTransformData.pivot.y + relativeRectMin;
        SetLocalPosition(Vector3(anchoredPosition.x, anchoredPosition.y, _localPosition.z));
        parentAnchorRelativePosition.y = 0;
        relativeMin.y = relativeRectMin - anchoredPosition.y;
        relativeMax.y = relativeRectMax - anchoredPosition.y;
    }
    else {
        relativeMin.y = -_rectTransformData.pivot.y * rectSize.y;
        relativeMax.y = (1.0f - _rectTransformData.pivot.y) * rectSize.y;
        offsetMin.y = anchoredPosition.y + relativeMin.y;
        offsetMax.y = anchoredPosition.y + relativeMax.y;
        absoluteMin.y = parentAnchorAbsolutePosition.y + offsetMin.y;
        absoluteMax.y = parentAnchorAbsolutePosition.y + offsetMax.y;
    }


    Rect2D absoluteRect{ absoluteMin, absoluteMax };
    Rect2D relativeRect{ relativeMin, relativeMax };

    Vector3 finalLocalPosition(
        parentAnchorRelativePosition.x + anchoredPosition.x,
        parentAnchorRelativePosition.y + anchoredPosition.y,
        _localPosition.z
    );


    Matrix localMatrix = Matrix::CreateScale(_localScale)
        * Matrix::CreateFromQuaternion(_localRotation)
		* Matrix::CreateTranslation(finalLocalPosition);
    Matrix worldMatrix = localMatrix * parentWorldMatrix;

	_computedRect.localPosition = finalLocalPosition;
	_computedRect.anchoredPosition = anchoredPosition;
	_computedRect.absoluteRect = absoluteRect;
    _computedRect.relativeRect = relativeRect;
	_computedRect.worldMatrix = worldMatrix;
	_computedRect.localMatrix = localMatrix;

    return _computedRect;
}