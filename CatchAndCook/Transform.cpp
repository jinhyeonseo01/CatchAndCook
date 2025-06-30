#include "pch.h"
#include "Transform.h"
#include "Core.h"
#include "GameObject.h"
#include "simple_mesh_ext.h"
#include "BufferPool.h"
#include "BufferManager.h"
#include "Gizmo.h"
#include "RendererBase.h"
#include "TerrainManager.h"
#include "Terrain.h"
// SCENE -> 트리구조 / MATERIAL CAMERA MESHRENDER SCRIPTS LIGHT  / RENDERPASS / MODEL / RESOURCEMANAGER / INSTANCING
// 1.12 전까지 엔진구조완성.

void Transform::SetDestroy()
{
	Component::SetDestroy();
}

Transform::Transform()
{
}

Transform::~Transform()
{

}

void Transform::Init()
{
	Component::Init();

}

void Transform::Start()
{
	Component::Start();

    if (GetOwner()->GetRenderer())
    {
        GetOwner()->GetRenderer()->AddStructuredSetter(static_pointer_cast<Transform>(shared_from_this()), BufferType::TransformParam);
    }


 
}

void Transform::Update()
{
	Component::Update();

   
}

void Transform::Update2()
{
	Component::Update2();
}

void Transform::Enable()
{
	Component::Enable();

  
}

void Transform::Disable()
{
	Component::Disable();


}

void Transform::Destroy()
{
    Component::Destroy();
}

void Transform::RenderBegin()
{
	Component::RenderBegin();

    if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::WorldPivot))
    {
        Gizmo::Width(0.02f);
        auto o = GetWorldPosition();
        auto f = GetForward();
        auto u = GetUp();
        auto r = GetRight();
        Gizmo::Line(o, o + f, Vector4(0, 0, 1, 1));
        Gizmo::Line(o, o + u, Vector4(0, 1, 0, 1));
        Gizmo::Line(o, o + r, Vector4(1, 0, 0, 1));
    }  

    _isLocalSRTChanged = false;
    _isLocalToWorldChanged = false;
}

void Transform::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void Transform::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider,other);
}

bool Transform::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void Transform::SetData(StructuredBuffer* buffer, Material* material)
{
    TransformParam transParam;
    transParam.localToWorld = _localToWorldMatrix;
    transParam.localToWorld.Invert(transParam.worldToLocal);
    transParam.worldPos = transParam.localToWorld.Translation();
    buffer->AddData(transParam);
}




vec3 Transform::SetForward(const vec3& dir)
{
    _forward = dir;
    _forward.Normalize(_forward);
    _right = _up.Cross(_forward);
    _up = _forward.Cross(_right);
    SetWorldRotation(Quaternion::CreateFromRotationMatrix(Matrix(_right, _up, _forward)));
    return _forward;

};

vec3 Transform::SetUp(const vec3& dir)
{

    _up = dir;
    _up.Normalize();
    _right = _up.Cross(_forward);
    _forward = _right.Cross(_up);
    SetWorldRotation(Quaternion::CreateFromRotationMatrix(Matrix(_right, _up, _forward)));
    return _up;

};

vec3 Transform::SetRight(const vec3& dir)
{
    _right = dir;
    _right.Normalize(_right);
    _forward = _right.Cross(_up);
    _up = _forward.Cross(_right);
    SetWorldRotation(Quaternion::CreateFromRotationMatrix(Matrix(_right, _up, _forward)));
    return _right;
};

vec3 Transform::GetForward()
{
    Quaternion quat = GetWorldRotation();
    _forward = vec3::Transform(vec3(0, 0, 1), quat);
    _forward.Normalize();
    return _forward;
}

vec3 Transform::GetUp()
{
    Quaternion quat = GetWorldRotation();
    _up = vec3::Transform(vec3(0, 1, 0), quat);
    return _up;
}

vec3 Transform::GetRight()
{
    Quaternion quat = GetWorldRotation();
    _right = vec3::Transform(vec3(1, 0, 0), quat);
    return _right;
}

vec3 Transform::GetLocalEuler() const
{
    return _localRotation.ToEuler();
}

const vec3& Transform::SetLocalRotation(const vec3& euler)
{
    _localRotation = Quaternion::CreateFromYawPitchRoll(euler);
    _needLocalSRTUpdated = true;
    return euler;
}

vec3& Transform::GetLocalPosition() 
{
    return _localPosition;
}

const vec3& Transform::SetLocalPosition(const vec3& worldPos)
{
    _needLocalSRTUpdated = true;
    return _localPosition = worldPos;
}

vec3 Transform::GetLocalScale() const
{
    return _localScale;
}

const vec3& Transform::SetLocalScale(const vec3& worldScale)
{
    _needLocalSRTUpdated = true;
    return _localScale = worldScale;
}

Quaternion Transform::GetLocalRotation() const
{
    return _localRotation;
}

const Quaternion& Transform::SetLocalRotation(const Quaternion& quaternion)
{
    _needLocalSRTUpdated = true;
    return _localRotation = quaternion;
}

vec3 Transform::GetWorldPosition()
{
    auto parent = GetOwner();
    if (parent)
        parent = parent->parent.lock();

    if (parent)
    {
        Matrix mat;
        parent->_transform->GetLocalToWorldMatrix_BottomUp(mat);
        return vec3::Transform(_localPosition, mat);
    }

    return _localPosition;
}

const vec3& Transform::SetWorldPosition(const vec3& worldPos)
{
    auto parent = GetOwner();
    if (parent)
        parent = parent->parent.lock();

    if(parent)
    {
        Matrix mat;
        parent->_transform->GetLocalToWorldMatrix_BottomUp(mat);
        _localPosition = vec3::Transform(worldPos,  mat.Invert());
    }
    else
        _localPosition = worldPos;

    _needLocalSRTUpdated = true;
    return worldPos;
}

vec3 Transform::GetWorldScale()
{
    vec3 totalScale = _localScale;
    auto parent = GetOwner();
    if (parent) parent = parent->parent.lock();
    while (parent)
    {
        totalScale = totalScale * parent->_transform->_localScale;
        parent = parent->parent.lock();
    }
    return totalScale;
}

const vec3& Transform::SetWorldScale(const vec3& scale)
{
    auto parent = GetOwner();
    if (parent) parent = parent->parent.lock();
    if (parent)
    {
        vec3 ws = parent->_transform->GetWorldScale();
        _localScale = (ws / (ws * ws)) * scale;
    }
    else
        _localScale = scale;
    _needLocalSRTUpdated = true;
    return scale;
}

vec3 Transform::GetWorldEuler()
{
    return GetWorldRotation().ToEuler();
}

Quaternion Transform::GetWorldRotation()
{
    Quaternion totalQuat = _localRotation;
    auto parent = GetOwner();
    if (parent)
        parent = parent->parent.lock();
    while (parent)
    {
        totalQuat = totalQuat * parent->_transform->_localRotation;
        parent = parent->parent.lock();
    }

    totalQuat.Normalize();
    return totalQuat;
}

const Quaternion& Transform::SetWorldRotation(const Quaternion& quaternion)
{
    auto parent = GetOwner();
    if (parent)
        parent = parent->parent.lock();
    if (parent)
    {
        Quaternion result;
        parent->_transform->GetWorldRotation().Inverse(result);
        _localRotation = quaternion * result;
        _localRotation.Normalize();
    }
    else
        _localRotation = quaternion;



    _needLocalSRTUpdated = true;

    return quaternion;
}

bool Transform::GetLocalToWorldMatrix(OUT Matrix& localToWorldMatrix)
{
    if (auto owner = GetOwner(); owner)
    {
        if (CheckLocalToWorldMatrixUpdate())
        {
            auto root = owner->rootParent.lock();
            bool result = root->_transform->TopDownLocalToWorldUpdate(Matrix::Identity);
            localToWorldMatrix = this->_localToWorldMatrix;
            return result;
        }

        localToWorldMatrix = this->_localToWorldMatrix;
        return false;
    }

    Matrix dummyLocalSRT;
    GetLocalSRTMatrix(dummyLocalSRT);
    if(CheckNeedLocalSRTUpdate()) {
        this->_localToWorldMatrix = this->_localSRTMatrix;
        _needLocalMatrixUpdated = false;
        _isLocalToWorldChanged = true;
        localToWorldMatrix = this->_localToWorldMatrix;
        return true;
    }
    localToWorldMatrix = this->_localToWorldMatrix;
    return false;
}

bool Transform::GetLocalToWorldMatrix_BottomUp(Matrix& localToWorld)
{
    if (CheckLocalToWorldMatrixUpdate())
    {
        bool result = BottomUpLocalToWorldUpdate();
        localToWorld = _localToWorldMatrix;
		return result;
    }

    localToWorld = _localToWorldMatrix;
    return false;
}

bool Transform::GetLocalSRTMatrix(Matrix& localSRT)
{
    if (CheckNeedLocalSRTUpdate())
    { 

        vec3 pivotCorrection = _pivotOffset - vec3::Transform(_pivotOffset, Matrix::CreateFromQuaternion(_localRotation));

        _localSRTMatrix =
            Matrix::CreateScale(_localScale) *
            Matrix::CreateFromQuaternion(_localRotation) *
            Matrix::CreateTranslation(_localPosition + pivotCorrection);

        _needLocalSRTUpdated = false;
        _needLocalMatrixUpdated = true;
        localSRT = _localSRTMatrix;
        _isLocalSRTChanged = true;
		return true;
    }


    localSRT = _localSRTMatrix;
    return false;
}

bool Transform::SetLocalSRTMatrix(Matrix& localSRT)
{
    vec3 position;
    Quaternion rotation;
    vec3 scale;
    // 행렬을 위치, 회전, 스케일로 분해
    _localSRTMatrix = localSRT;
    _needLocalMatrixUpdated = true;

    if (localSRT.Decompose(scale, rotation, position))
    {
        _localScale = scale;
        _localRotation = rotation;
        _localPosition = position;
        _needLocalSRTUpdated = false;
        return true;
    }
    std::cout << "분해 실패\n";
    return false;
}

bool Transform::CheckNeedLocalSRTUpdate() const
{
    return _needLocalSRTUpdated;
}

bool Transform::CheckNeedLocalMatrixUpdate() const
{
    return CheckNeedLocalSRTUpdate() || _needLocalMatrixUpdated;
}

bool Transform::CheckLocalToWorldMatrixUpdate()
{
    if(CheckNeedLocalMatrixUpdate() || _needLocalToWorldUpdated)
        return true;

    for(auto currentObj = GetOwner()->parent.lock(); currentObj != nullptr; currentObj = currentObj->parent.lock())
        if(currentObj->_transform->CheckNeedLocalMatrixUpdate() || currentObj->_transform->_needLocalToWorldUpdated)
            return true;
    return false;
}

bool Transform::TopDownLocalToWorldUpdate(const Matrix& parentLocalToWorld, bool isParentUpdate)
{
    Matrix dummy;
    bool localUpdated = GetLocalSRTMatrix(dummy);
    bool finalUpdate = CheckNeedLocalMatrixUpdate() || _needLocalToWorldUpdated || isParentUpdate;
    _needLocalMatrixUpdated = false;

    if (finalUpdate)
    {
        _localToWorldMatrix = _localSRTMatrix * parentLocalToWorld;
        _needLocalToWorldUpdated = false;
        _isLocalToWorldChanged = true;
    }

	bool childUpdate = false;
    if(auto owner = GetOwner(); owner)
        for(auto& childWeak : owner->_childs)
            if(auto child = childWeak.lock())
                childUpdate |= child->_transform->TopDownLocalToWorldUpdate(_localToWorldMatrix, finalUpdate);

    return finalUpdate || childUpdate;
}

bool Transform::BottomUpLocalToWorldUpdate()
{
    auto owner = GetOwner();
    std::shared_ptr<GameObject> parent;
    if (owner)
        parent = owner->parent.lock();

    Matrix dummyLocalSRT;
    GetLocalSRTMatrix(dummyLocalSRT);
    bool localUpdated = CheckNeedLocalMatrixUpdate();
    _needLocalMatrixUpdated = false;

    if (parent)
    {
        bool parentUpdate = parent->_transform->BottomUpLocalToWorldUpdate();

        if (localUpdated || _needLocalToWorldUpdated || parentUpdate)
        {
            _localToWorldMatrix = _localSRTMatrix * parent->_transform->_localToWorldMatrix;
            _needLocalToWorldUpdated = false;
            _isLocalToWorldChanged = true;

            if(owner)
	            for(auto& childWeak : owner->_childs)
	                if(auto child = childWeak.lock())
	                    child->_transform->_needLocalToWorldUpdated = true;

            return true;
        }
    }

    else if (localUpdated || _needLocalToWorldUpdated)
    {
        _localToWorldMatrix = _localSRTMatrix;
        _needLocalToWorldUpdated = false;
        _isLocalToWorldChanged = true;

        if(owner)
	        for(auto& childWeak : owner->_childs)
	            if(auto child = childWeak.lock())
	                child->_transform->_needLocalToWorldUpdated = true;

        return true;
    }
    return false;
}



void Transform::LookUp(const vec3& dir, const vec3& up, const Quaternion& orgin)
{
    SetWorldRotation(orgin * LookToQuaternion(dir, up));
}

void Transform::LookUpSmooth(const vec3& dir, const vec3& up, float speed, const Quaternion& orgin)
{
    Quaternion target = orgin * LookToQuaternion(dir, up);
    Quaternion current = GetWorldRotation();
    float delta = speed * Time::main->GetDeltaTime();
    Quaternion result = Quaternion::Slerp(current, target, delta);
    SetWorldRotation(result);
}



vec3 Transform::LocalToWorld_Position(const vec3& value)
{
    Matrix mat;
    GetLocalToWorldMatrix_BottomUp(mat);
    return vec3::Transform(value, mat);
}

vec3 Transform::LocalToWorld_Direction(const vec3& value)
{
    Matrix mat;
    GetLocalToWorldMatrix_BottomUp(mat);
    return vec3::TransformNormal(value, mat);
}

Quaternion Transform::LocalToWorld_Quaternion(const Quaternion& value)
{
    Quaternion result = value * GetWorldRotation();
    result.Normalize();
    return result;
}

vec3 Transform::WorldToLocal_Position(const vec3& value)
{
    Matrix mat;
    GetLocalToWorldMatrix_BottomUp(mat);
    mat.Invert(mat);
    return vec3::Transform(value, mat);
}

vec3 Transform::WorldToLocal_Direction(const vec3& value)
{
    Matrix mat;
    GetLocalToWorldMatrix_BottomUp(mat);
    mat.Invert(mat);
    return vec3::TransformNormal(value, mat);
}

Quaternion Transform::WorldToLocal_Quaternion(const Quaternion& value)
{
    Quaternion result;
    GetWorldRotation().Inverse(result);
    result = value * result;
    result.Normalize();
    return result;
}