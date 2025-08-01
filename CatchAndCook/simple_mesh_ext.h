#pragma once

#include "simple_mesh_LH.h"

namespace DirectX::SimpleMath
{
    inline bool Equals(const float& a, const float& b) noexcept
    {
        return std::abs(a - b) <= 1e-6f;
    }

    inline bool Equals(const double& a, const double& b) noexcept
    {
        return std::abs(a - b) <= 1e-14f;
    }

    inline bool Equals(const Vector3& a, const Vector3& b) noexcept
    {
        return (a - b).LengthSquared() <= 1e-6f;
    }

    inline bool Equals(const Vector2& a, const Vector2& b) noexcept
    {
        return (a - b).LengthSquared() <= 1e-6f;
    }

    inline void ToEuler(const Quaternion& quat, Vector3& euler) noexcept
    {
        float x = quat.x;
        float y = quat.y;
        float z = quat.z;
        float w = quat.w;

        // Yaw (Z축 회전)
        float siny_cosp = 2 * (w * z + x * y);
        float cosy_cosp = 1 - 2 * (y * y + z * z);
        float yaw = std::atan2(siny_cosp, cosy_cosp); // 아크탄젠트 계산

        // Pitch (Y축 회전)
        float sinp = 2 * (w * y - z * x);
        float pitch;
        if (std::abs(sinp) >= 1)
            pitch = std::copysign(XM_PI / 2, sinp); // Gimbal lock 처리
        else
            pitch = std::asin(sinp); // 일반적인 경우의 Pitch 계산

        // Roll (X축 회전)
        float sinr_cosp = 2 * (w * x + y * z);
        float cosr_cosp = 1 - 2 * (x * x + y * y);
        float roll = std::atan2(sinr_cosp, cosr_cosp); // 아크탄젠트 계산

        // 오일러 각을 Vector3로 저장 (Roll, Pitch, Yaw 순서로)
        euler = Vector3(roll, pitch, yaw); // 순서: Roll, Pitch, Yaw
    };

    inline Vector3 QuaternionToEuler(const Quaternion& quaternion)
    {
        // 쿼터니언을 회전 행렬로 변환
        XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(quaternion);

        // 행렬에서 오일러 각 추출
        float pitch, yaw, roll;

        // 행렬에서 오일러 각 추출
        pitch = std::asin(-rotationMatrix.r[2].m128_f32[0]);

        if (std::cos(pitch) > 0.0001)
        {
            // 기울기(Gimbal Lock) 체크
            yaw = std::atan2(rotationMatrix.r[2].m128_f32[1], rotationMatrix.r[2].m128_f32[2]);
            roll = std::atan2(rotationMatrix.r[1].m128_f32[0], rotationMatrix.r[0].m128_f32[0]);
        }
        else
        {
            yaw = std::atan2(-rotationMatrix.r[0].m128_f32[2], rotationMatrix.r[1].m128_f32[1]);
            roll = 0.0f;
        }

        return Vector3(pitch, yaw, roll);
    }

    inline Quaternion CreateRotationZXY(float rollZ, float pitchX, float yawY)
    {
        // Z, X, Y 각 축에 대한 회전 쿼터니언 생성
        Quaternion rotationZ = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, rollZ); // Z축 회전 (Roll)
        Quaternion rotationX = Quaternion::CreateFromAxisAngle(Vector3::UnitX, pitchX); // X축 회전 (Pitch)
        Quaternion rotationY = Quaternion::CreateFromAxisAngle(Vector3::UnitY, yawY); // Y축 회전 (Yaw)

        // Z -> X -> Y 순서로 결합된 최종 회전 쿼터니언 생성
        Quaternion finalRotation = rotationY * rotationX * rotationZ;
        //Quaternion finalRotation = rotationZ * rotationX * rotationY;
        return finalRotation;
    }

    inline Quaternion CreateRotationZXY(const Vector3& euler)
    {
        return CreateRotationZXY(euler.z, euler.x, euler.y);
    }

    inline Quaternion LookToQuaternion(const Vector3& _forward, const Vector3& up = Vector3::UnitY)
    {
        // forward 벡터 정규화
        Vector3 forward = _forward;
        forward.Normalize();

        // 오른쪽(right) 벡터 계산
        Vector3 right = up.Cross(forward);
        right.Normalize();

        // 새로운 up 벡터 재계산
        Vector3 recalculatedUp = forward.Cross(right);

        // 회전 행렬 구성
        Matrix lookToMatrix(
            right.x, right.y, right.z, 0.0f,
            recalculatedUp.x, recalculatedUp.y, recalculatedUp.z, 0.0f,
            forward.x, forward.y, forward.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        // 회전 행렬에서 쿼터니언 생성
        return Quaternion::CreateFromRotationMatrix(lookToMatrix);
    }

    inline Matrix CreateViewportMatrix(Viewport view) {
        // Scale factors
        float scaleX = view.width / 2.0f;
        float scaleY = view.height / 2.0f;
        float scaleZ = view.maxDepth - view.minDepth;

        // Translation factors
        float offsetX = 0 + scaleX;//view.x
        float offsetY = 0 + scaleY;//view.y
        float offsetZ = view.minDepth;

        // Viewport matrix
        return Matrix(
            scaleX, 0.0f, 0.0f, 0.0f,  // X scale
            0.0f, -scaleY, 0.0f, 0.0f,  // Y scale (Y-axis inverted)
            0.0f, 0.0f, scaleZ, 0.0f,  // Z scale
            offsetX, offsetY, offsetZ, 1.0f   // Translation
        );
    }

    inline BoundingOrientedBox TransformBoundingOrientedBox(const BoundingOrientedBox& box, const Matrix& mat, const Vector3& worldScale, const Quaternion& worldRot)
    {
        BoundingOrientedBox box2;
        Vector3 corners[8];
        box.GetCorners(corners);

        // 2. 각 꼭지점에 변환 행렬 적용
        Vector3 center;
        for (int i = 0; i < 8; ++i)
        {
            corners[i] = Vector3::Transform(corners[i], mat);
            center += corners[i];
        }
        center /= 8;
        box2.Center = center;
        box2.Extents = box.Extents * worldScale;
        box2.Orientation = worldRot;
        return box2;
    }
}
