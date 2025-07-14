#include "pch.h"
#include "RectTransform.h"

#include "Camera.h"
#include "CameraManager.h"
#include "Canvas.h"
#include "Gizmo.h"
#include "ImageRenderer.h"
#include "MeshRenderer.h"


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
    if (auto renderer = GetOwner()->GetComponent<MeshRenderer>())
        renderer->AddCbufferSetter(GetCast<RectTransform>());
}

void RectTransform::Update()
{
	Transform::Update();
}

void RectTransform::Update2()
{
	Transform::Update2();

    if (GetOwner()->GetParent())
    {
        if (auto rect = GetOwner()->GetParent()->GetComponent<RectTransform>())
        {
            _computedRect = ComputeRectTransform(rect->_computedRect);
        }
        else if (auto rect = GetOwner()->GetParent()->GetComponent<Transform>())
        {
            auto rect2 = ChangeTransToRectTrans(rect);
            _computedRect = ComputeRectTransform(rect2->_computedRect);
        }
        else
            _computedRect = ComputeRectTransform({});
    }
    else
    {
        _computedRect = ComputeRectTransform({});
        if (auto canvas = GetOwner()->GetComponent<Canvas>())
        {
            
			_computedRect.worldMatrix =
            //Matrix::CreateTranslation(Vector3(0,0,1))
            CameraManager::main->GetActiveCamera()->_params.InvertViewMatrix
			//* Matrix::CreateScale(Vector3(1/canvas->resolution.x, 1 / canvas->resolution.y, 0))
        	* _computedRect.worldMatrix;
        	
	        //canvas->
        }
    }


    //auto worldPos = Vector3::Transform(Vector3::Zero, _computedRect.worldMatrix);

	if (auto render = GetOwner()->GetRenderer())
	{
		render->order = GetOrder();
        //render->SetBound(render->GetBound())
	}
    //worldPos

    /*
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
    Gizmo::Width(0.02);
	Gizmo::Line(bl, tl, Vector4(0, 0, 0, 1));
    Gizmo::Line(tl, tr, Vector4(0, 0, 0, 1));
    Gizmo::Line(tr, br, Vector4(0, 0, 0, 1));
    Gizmo::Line(br, bl, Vector4(0, 0, 0, 1));
    Gizmo::WidthRollBack();
	*/
}

shared_ptr<RectTransform> RectTransform::ChangeTransToRectTrans(const shared_ptr<Transform>& transform)
{
    auto gameObject = transform->GetOwner();
    auto prevTransform = transform;
    auto newRectTrans = std::make_shared<RectTransform>();

    newRectTrans->SetLocalPosition(prevTransform->GetLocalPosition());
    newRectTrans->SetLocalScale(prevTransform->GetLocalScale());
    newRectTrans->SetLocalRotation(prevTransform->GetLocalRotation());
	newRectTrans->_owner = gameObject;
    newRectTrans->_first = false;

    gameObject->_transform = newRectTrans;
    for (auto& comp : gameObject->_components)
    {
        if (auto trans = std::dynamic_pointer_cast<Transform>(comp))
        {
            comp = newRectTrans;
            break;
        }
    }
    return newRectTrans;
}

void RectTransform::SetData(Material* material)
{
    auto min = _computedRect.relativeRect.min;
    auto max = _computedRect.relativeRect.max;
    Vector3 bl(min.x, min.y, 0.0f); // Bottom-Left
    Vector3 tl(min.x, max.y, 0.0f); // Top-Left
    Vector3 tr(max.x, max.y, 0.0f); // Top-Right
    Vector3 br(max.x, min.y, 0.0f); // Bottom-Right
    Vector3 pos(0, 0, 0); // Bottom-Right
    Vector3::Transform(bl, _computedRect.worldMatrix, bl);
    Vector3::Transform(tl, _computedRect.worldMatrix, tl);
    Vector3::Transform(tr, _computedRect.worldMatrix, tr);
    Vector3::Transform(br, _computedRect.worldMatrix, br);
    Vector3::Transform(pos, _computedRect.worldMatrix, pos);


    auto _rectCBuffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::RectTransformParam)->Alloc(1);
	RectTransformParam rectParam;
    rectParam.localToWorld = _computedRect.worldMatrix;
	rectParam.worldToLocal = _computedRect.worldMatrix.Invert();
    rectParam.normalizeToLocal = _computedRect.rectTranslateMatrix;
	rectParam.worldPos = Vector3::Transform(Vector3::Zero, _computedRect.worldMatrix);
    memcpy(_rectCBuffer->ptr, &rectParam, sizeof(rectParam));

    int index = material->GetShader()->GetRegisterIndex("RectTransformParam");
    if (index != -1)
        Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, _rectCBuffer->GPUAdress);
}

void RectTransform::TopDownCompute(const shared_ptr<Transform>& transform, const ComputedRect& parent)
{
    auto obj = transform->GetOwner();
    auto rectTrans = std::dynamic_pointer_cast<RectTransform>(transform);
    ComputedRect current;
    if (rectTrans)
        current = rectTrans->ComputeRectTransform(parent);
    else
        current = ComputeTransform(transform, parent);
    
    for (auto& c : obj->_childs)
    {
        if (auto rect = c.lock()->GetComponent<Transform>())
        {
            TopDownCompute(rect, current);
        }
    }
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

    if (auto renderer = GetOwner()->GetComponent<MeshRenderer>())
		renderer->RemoveCbufferSetter(GetCast<RectTransform>());
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

ComputedRect RectTransform::ComputeTransform(const shared_ptr<Transform>& transform, const ComputedRect& parent)
{
	ComputedRect current;
    Matrix localMatrix;
	transform->GetLocalSRTMatrix(localMatrix);
	current.localMatrix = localMatrix;
	current.worldMatrix = localMatrix * parent.worldMatrix;
	current.localPosition = transform->GetLocalPosition();
	current.rectTranslateMatrix = Matrix::Identity;
	current.anchoredPosition = Vector2(current.localPosition.x, current.localPosition.y);

    return current;
}

ComputedRect RectTransform::ComputeRectTransform(const ComputedRect& parent)
{
    // 1) 부모 Rect & WorldMatrix
    const Rect2D& parentAbsoluteRect = parent.absoluteRect;
    const Rect2D& parentRelativeRect = parent.relativeRect;
    const Vector3& parentLocalPosition = parent.localPosition;
	const Matrix& parentWorldMatrix = parent.worldMatrix;
    Vector2 parentRectSize = parentRelativeRect.max - parentRelativeRect.min;

    // 2) 부모 크기, anchor 기준점
    Vector2  anchorMin = _rectTransformData.anchorMin;
    Vector2  anchorMax = _rectTransformData.anchorMax;

    Vector2 parentAnchorAbsolutePosition = parentAbsoluteRect.min + parentRectSize * anchorMin;
    Vector2 parentAnchorRelativePosition = parentRelativeRect.min + parentRectSize * anchorMin;

    Vector2 rectSize = _rectTransformData.rectSize;
    Vector2 anchoredPosition(_localPosition.x, _localPosition.y);

    bool stretchX = (anchorMin.x != anchorMax.x);
    bool stretchY = (anchorMin.y != anchorMax.y);

    // 5) paddingMin/paddingMax: stretch 축은 JSON 값, 나머지는 pivot 기반 계산
    Vector2 relativeMin, relativeMax;
	Vector2 offsetMin, offsetMax;
    Vector2 absoluteMin, absoluteMax;
    //  — X 축 (Left/Right)
    if (stretchX) {
        offsetMin.x = _rectTransformData.paddingMin.x;       // Left
        offsetMax.x = _rectTransformData.paddingMax.x;       // Right
        absoluteMin.x = parentAbsoluteRect.min.x + offsetMin.x;
        absoluteMax.x = parentAbsoluteRect.max.x + offsetMax.x;

        float paddingRectMin = parentRelativeRect.min.x + offsetMin.x;
        float paddingRectMax = parentRelativeRect.max.x + offsetMax.x;

        anchoredPosition.x = (paddingRectMax - paddingRectMin) * _rectTransformData.pivot.x + paddingRectMin;
        SetLocalPosition(Vector3(anchoredPosition.x, anchoredPosition.y, _localPosition.z));
        parentAnchorRelativePosition.x = 0;
        relativeMin.x = paddingRectMin -  anchoredPosition.x;
        relativeMax.x = paddingRectMax -  anchoredPosition.x;
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
        offsetMin.y = _rectTransformData.paddingMin.y;       // Bottom
        offsetMax.y = _rectTransformData.paddingMax.y;       // Top

        absoluteMin.y = parentAbsoluteRect.min.y + offsetMin.y;
        absoluteMax.y = parentAbsoluteRect.max.y + offsetMax.y;
        float paddingRectMin = parentRelativeRect.min.y + offsetMin.y;
        float paddingRectMax = parentRelativeRect.max.y + offsetMax.y;

        anchoredPosition.y = (paddingRectMax - paddingRectMin) * _rectTransformData.pivot.y + paddingRectMin;
        SetLocalPosition(Vector3(anchoredPosition.x, anchoredPosition.y, _localPosition.z));
        parentAnchorRelativePosition.y = 0;
        relativeMin.y = paddingRectMin - anchoredPosition.y;
        relativeMax.y = paddingRectMax - anchoredPosition.y;
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
    Vector2 rectHalfSize = (relativeRect.max - relativeRect.min) / 2.0f;
    Vector2 rectCenter = relativeRect.min + rectHalfSize;


    Matrix localMatrix = Matrix::CreateScale(_localScale) * Matrix::CreateFromQuaternion(_localRotation) * Matrix::CreateTranslation(finalLocalPosition);
    Matrix worldMatrix = localMatrix * parentWorldMatrix;

	_computedRect.localPosition = finalLocalPosition;
	_computedRect.anchoredPosition = anchoredPosition;
	_computedRect.absoluteRect = absoluteRect;
    _computedRect.relativeRect = relativeRect;
	_computedRect.worldMatrix = worldMatrix;
	_computedRect.localMatrix = localMatrix;
    _computedRect.rectTranslateMatrix = Matrix::CreateScale(rectHalfSize.x, rectHalfSize.y, 1.0f) * Matrix::CreateTranslation(rectCenter.x, rectCenter.y, 0.0f);
    
    return _computedRect;
}

