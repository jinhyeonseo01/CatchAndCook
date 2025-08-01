#include "pch.h"
#include "simple_mesh_ext.h"
/*
namespace DirectX
{
	namespace SimpleMath
	{
		inline void ToEuler(const Quaternion& quat, Vector3& euler) noexcept
		{
			// Pitch
			float sinr_cosp = 2.0f * (quat.w * quat.x + quat.y * quat.z);
			float cosr_cosp = 1.0f - 2.0f * (quat.x * quat.x + quat.y * quat.y);
			euler.z = atan2f(sinr_cosp, cosr_cosp); // Roll

			// Yaw
			float sinp = 2.0f * (quat.w * quat.y - quat.z * quat.x);
			sinp = std::clamp(sinp, -1.0f, 1.0f);
			euler.y = asinf(sinp); // Pitch

			// Yaw
			float siny_cosp = 2.0f * (quat.w * quat.z + quat.x * quat.y);
			float cosy_cosp = 1.0f - 2.0f * (quat.y * quat.y + quat.z * quat.z);
			euler.x = atan2f(siny_cosp, cosy_cosp); // Yaw
		};
	}
}
*/
