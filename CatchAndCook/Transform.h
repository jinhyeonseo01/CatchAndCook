#pragma once
#include "Component.h"

struct CBufferContainer;
class Material;

class Transform : public Component, public RenderStructuredSetter
{
public:
	Transform();
	virtual ~Transform() override;

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
    void CollisionEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other) override;

    bool IsExecuteAble() override;
	virtual void SetData(StructuredBuffer* buffer = nullptr ,Material* material=nullptr) override;

public:
    vec3 SetForward(const vec3& dir );
    vec3 SetUp(const vec3& dir );
    vec3 SetRight(const vec3& dir );

    vec3 GetForward();
    vec3 GetUp();
    vec3 GetRight();

    vec3 GetLocalEuler() const;
    const vec3& SetLocalRotation(const vec3& euler);
    vec3& GetLocalPosition();
    const vec3& SetLocalPosition(const vec3& worldPos);
    vec3 GetLocalScale() const;
    const vec3& SetLocalScale(const vec3& worldScale);
    Quaternion GetLocalRotation() const;
    const Quaternion& SetLocalRotation(const Quaternion& quaternion);

    vec3 GetWorldPosition();
    const vec3& SetWorldPosition(const vec3& worldPos);
    vec3 GetWorldScale();
    const vec3& SetWorldScale(const vec3& worldScale);
    vec3 GetWorldEuler();
    Quaternion GetWorldRotation();
    const Quaternion& SetWorldRotation(const Quaternion& quaternion);

    void SetVelocity(const vec3& velocity) { _velocity = velocity; }
    void SetVelocity(vec3&& velocity) { _velocity = std::move(velocity);}
	vec3 GetVelocity() const { return _velocity; }
     
    bool GetLocalToWorldMatrix(OUT Matrix& localToWorldMatrix);
    bool GetLocalToWorldMatrix_BottomUp(Matrix& localToWorld);
    bool GetLocalSRTMatrix(Matrix& localSRT);
    bool SetLocalSRTMatrix(Matrix& localSRT);
    bool CheckNeedLocalSRTUpdate() const;
    bool CheckNeedLocalMatrixUpdate() const;
    bool CheckLocalToWorldMatrixUpdate();
    bool TopDownLocalToWorldUpdate(const Matrix& parentLocalToWorld, bool isParentUpdate = false);
    bool BottomUpLocalToWorldUpdate();

    void LookUp(const vec3& dir, const vec3& up, const Quaternion& orgin = Quaternion::Identity);
	void LookUpSmooth(const vec3& dir, const vec3& up, float speed , const Quaternion& orgin = Quaternion::Identity);

    vec3 LocalToWorld_Position(const vec3& value);
    vec3 LocalToWorld_Direction(const vec3& value);
    Quaternion LocalToWorld_Quaternion(const Quaternion& value);

    vec3 WorldToLocal_Position(const vec3& value);
    vec3 WorldToLocal_Direction(const vec3& value);
    Quaternion WorldToLocal_Quaternion(const Quaternion& value);

	bool IsLocalSRTChanged() const { return _isLocalSRTChanged; }
	bool IsLocalToWorldChanged() const { return _isLocalToWorldChanged; }
    void SetPivotOffset(const vec3& pivotOffset) { _pivotOffset = pivotOffset; }

private:



	vec3 _velocity = vec3::Zero;
    vec3 _pivotOffset = vec3::Zero;
    vec3 _localPosition = vec3::Zero;
    vec3 _localScale = vec3::One;
    Quaternion _localRotation = Quaternion::Identity;

    vec3 _forward = vec3::Forward;
    vec3 _up = vec3::Up;
    vec3 _right = vec3::Right;

    Matrix _localSRTMatrix = Matrix::Identity; // prev랑 비교후 갱신/ 갱신시 islocal머시기 true 아니면 false
    Matrix _localToWorldMatrix = Matrix::Identity;

    bool _isLocalSRTChanged = true; // 이전 프레임과 SRT가 달라졌을때
    bool _isLocalToWorldChanged = true; // 이전 프레임과 L2W가 달라졌을때

    bool _needLocalSRTUpdated = true; // 나 자신이 SRT 갱신 해야해.
    bool _needLocalMatrixUpdated = true;
    bool _needLocalToWorldUpdated = true; // 부모가 업데이트 됬을때 내가 변경되어야함을 표기, 위에꺼랑은 역할이 조금 다른게. 위에껀 자기 기준이라, 전 프레임이랑 같으면 바뀌는데, 이건 내가 바뀌기 전까지 안꺼짐
    CBufferContainer* _cbufferContainer;

    friend class SkinnedHierarchy;
	friend class ImguiManager;


};


