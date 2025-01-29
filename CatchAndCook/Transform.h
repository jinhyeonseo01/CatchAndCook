#pragma once
#include "Component.h"

struct CBufferContainer;

class Transform : public Component, public RenderObjectSetter
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
	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;

    void PushData() override;
	virtual void SetData(Material* material = nullptr) override;

public:

    vec3 SetForward(const vec3& dir );
    vec3 SetUp(const vec3& dir );
    vec3 SetRight(const vec3& dir );

    vec3 GetForward();
    vec3 GetUp();
    vec3 GetRight();

    vec3 GetLocalEuler() const;
    const vec3& SetLocalRotation(const vec3& euler);
    vec3 GetLocalPosition() const;
    const vec3& SetLocalPosition(const vec3& worldPos);
    vec3 GetLocalScale() const;
    const vec3& SetLocalScale(const vec3& worldScale);
    Quaternion GetLocalRotation() const;
    const Quaternion& SetLocalRotation(const Quaternion& quaternion);

    vec3 GetWorldPosition();
    const vec3& SetWorldPosition(const vec3& worldPos);
    vec3 GetWorldScale();
    const vec3& SetWorldScale(const vec3& worldScale);
    Quaternion GetWorldRotation();
    const Quaternion& SetWorldRotation(const Quaternion& quaternion);

    bool GetLocalToWorldMatrix(OUT Matrix& localToWorldMatrix);
    bool GetLocalToWorldMatrix_BottomUp(Matrix& localToWorld);
    bool GetLocalSRTMatrix(Matrix& localSRT);
    bool SetLocalSRTMatrix(Matrix& localSRT);
    bool CheckLocalSRTUpdate() const;
    bool CheckLocalMatrixUpdate() const;
    bool CheckLocalToWorldMatrixUpdate();
    void TopDownLocalToWorldUpdate(const Matrix& parentLocalToWorld, bool isParentUpdate = false);
    bool BottomUpLocalToWorldUpdate();

    void LookUp(const vec3& dir, const vec3& up);

    vec3 LocalToWorld_Position(const vec3& value);
    vec3 LocalToWorld_Direction(const vec3& value);
    Quaternion LocalToWorld_Quaternion(const Quaternion& value);

    vec3 WorldToLocal_Position(const vec3& value);
    vec3 WorldToLocal_Direction(const vec3& value);
    Quaternion WorldToLocal_Quaternion(const Quaternion& value);


public:
    bool _isLocalSRTChanged = true; //�̰� Ȱ��ȭ�� �� �����Ʈ���� ����.isLocalToWorldChanged �̰� Ȱ��ȭ
    bool _isLocalToWorldChanged = true; //�θ� local ������Ʈ or �θ� world ����� �̰� true.worldtrs����.

private:
    vec3 _localPosition = vec3::Zero;
    vec3 _localScale = vec3::One;
    Quaternion _localRotation = Quaternion::Identity;

    vec3 _forward = vec3::Forward;
    vec3 _up = vec3::Up;
    vec3 _right = vec3::Right;

    Matrix _prevLocalSRTMatrix = Matrix::Identity;
    Matrix _localSRTMatrix = Matrix::Identity; // prev�� ���� ����/ ���Ž� islocal�ӽñ� true �ƴϸ� false
    Matrix _localToWorldMatrix = Matrix::Identity;

    bool _needLocalUpdated = true; // �� �ڽ��� SRT ���� �ؾ���.
    bool _needLocalToWorldUpdated = true; // �θ� ������Ʈ ������ ���� ����Ǿ������ ǥ��, ���������� ������ ���� �ٸ���. ������ �ڱ� �����̶�, �� �������̶� ������ �ٲ�µ�, �̰� ���� �ٲ�� ������ �Ȳ���
    CBufferContainer* _cbufferContainer;
};


