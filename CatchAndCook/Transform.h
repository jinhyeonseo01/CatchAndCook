#pragma once
#include "Component.h"

class Transform : public Component
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
	void Rendering() override;
	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void DebugRendering() override;


public:
    vec3 _localPosition;
    vec3 _localScale;
    Quaternion _localRotation;

    vec3 _forward;
    vec3 _up;
    vec3 _right;

    vec3 forward(const vec3& dir = Vector3::Zero);
    vec3 up(const vec3& dir = Vector3::Zero);
    vec3 right(const vec3& dir = Vector3::Zero);

    vec3 GetLocalEuler();
    const vec3& SetLocalEuler(const vec3& euler);

    vec3 GetLocalPosition();
    const vec3& SetLocalPosition(const vec3& worldPos);
    vec3 GetLocalScale();
    const vec3& SetLocalScale(const vec3& worldScale);
    Quaternion GetLocalRotation();
    const Quaternion& SetLocalRotation(const Quaternion& quaternion);

    vec3 GetWorldPosition();
    const vec3& SetWorldPosition(const vec3& worldPos);
    vec3 GetWorldScale();
    const vec3& SetWorldScale(const vec3& worldScale);
    Quaternion GetWorldRotation();
    const Quaternion& SetWorldRotation(const Quaternion& quaternion);

    Matrix _prevLocalSRTMatrix = Matrix::Identity;
    Matrix localSRTMatrix = Matrix::Identity; // prev�� ���� ����/ ���Ž� islocal�ӽñ� true �ƴϸ� false
    Matrix localToWorldMatrix = Matrix::Identity;
    bool GetLocalToWorldMatrix(Matrix& localToWorldMatrix);
    bool GetLocalToWorldMatrix_BottomUp(Matrix& localToWorld);
    bool GetLocalSRTMatrix(Matrix& localSRT);
    bool SetLocalSRTMatrix(Matrix& localSRT);
    bool CheckLocalSRTUpdate();
    bool CheckLocalMatrixUpdate();
    bool CheckLocalToWorldMatrixUpdate();
    void TopDownLocalToWorldUpdate(const Matrix& parentLocalToWorld, bool isParentUpdate = false);
    bool BottomUpLocalToWorldUpdate();

    void LookUp(const vec3& dir, const vec3& up);

    bool isLocalSRTChanged = true; //�̰� Ȱ��ȭ�� �� �����Ʈ���� ����.isLocalToWorldChanged �̰� Ȱ��ȭ
    bool isLocalToWorldChanged = true; //�θ� local ������Ʈ or �θ� world ����� �̰� true.worldtrs����.

	bool _needLocalUpdated = true; // �� �ڽ��� SRT ���� �ؾ���.
	bool _needLocalToWorldUpdated = true; // �θ� ������Ʈ ������ ���� ����Ǿ������ ǥ��, ���������� ������ ���� �ٸ���. ������ �ڱ� �����̶�, �� �������̶� ������ �ٲ�µ�, �̰� ���� �ٲ�� ������ �Ȳ���

    vec3 LocalToWorld_Position(const vec3& value);
    vec3 LocalToWorld_Direction(const vec3& value);
    Quaternion LocalToWorld_Quaternion(const Quaternion& value);

    vec3 WorldToLocal_Position(const vec3& value);
    vec3 WorldToLocal_Direction(const vec3& value);
    Quaternion WorldToLocal_Quaternion(const Quaternion& value);
};


